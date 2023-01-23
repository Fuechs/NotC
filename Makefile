cc = clang++
flags = -std=c++20 -g -O3 -std=c++20
src = $(wildcard src/*.cpp)

all: comp
	

comp:
	$(cc) $(flags) $(src)

run: comp
	./a.out