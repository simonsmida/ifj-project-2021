#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/scanner.h"
#include "include/parser.h"
#include "include/recursive_descent.h" // prog()
#include "include/error.h"

/**
 * @brief Initialize parser structure
 */
parser_t *parser_init(void)
{
    parser_t *parser = calloc(1, sizeof(parser_t));
    if (parser == NULL) {
        // TODO: deallocate resources
        error_message("Parser", ERR_INTERNAL,  "Calloc failure.\n"); 
        return NULL;
    }
    
    // Create local symtable
    if ((parser->local_symtable = symtable_init(CAPACITY)) != NULL) {
        free(parser);
        return NULL;
    }

    // Create global symtable
    if ((parser->global_symtable = symtable_init(CAPACITY)) != NULL) {
        free(parser);
        return NULL;
    }  
    
    parser->in_function = false;
    parser->declared_function = false;
    
    return parser;
}

/**
 * @brief Start parsing source file 
 */
int parser_parse(FILE *src)
{
    if (src == NULL) { /* File error */ }

    parser_t *parser = parser_init();
    parser->token = get_next_token(src);

    int result = parser->token->type;
    if (result != TOKEN_ERROR) {
        // Starting recursive descent
        result = prog(parser); 
    }

    // Parsing finished
    parser_destroy(parser);
    return result;
}

/**
 * @brief Deallocate parser structure
 */
void parser_destroy(parser_t *parser)
{
    symtable_destroy(&parser->local_symtable);
    symtable_destroy(&parser->global_symtable);
    free(parser);
}

