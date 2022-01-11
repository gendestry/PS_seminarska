.phony: clean

all: test

test: test.cpp
	g++ test.cpp --std=c++17 -o test

rtest: test
	./test

clean:
	rm test