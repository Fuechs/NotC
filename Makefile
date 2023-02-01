cc = clang
flags = -std=c17 -g -O3

all: comp
	
comp:
	clang -o notc -std=c17 -g -O3 notc.c

run: comp
	./notc