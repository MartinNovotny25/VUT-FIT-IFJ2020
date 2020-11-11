CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

all: projekt

projekt: error.o parser.o scanner.o symtable.o
	$(CC) $(CFLAGS) -o projekt error.o parser.o scanner.o symtable.o

error.o: error.c
	$(CC) $(CFLAGS) -c error.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

scanner.o: scanner.c
	$(CC) $(CFLAGS) -c scanner.c

symtable.o: symtable.c
	$(CC) $(CFLAGS) -c symtable.c

clean:
	rm -f projekt *.o


