#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/buffer.h"
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

    // Create global symtable
    if ((parser->global_symtable = symtable_init(CAPACITY)) == NULL) {
        free(parser);
        return NULL;
    }  
    
    DLL_Init(&(parser->list));

    parser->curr_block_id = 0; 
    parser->curr_block_depth = -1;
    parser->block_temp_id = 0;
    
    parser->curr_ret_val_count = 0;
    parser->curr_arg_count = 0;
    parser->inside_func_def = false;
    parser->inside_func_dec = false;
    parser->src = src;
    parser->is_in_return = false;

    parser->curr_func = NULL;
    parser->curr_item = NULL;
    parser->curr_rhs = NULL;

	for (int i = 0; i < ARRAY_DEPTH_NUM; i++){
		parser->array_depth[i] = 0;
	}
	parser->inside_while = false;
	parser->buffer = init_buffer();
    
    return parser;
}



/**
 * @brief Start parsing source file 
 */
int parser_parse(FILE *src)
{
    if (src == NULL) { 
        error_message("FATAL", ERR_INTERNAL, "input source file is NULL");
        return ERR_INTERNAL;
    }
    
    parser_t *parser = parser_init(src);
    if (parser == NULL) {
        error_message("FATAL", ERR_INTERNAL, "parser initialization failed");
        return ERR_INTERNAL;
    }
    if ((parser->token = get_next_token(parser->src)) == NULL) {
        error_message("Scanner", ERR_LEX, "token returned NULL");
        return ERR_INTERNAL;
    }
    
    int result = parser->token->type;

    if (result != TOKEN_ERROR) {
        // Starting recursive descent
        result = prog(parser); 
    }
	else{
		parser_destroy(parser);
		error_message("Scanner", ERR_LEX, "token returned TOKEN_ERROR");
		return ERR_LEX;
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
    DLL_Dispose(&(parser->list));
    destroy_token(parser->token);
    parser->curr_func = NULL;
    parser->curr_item = NULL;
    parser->curr_rhs = NULL;
    // Destroying global symtab handles destruction of locals
    symtable_destroy(SYMTAB_G);
	if (parser->buffer != NULL){
		destroy_buffer(parser->buffer);
		parser->buffer = NULL;
	}
    free(parser);
}


/**
 * @brief Check if given function id is built in function
 */
bool is_built_in(char *func_id)
{
    if (!strcmp(func_id, "reads")     ||
        !strcmp(func_id, "readi")     ||
        !strcmp(func_id, "readn")     ||
        !strcmp(func_id, "write")     ||
        !strcmp(func_id, "tointeger") ||
        !strcmp(func_id, "substr")    ||
        !strcmp(func_id, "ord")       ||
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
    // Insert new function id into the global symtable
    parser->curr_item = symtable_create_and_insert_function(SYMTAB_G, func_id);
    if (parser->curr_item == NULL) {
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
    if ((define_ord(      parser, "ord"      ) == EXIT_OK) &&
        (define_chr(      parser, "chr"      ) == EXIT_OK) &&
        (define_reads(    parser, "reads"    ) == EXIT_OK) &&
        (define_readi(    parser, "readi"    ) == EXIT_OK) &&
        (define_readn(    parser, "readn"    ) == EXIT_OK) &&
        (define_write(    parser, "write"    ) == EXIT_OK) &&
        (define_substr(   parser, "substr"   ) == EXIT_OK) &&
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
        // Insert return type - updates num_ret_types
        symtable_insert_new_function_ret_type(SYMTAB_G, DTYPE_STRING,  parser->curr_item->key);
        
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}


int define_readi(parser_t *parser, char *func_id)
{
    // function readi() : integer

    if (create_builtin_function(parser, func_id) == EXIT_OK) {
        FUNC_ITEM->num_params = 0;
        // Insert return type - updates num_ret_types
        symtable_insert_new_function_ret_type(SYMTAB_G, DTYPE_INT,  parser->curr_item->key);
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_readn(parser_t *parser, char *func_id)
{
    // function readn() : number

    if (create_builtin_function(parser, func_id) == EXIT_OK) {
        FUNC_ITEM->num_params = 0;
        // Insert return type - updates num_ret_types
        symtable_insert_new_function_ret_type(SYMTAB_G, DTYPE_NUMBER, parser->curr_item->key);
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_write(parser_t *parser, char *func_id)
{
    // function write(term1, term2, ..., term_n)

    if (create_builtin_function(parser, func_id) == EXIT_OK) {
        // Variadic function + no return values
        FUNC_ITEM->num_ret_types = 0;
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_tointeger(parser_t *parser, char *func_id)
{
    // function tointeger(f : number) : integer

    if (create_builtin_function(parser, func_id) == EXIT_OK) {
        // Insert parameters - updates num_params
        symtable_insert_new_function_param(SYMTAB_G, DTYPE_NUMBER, parser->curr_item->key);
        // Insert return type - updates num_ret_types
        symtable_insert_new_function_ret_type(SYMTAB_G, DTYPE_INT, parser->curr_item->key);
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_substr(parser_t *parser, char *func_id)
{
    // function substr(s : string, i : number, j : number) : string 

    if (create_builtin_function(parser, func_id) == EXIT_OK) {
        // Insert parameters - updates num_params
        symtable_insert_new_function_param(SYMTAB_G, DTYPE_STRING, parser->curr_item->key);
        symtable_insert_new_function_param(SYMTAB_G, DTYPE_NUMBER, parser->curr_item->key);
        symtable_insert_new_function_param(SYMTAB_G, DTYPE_NUMBER, parser->curr_item->key);
        // Insert return type - updates num_ret_types
        symtable_insert_new_function_ret_type(SYMTAB_G, DTYPE_STRING, parser->curr_item->key);
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_ord(parser_t *parser, char *func_id)
{
    // function ord(s : string, i : integer) : integer

    if (create_builtin_function(parser, func_id) == EXIT_OK) {     
        // Insert parameters - updates num_params
        symtable_insert_new_function_param(SYMTAB_G, DTYPE_STRING, parser->curr_item->key);
        symtable_insert_new_function_param(SYMTAB_G, DTYPE_INT, parser->curr_item->key);    
        // Insert return type - updates num_ret_types
        symtable_insert_new_function_ret_type(SYMTAB_G, DTYPE_STRING, parser->curr_item->key);
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}

int define_chr(parser_t *parser, char *func_id)
{
    // function chr(i : integer) : string

    if (create_builtin_function(parser, func_id) == EXIT_OK) {
        // Insert parameters - updates num_params
        symtable_insert_new_function_param(SYMTAB_G, DTYPE_INT, parser->curr_item->key);     
        // Insert return type - updates num_ret_types
        symtable_insert_new_function_ret_type(SYMTAB_G, DTYPE_STRING, parser->curr_item->key);
        return EXIT_OK;
    }
    return ERR_INTERNAL;
}
