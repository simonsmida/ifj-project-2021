#ifndef PARSER_H
#define PARSER_H

#include "symtable.h"
#include "scanner.h"

typedef struct parser {
    symtable_t *local_symtable;
    symtable_t *global_symtable;
    symtable_item_t *curr_item;
    // list of tokens?
    // semantic analysis
    token_t *token;
    FILE *src;
    bool inside_func_def;
    bool inside_func_dec;
} parser_t;


/**
 * @brief Initialize parser structure
 */
parser_t *parser_init(FILE *src);

/**
 * @brief Start parsing source file
 * @param src input source file to be parsed
 */
int parser_parse(FILE *src);

/**
 * @brief Deallocate parser structure
 * @param parser structure
 */
void parser_destroy(parser_t *parser);

#endif // PARSER_H
