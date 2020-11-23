CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

all: projekt

projekt: tokenList.o parser.o scanner.o error.o symtable.o
	$(CC) $(CFLAGS) -o projekt tokenList.o parser.o scanner.o error.o symtable.o

tokenList.o: tokenList.c
	$(CC) $(CFLAGS) -c tokenList.c

parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

scanner.o: scanner.c
	$(CC) $(CFLAGS) -c scanner.c

error.o: error.c
	$(CC) $(CFLAGS) -c error.c

symtable.o: symtable.c
	$(CC) $(CFLAGS) -c symtable.c

clean:
	rm -f projekt *.o


