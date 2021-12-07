OUT = main
CC = gcc
CFLAGS = -std=c11 -Wall -pedantic -g -lm
TEST_FLAGS = -Wall -g -ggdb3 -W -std=c99 -I tests/unity/src tests/unity/src/unity.c

# Path to source files
PATHS = src/
# Path to tests
PATHT = tests/
# Path to test binaries
PATHB = tests/bin/

TEST_SRC  = $(wildcard $(PATHT)*.c)
SRC_FILES = $(wildcard $(PATHS)*.c)
OBJ_FILES = $(SRC_FILES:.c=.o)
BIN_TESTS = $(addprefix $(PATHB), $(basename $(notdir $(TEST_SRC))))

#example usage: ./main examples/fact_recur.ifj21
#if you get bored, type 'q', to quit scanning process

$(OUT): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c $(PATHS)include/%.h
	$(CC) -c $(CFLAGS) $< -o $@

# ------------------------------------ TESTS ---------------------------------------
$(PATHB):
	mkdir $(PATHB)

test: $(PATHB) $(BIN_TESTS)
	$(PATHB)scanner_test
	$(PATHB)symtable_test
	$(PATHB)generator_test
	$(PATHB)parser_test

$(PATHB)scanner_test: $(PATHT)scanner_test.c $(PATHS)scanner.c $(PATHS)buffer.c
	$(CC) $(TEST_FLAGS) $^ -o $@

$(PATHB)symtable_test: $(PATHT)symtable_test.c $(PATHS)symtable.c $(PATHS)buffer.c
	$(CC) $(TEST_FLAGS) $^ -o $@

$(PATHB)generator_test: $(PATHT)generator_test.c $(PATHS)code_generator.c  $(PATHS)scanner.c  $(PATHS)buffer.c
	$(CC) $(TEST_FLAGS) $^ -o $@

$(PATHB)parser_test: $(PATHT)parser_test.c $(PATHS)bottom_up_sa.c $(PATHS)semantics.c $(PATHS)scanner.c  $(PATHS)buffer.c $(PATHS)PA_STACK.c $(PATHS)code_generator.c $(PATHS)symtable.c $(PATHS)parser.c $(PATHS)error.c $(PATHS)recursive_descent.c
	$(CC) $(TEST_FLAGS) $^ -o $@
# ----------------------------------------------------------------------------------

.PHONY: main

clean:
	rm -f *.o src/*.o main  
	rm -r -f tests/bin/
