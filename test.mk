# IFJ2021

PATHS = src/
PATHT = tests/
PATHB = bin/

CC := gcc
CFLAGS = -Wall -g -ggdb3 -W -std=c99 -I unity/src unity/src/unity.c

SRC = $(wildcard $(PATHT)*.c)
BIN = $(addprefix $(PATHB), $(basename $(notdir $(SRC))))

test : $(PATHB) $(BIN)
	./$(PATHB)scanner_test
	#./$(PATHB)symtable_test

$(PATHB) :
	mkdir $(PATHB)

$(PATHB)scanner_test : $(PATHT)scanner_test.c $(PATHS)scanner.c $(PATHS)buffer.c
	$(CC) $(CFLAGS) $^ -o $@

$(PATHB)symtable_test : $(PATHT)symtable_test.c $(PATHS)symtable.c $(PATHS)buffer.c
	$(CC) $(CFLAGS) $^ -o $@

clean :
	rm -r $(PATHB)
