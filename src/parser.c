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
parser_t *parser_init(FILE *src)
{
    parser_t *parser = calloc(1, sizeof(struct parser));
    if (parser == NULL) {
        // TODO: deallocate resources
        error_message("Parser", ERR_INTERNAL,  "Calloc failure."); 
        return NULL;
    }

    // Create local symtable
    if ((parser->local_symtable = symtable_init(CAPACITY)) == NULL) {
        free(parser);
        return NULL;
    }

    // Create global symtable
    if ((parser->global_symtable = symtable_init(CAPACITY)) == NULL) {
        free(parser);
        return NULL;
    }  
    
    parser->in_function = false;
    parser->declared_function = false;
    parser->src = src;
    parser->curr_item = NULL;
    
    return parser;
}

/**
 * @brief Start parsing source file 
 */
int parser_parse(FILE *src)
{
    if (src == NULL) { 
        error_message("FATAL", -69, "input source filed is NULL");
        return -69;
    }
    
    parser_t *parser = parser_init(src);
    if (parser == NULL) {
        error_message("FATAL", -69, "parser initialization failed");
        return -69;
    }
    if ((parser->token = get_next_token(parser->src)) == NULL) {
        error_message("Scanner", ERR_LEX, "token returned NULL");
        return ERR_LEX;
    }
    
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
    symtable_destroy(parser->local_symtable);
    symtable_destroy(parser->global_symtable);
    free(parser);
}

