.phony: clean

all: test

test: test.cpp Parser.cpp
	g++ Parser.cpp test.cpp --std=c++17 -o test

rtest: test
	./test

clean:
	rm test