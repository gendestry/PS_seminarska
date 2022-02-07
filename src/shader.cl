#pragma OPENCL EXTENSION cl_khr_fp64: enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics: enable
// Stackoverflow magic
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

typedef struct SparseValue{
	int row;
	int col;
	double value;
} SparseValue;

// Sparse matrix multiplication
kernel void matrix(global double* newRank, global double* oldRank, global SparseValue* sparseMatrix, int O) {
	int id = get_global_id(0);
	if(id < O){
		SparseValue sv = sparseMatrix[id];
		AtomicAdd(&newRank[sv.row], sv.value * oldRank[sv.col]);
	}
}

// Update ranks, calculate difference
kernel void matrixUpdateRank(global double* newRank, global double* oldRank, int rankLength, double d, double offset, global double* rankDiff) {
	int id = get_global_id(0);
	if(id < rankLength) {
		double oldOldRank = oldRank[id];
		oldRank[id] = newRank[id]*d + offset;
		rankDiff[id] = fabs(oldRank[id]-oldOldRank);
		newRank[id] = 0;
	}
}

kernel void rankDiffReduction(global double* rankDiff, int N, global double* partials, local double* diff) {
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