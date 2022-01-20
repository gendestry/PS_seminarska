.phony: clean

all: test

test: Parser.cpp test.cpp
	g++ Parser.cpp test.cpp -g --std=c++17 -o test

run: test
	# srun -n1 --reservation=fri test
	./test

clean:
	rm test