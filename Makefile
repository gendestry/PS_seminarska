.phony: clean

all: test

test: test.cpp Parser.cpp Matrix.cpp
	g++ Matrix.cpp Parser.cpp test.cpp -g --std=c++17 -o test

rtest: test
	./test

clean:
	rm test