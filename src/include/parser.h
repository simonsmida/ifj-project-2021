#ifndef PARSER_H
#define PARSER_H

#include "symtable.h"
#include "scanner.h"

typedef struct parser {
    symtable_t *local_symtable;
    symtable_t *global_symtable;

    // list of tokens?
    // semantic analysis
    token_t *token;

    bool in_function;
    bool declared_function;
} parser_t;

/**
 * @brief Initialize parser structure
 */
parser_t *parser_init(void);

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
