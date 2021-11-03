CC=gcc
CFLAGS=-std=c11 -Wall -pedantic
CFILES=scanner.c main.c
HEADERS = buffer.h scanner.h
OBJ=scanner.o buffer.o
  
compiler: main.c $(OBJ)
	$(CC) $(CFLAGS) -g -o $@ $^

debug: $(CFILES)
	$(CC) $(CFLAGS) -o $@  -DDEBUG $^

lex_an.o: scanner.c
	$(CC) $(CFLAGS) -fPIC -c $^ -o $@

buffer.o: buffer.c
	$(CC) $(CFLAGS) -fPIC -c $^ -o $@

clean:
	rm -f debug compiler output.txt *.o
