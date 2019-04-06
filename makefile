CC=gcc
CFLAGS= -Iinclude -Wall  -g -DDEBUG
LDFLAGS=-L./lib -Wl,-rpath=./lib -Wl,-rpath=/usr/local/lib

parser: parser.o stringify.o access.o demo.o randJsonGen.o
	$(CC) -o $@ $(LDFLAGS) $^ -ldl

clean:
	rm -rf *.o parser
