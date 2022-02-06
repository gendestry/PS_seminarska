#pragma OPENCL EXTENSION cl_khr_fp64: enable
#pragma OPENCL EXTENSION cl_khr_int64_base_atomics: enable
void AtomicAdd(local double *val, double delta) {
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
	} while (atom_cmpxchg ( (volatile local ulong *)val, old.i, new.i) != old.i);
}

typedef struct SparseValue{
	int row;
	int col;
	double value;
} SparseValue;

kernel void matrix(global double* newRank, global double* oldRank, global SparseValue* sparseMatrix, global int* offsets, local double* ranks, double d, double offset) {
	int id = get_local_id(0);
	int gsize = get_local_size(0);
	int gid = get_group_id(0);

	int from = offsets[gid];
	int to = offsets[gid+1];
	for(int i = from + id; i < to; i += gsize) {
		SparseValue sv = sparseMatrix[i];
		//printf("%d %d %d %d %d\n", from, to, gid, id, i);
		ranks[sv.row] = 0;
		//newRank[sv.row] += sv.value * oldRank[sv.col];
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	for(int i = from + id; i < to; i += gsize) {
		SparseValue sv = sparseMatrix[i];
		//printf("%d, %f %f %f\n", sv.row, sv.value * oldRank[sv.col], sv.value, oldRank[sv.col]);
		AtomicAdd(&ranks[sv.row], sv.value * oldRank[sv.col]);
		//newRank[sv.row] += sv.value * oldRank[sv.col];
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	if(id == 0) {
		int prev = -1;
		for(int i = from; i < to; i++) {
			//printf("%d, %f\n", i, ranks[i]);
			int row = sparseMatrix[i].row;
			newRank[row] = ranks[row]*d + offset;
		}
	}
}

kernel void matrixUpdateRank(global double* newRank, global double* oldRank, int rankLength, global double* rankDiff) {
	int id = get_global_id(0);
	if(id < rankLength) {
		//rankDiff += abs_diff(oldRank[id]-newRank[id]);
		//oldRank[id] = newRank[id]*d + offset;
		rankDiff[id] = fabs(oldRank[id]-newRank[id]);
		oldRank[id] = newRank[id];
		printf("%d %f\n", id, oldRank[id]);
		//printf("%d, %f\n", id, rankDiff[id]);
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


/*kernel void graph(global unsigned int* buffer) {

}*/