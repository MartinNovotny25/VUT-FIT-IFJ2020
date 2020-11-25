CC=gcc
CFLAGS=-Wall -std=c99 -pedantic

all: projekt

projekt: tokenList.o parser.o scanner.o error.o symtable.o semantics.o precanalysis_stack.o precanalysis.o
	$(CC) $(CFLAGS) -o projekt tokenList.o parser.o scanner.o error.o symtable.o semantics.o precanalysis_stack.o precanalysis.o

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

semantics.o: semantics.c
	$(CC) $(CFLAGS) -c semantics.c

precanalysis_stack.o: precanalysis_stack.c
	$(CC) $(CFLAGS) -c precanalysis_stack.c

precanalysis.o: precanalysis.c
	$(CC) $(CFLAGS) -c precanalysis.c


clean:
	rm -f projekt *.o


