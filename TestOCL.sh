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

for WGS in 1 2 4 8 16 32 64 128 256
do
	echo $WGS
	for ITERS in {1..5} 
	do
		bin/PageRank-OpenCL $WGS 
	done
done