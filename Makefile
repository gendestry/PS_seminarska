.phony: clean

all: test

test: Parser.cpp test.cpp
	g++ Parser.cpp test.cpp -g --std=c++17 -o test

run: test
	srun --reservation=fri ./test

omp: Parser.cpp PageRank_omp.cpp
	g++ Parser.cpp PageRank_omp.cpp -fopenmp -g --std=c++17 -o omp

run_omp: omp
	srun -n1 --cpus-per-task=32 --reservation=fri ./omp

clean:
	rm test
