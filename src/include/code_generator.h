#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "include/scanner.h"
#include "include/symtable.h"

void generate_built_in_write(bool is_global, token_t *token);

void generate_head();

void generate_built_in_reads();

void generate_built_in_readi();

void generate_built_in_readn();

void generate_built_in_functions();

void generate_end();

void generate_built_in_substr();

void generate_built_in_ord();

void generate_built_in_chr();