#include <stdlib.h>
// TODO
//#include "include/scanner.h"
#include "include/parser.h"
#include "include/recursive_descent.h" // prog()

/**
 * @brief Initialize parser structure
 */
parser_t *parser_init(void)
{
    parser_t *parser = calloc(1, sizeof(parser_t));
    if (parser == NULL) {
        // internal error?
        return NULL;
    }

    // TODO
    // parser->symtable = symtable_init();
    // if (parser->symtable == NULL) {
    //     free(parser); // internal error?
    //     return NULL; 
    // }
    parser->token = get_next_token();
    
    return parser;
}

/**
 * @brief Start parsing source file // TODO: handle src file
 */
int parser_parse(void)
{
    parser_t *parser = parser_init();
    parser->token = get_next_token();
    int result = parser->token->err_code;
    if (result == EXIT_OK) {
        // HANDLE FIRST NONTERMINAL AND START REC. DESCEND
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
    // TODO free symtable
    // TODO free token list if we have it
    free(parser);
}

