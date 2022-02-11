#!/bin/bash

#SBATCH --job-name=PageRank-OpenCL
#SBATCH --output=log/opencl-tests.log
#SBATCH --ntasks=1
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --time=8:00:00
#SBATCH --mem-per-cpu=8000
#SBATCH --constraint=gpu
#SBATCH --reservation=fri

for WGS in 16 32 64 128 256 512 1024
do
	echo "WGS:" $WGS
	for DATATYPE in 0 1
	do
		echo "#DATATYPE:" $DATATYPE
		for VERTICES in 10000 20000 50000 100000 200000 500000 -1
		do
			echo "##VERTICES:" $VERTICES
			for ITERS in {1..25}
			do
				bin/PageRank-OpenCL 1e-7 $DATATYPE $VERTICES $WGS
			done
		done
	done
done