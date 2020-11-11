CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm

projekt: error.c parser.c scanner.c symtable.c
     $(CC) $(CFLAGS) -o projekt error.c parser.c scanner.c symtable.c
