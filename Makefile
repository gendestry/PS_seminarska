N ?= 16

.phony: clean

all: seq omp

# release versions
seq: src/Parser.cpp src/PageRank.cpp
	g++ src/Parser.cpp src/PageRank.cpp -O3 --std=c++17 -o bin/PageRank

omp: src/Parser.cpp src/PageRank-OpenMP.cpp
	g++ src/Parser.cpp src/PageRank-OpenMP.cpp -fopenmp -O3 --std=c++17 -o bin/PageRank-OpenMP


# debug variants
dseq: src/Parser.cpp src/PageRank.cpp
	g++ src/Parser.cpp src/PageRank.cpp -g --std=c++17 -o bin/PageRank

domp: src/Parser.cpp src/PageRank-OpenMP.cpp
	g++ src/Parser.cpp src/PageRank-OpenMP.cpp -fopenmp -g --std=c++17 -o bin/PageRank-OpenMP


# running
run_seq: seq
	srun -n1 --cpus-per-task=1 --reservation=fri bin/PageRank

run_omp: omp
	srun -n1 --cpus-per-task=$(N) --reservation=fri bin/PageRank-OpenMP

# clean
clean:
	rm bin/*
