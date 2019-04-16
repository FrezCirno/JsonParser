CC=gcc
CFLAGS= -Iinclude -Wall  -g -DDEBUG
LDFLAGS=-L./lib -Wl,-rpath=./lib -Wl,-rpath=/usr/local/lib

parser: exstring.o randJsonGen.o parser.o stringify.o access.o utf-8.o demo.o  
	$(CC) -o $@ $(LDFLAGS) $^ -ldl

clean:
	rm -rf *.o parser
