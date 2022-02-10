// check to see the type
#ifndef USE_DOUBLE
	#define USE_DOUBLE 1
#endif

#if USE_DOUBLE == 0
	#define TYPE float
#else
	#define TYPE double
	#pragma OPENCL EXTENSION cl_khr_fp64: enable
	#pragma OPENCL EXTENSION cl_khr_int64_base_atomics: enable
#endif

// Stackoverflow magic
#if USE_DOUBLE == 1
	void AtomicAdd(global double *val, double delta) {
		union {
			double f;
			ulong  i;
		} old;
		union {
			double f;
			ulong  i;
		} new;
		do {
			old.f = *val;
			new.f = old.f + delta;
		} while (atom_cmpxchg ( (volatile global ulong *)val, old.i, new.i) != old.i);
	}
#else
	void AtomicAdd(volatile global float* val, const float delta) {
		union {
			float f;
			uint  i;
		} old;
		union {
			float f;
			uint  i;
		} new;
		do {
			old.f = *val;
			new.f = old.f + delta;
		} while (atom_cmpxchg ( (volatile global uint *)val, old.i, new.i) != old.i);
	}
#endif
typedef struct SparseValue{
	int row;
	int col;
	TYPE value;
} SparseValue;

// Sparse matrix multiplication
kernel void matrix(global TYPE* newRank, global TYPE* oldRank, global SparseValue* sparseMatrix, int O) {
	int id = get_global_id(0);
	if(id < O){
		SparseValue sv = sparseMatrix[id];
		AtomicAdd(&newRank[sv.row], sv.value * oldRank[sv.col]);
	}
}

// Update ranks, calculate difference
kernel void matrixUpdateRank(global TYPE* newRank, global TYPE* oldRank, int rankLength, TYPE d, TYPE offset, global TYPE* rankDiff) {
	int id = get_global_id(0);
	if(id < rankLength) {
		TYPE oldOldRank = oldRank[id];
		oldRank[id] = newRank[id]*d + offset;
		rankDiff[id] = fabs(oldRank[id]-oldOldRank);
		newRank[id] = 0;
	}
}

kernel void rankDiffReduction(global TYPE* rankDiff, int N, global TYPE* partials, local TYPE* diff) {
	int id = get_global_id(0);
	int lid = get_local_id(0);
	int gid = get_group_id(0);
	int gsize = get_local_size(0);
	diff[lid] = 0;
	if(id < N){
		diff[lid] = rankDiff[id];
	}
	int floorPow2 = get_local_size(0);
	while (floorPow2 & (floorPow2-1))
		floorPow2 &= floorPow2-1;
	if (gsize != floorPow2)	{
		if (lid >= floorPow2) {
            diff[lid - floorPow2] += diff[lid];
		}
		barrier(CLK_LOCAL_MEM_FENCE);
    }
	int idxStep;

	for (idxStep = floorPow2 >> 1; idxStep > 32; idxStep >>= 1) {
		if (lid < idxStep)
			diff[lid] += diff[lid+idxStep];
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	for ( ; idxStep > 0; idxStep >>= 1) {
		if (lid < idxStep)
			diff[lid] += diff[lid+idxStep];
	}

	if (lid == 0) {		
		partials[get_group_id(0)] = diff[0];
	}
}