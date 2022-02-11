N ?= 16

.phony: clean

all: seq omp ocl

seq: bin/PageRank
omp: bin/PageRank-OpenMP
ocl: bin/PageRank-OpenCL

dseq: bin/PageRank-d
domp: bin/PageRank-OpenMP-d
docl: bin/PageRank-OpenCL-d

# release versions
bin/PageRank: src/Parser.cpp src/PageRank.cpp
	g++ $^ -O3 --std=c++17 -o $@

bin/PageRank-OpenMP: src/Parser.cpp src/PageRank-OpenMP.cpp
	g++ $^ -fopenmp -O3 --std=c++17 -o $@

bin/PageRank-OpenCL: src/Parser.cpp src/PageRank-OpenCL.cpp
	g++ $^ -lOpenCL -O3 --std=c++17 -o $@


# debug variants
bin/PageRank-d: src/Parser.cpp src/PageRank.cpp
	g++ $^ -g --std=c++17 -o $@

bin/PageRank-OpenMP-d: src/Parser.cpp src/PageRank-OpenMP.cpp
	g++ $^ -fopenmp -g --std=c++17 -o $@

bin/PageRank-OpenCL-d: src/Parser.cpp src/PageRank-OpenCL.cpp
	g++ $^ -lOpenCL -g --std=c++17 -o $@


# running
run_seq: seq
	srun -n1 --cpus-per-task=1 --reservation=fri bin/PageRank

run_omp: omp
	srun -n1 --cpus-per-task=$(N) --reservation=fri bin/PageRank-OpenMP

run_ocl: ocl
	srun -n1 -G1 --reservation=fri bin/PageRank-OpenCL

# clean
clean:
	rm bin/*