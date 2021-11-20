#ifndef PARSER_H
#define PARSER_H

// TODO#include "symtable.h"
#include "scanner.h"

typedef struct parser {
    // symtable_t symtable;
    // list of tokens?
    // semantic analysis
    token_t token;
} parser_t;

/**
 * @brief Initialize parser structure
 * @param TODO
 */
parser_t *parser_init(void);

/**
 * @brief Start parsing source file // TODO: handle src file
 */
int parser_parse(void);

/**
 * @brief Deallocate parser structure
 * @param parser structure
 */
void parser_destroy(parser_t *parser);

#endif // PARSER_H
