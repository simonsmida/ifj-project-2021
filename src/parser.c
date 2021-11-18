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
    // parser->token = get_next_token(); ?
    
    return parser;
}

int parser_parse(void)
{
    parser_t *parser = parser_init();
    int result;
    if ((result = get_next_token()) == EXIT_OK) {
        // HANDLE FIRST NONTERMINAL AND START REC. DESCENT
        result = prog(); 
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

