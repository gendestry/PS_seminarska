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
		for VERTICES in 10000 20000 50000 100000 200000 500000 -1
		do
			echo "##VERTICES:" $VERTICES
			for ITERS in {1..25}
			do
				bin/PageRank 1e-7 $DATATYPE $VERTICES $ITERTYPE
			done
		done
	done
done
