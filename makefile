CC=gcc
CFLAGS=-std=c11 -Wall -pedantic
CFILES=lex_an.c main.c
OBJ=lex_an.o
  
compiler: $(CFILES)
	$(CC) $(CFLAGS) -o $@ $^

debug: $(CFILES)
	$(CC) $(CFLAGS) -o $@  -DDEBUG $^



lex_an.o: lex_an.c
	$(CC) $(CFLAGS) -fPIC -c $^ -o $@

clean:
	rm -f debug compiler output.txt *.o