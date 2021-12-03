#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/symtable.h"
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

    // Create local symtable TODO
    if ((parser->local_symtable = symtable_init(CAPACITY)) == NULL) {
        free(parser);
        return NULL;
    }

    // Create global symtable
    if ((parser->global_symtable = symtable_init(CAPACITY)) == NULL) {
        free(parser);
        return NULL;
    }  
    
    parser->inside_func_def = false;
    parser->inside_func_dec = false;
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
    if (src != stdin) {
        fclose(src); 
    }
    return result;
}

/**
 * @brief Deallocate parser structure
 */
void parser_destroy(parser_t *parser)
{
    destroy_token(parser->token);
    symtable_destroy(parser->local_symtable);
    symtable_destroy(parser->global_symtable);
    free(parser);
}

/**
 * @brief Check if given function id is built in function
 */
bool is_built_in(char *func_id)
{
    if (!strcmp(func_id, "reads") ||
        !strcmp(func_id, "readi") ||
        !strcmp(func_id, "readn") ||
        !strcmp(func_id, "write") ||
        !strcmp(func_id, "tointeger") ||
        !strcmp(func_id, "substr") ||
        !strcmp(func_id, "ord") ||
        !strcmp(func_id, "chr")) {
        return true;
    }
    return false;
}

/**
 * @brief Create an item in the global symtable and insert corresponding function
 */
int create_builtin_function(parser_t *parser, char *func_id)
{
    /* Consider FUNC_ITEM <=> parser->curr_item->function */

    // Create new item in the global symtable
    if (!(parser->curr_item = symtable_insert(parser->global_symtable, func_id))) {
        return ERR_INTERNAL;
    }     
    
    // Insert new function id into the global symtable
    FUNC_ITEM = symtable_create_and_insert_function(parser->global_symtable, func_id);
    if (FUNC_ITEM == NULL) {
        return ERR_INTERNAL;
    } 
    
    // Every built in function is considered to be declared and defined 
    FUNC_ITEM->declared = true;
    FUNC_ITEM->defined = true;

    return EXIT_OK;
}

/**
 * @brief Define every specified built in function of ifj21
 */
int define_every_builtin_function(parser_t *parser)
{
    if ((define_ord(parser, "ord")             == EXIT_OK) &&
        (define_chr(parser, "chr")             == EXIT_OK) &&
        (define_reads(parser, "reads")         == EXIT_OK) &&
        (define_readi(parser, "readi")         == EXIT_OK) &&
        (define_readn(parser, "readn")         == EXIT_OK) &&
        (define_write(parser, "write")         == EXIT_OK) &&
        (define_substr(parser, "substr")       == EXIT_OK) &&
        (define_tointeger(parser, "tointeger") == EXIT_OK)) {
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_reads(parser_t *parser, char *func_id)
{
    // function reads() : string

    if (create_builtin_function(parser, func_id) == EXIT_OK) {
        
        FUNC_ITEM->num_params = 0;
        FUNC_ITEM->num_ret_types = 1;
        FUNC_ITEM->type_params = NULL; // No params
        FUNC_ITEM->ret_types[0] = DTYPE_STRING;
        
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}


int define_readi(parser_t *parser, char *func_id)
{
    // function readi() : integer

    if (create_builtin_function(parser, func_id) == EXIT_OK) {

        FUNC_ITEM->num_params = 0;
        FUNC_ITEM->num_ret_types = 1;
        FUNC_ITEM->type_params = NULL; // No params
        FUNC_ITEM->ret_types[0] = DTYPE_INT;

        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_readn(parser_t *parser, char *func_id)
{
    // function readn() : number

    if (create_builtin_function(parser, func_id) == EXIT_OK) {

        FUNC_ITEM->num_params = 0;
        FUNC_ITEM->num_ret_types = 1;
        FUNC_ITEM->type_params = NULL; // No params
        FUNC_ITEM->ret_types[0] = DTYPE_NUMBER;

        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_write(parser_t *parser, char *func_id)
{
    // function write(term1, term2, ..., term_n)

    if (create_builtin_function(parser, func_id) == EXIT_OK) {

        // FUNC_ITEM->num_params = infinity; - variadic function
        // FUNC_ITEM->type_params = NULL; // - variadic function
        FUNC_ITEM->num_ret_types = 0;
        FUNC_ITEM->ret_types = NULL; // No return value

        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_tointeger(parser_t *parser, char *func_id)
{
    // function tointeger(f : number) : integer

    if (create_builtin_function(parser, func_id) == EXIT_OK) {

        FUNC_ITEM->num_params = 1;
        FUNC_ITEM->num_ret_types = 1;
        FUNC_ITEM->type_params[0] = DTYPE_NUMBER;
        FUNC_ITEM->ret_types[0] = DTYPE_INT;

        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_substr(parser_t *parser, char *func_id)
{
    // function substr(s : string, i : number, j : number) : string 

    if (create_builtin_function(parser, func_id) == EXIT_OK) {

        FUNC_ITEM->num_params = 3;
        FUNC_ITEM->num_ret_types = 1;
        FUNC_ITEM->type_params[0] = DTYPE_STRING;
        FUNC_ITEM->type_params[1] = DTYPE_NUMBER;
        FUNC_ITEM->type_params[2] = DTYPE_NUMBER;
        FUNC_ITEM->ret_types[0] = DTYPE_STRING;

        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_ord(parser_t *parser, char *func_id)
{
    // function ord(s : string, i : integer) : integer

    if (create_builtin_function(parser, func_id) == EXIT_OK) {

        FUNC_ITEM->num_params = 2;
        FUNC_ITEM->num_ret_types = 1;
        FUNC_ITEM->type_params[0] = DTYPE_STRING;
        FUNC_ITEM->type_params[1] = DTYPE_INT;
        FUNC_ITEM->ret_types[0] = DTYPE_INT;

        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_chr(parser_t *parser, char *func_id)
{
    // function ord(s : string, i : integer) : integer

    if (create_builtin_function(parser, func_id) == EXIT_OK) {

        FUNC_ITEM->num_params = 1;
        FUNC_ITEM->num_ret_types = 1;
        FUNC_ITEM->type_params[0] = DTYPE_INT;
        FUNC_ITEM->ret_types[0] = DTYPE_STRING;

        return EXIT_OK;
    }
    return ERR_INTERNAL;
}
