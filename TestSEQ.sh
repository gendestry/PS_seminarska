#!/bin/bash

#SBATCH --job-name=PageRank-SEQ
#SBATCH --output=log/seq-tests.log
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --time=8:00:00
#SBATCH --mem-per-cpu=8000
#SBATCH --constraint=AMD
#SBATCH --reservation=fri

for ITERS in {1..5} 
do
	bin/PageRank
done