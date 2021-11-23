OUT=main
CC=gcc
CFLAGS=-std=c11 -Wall -pedantic -g -lm

#SRC_FILES = $(wildcard PA*.c )
SRC_FILES = $(filter-out main.c symtable.c, $(wildcard *.c))
OBJ_FILES = $(SRC_FILES:.c=.o)

#example usage: ./main examples/fact_recur.ifj21
#if you get bored, type 'q', to quit scanning process

$(OUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c include/%.h include/symtable.h include/scanner.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o main
