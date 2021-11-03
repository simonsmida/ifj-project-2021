OUT=main
CC=gcc
CFLAGS=-std=c11 -Wall -pedantic -g -lm

SRC_FILES = $(wildcard src/*.c)
OBJ_FILES = $(SRC_FILES:.c=.o)

# example usage: ./main examples/fact_iter.ifj21

$(OUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c src/include/%.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o src/*.o main
