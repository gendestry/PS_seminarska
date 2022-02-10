#!/bin/bash

#SBATCH --job-name=PageRank-SEQ
#SBATCH --output=log/seq-tests.log
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --time=8:00:00
#SBATCH --mem-per-cpu=8000
#SBATCH --constraint=AMD
#SBATCH --reservation=fri

for ITERTYPE in 0 1
do
	echo "ITERTYPE:" $ITERTYPE
	for DATATYPE in 0 1
	do
		echo "#DATATYPE:" $DATATYPE
		for ERROR in 1e-5 1e-6 1e-7
		do
			echo "##ERROR:" $ERROR
			for ITERS in {1..5} 
			do
				bin/PageRank $ERROR $DATATYPE $ITERTYPE
			done
		done
	done
done
