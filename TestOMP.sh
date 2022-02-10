#!/bin/bash

#SBATCH --job-name=PageRank-OpenMP
#SBATCH --output=log/openmp-tests.log
#SBATCH --ntasks=64
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --time=8:00:00
#SBATCH --mem-per-cpu=8000
#SBATCH --constraint=AMD
#SBATCH --reservation=fri

export OMP_PLACES=cores
export OMP_PROC_BIND=close

for THREADS in 1 2 4 8 16 32 64
do
	export OMP_NUM_THREADS=$THREADS
	echo "NUM_THREADS:" $THREADS
	for ITERTYPE in 0 1
	do
		echo "#ITERTYPE:" $ITERTYPE
		for DATATYPE in 0 1
		do
			echo "##DATATYPE:" $DATATYPE
			for ERROR in 1e-5 1e-6 1e-7
			do
				echo "###ERROR:" $ERROR
				for ITERS in {1..5} 
				do
					bin/PageRank-OpenMP $ERROR $DATATYPE $ITERTYPE
				done
			done
		done
	done
done
