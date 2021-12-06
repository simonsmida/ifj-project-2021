#include <stdio.h>
#include "include/semantics.h"


int dtype_keyword(int keyword_type)
{
    switch (keyword_type) 
    {
        case KEYWORD_STRING:
            return DTYPE_STRING;
        case KEYWORD_INTEGER:
            return DTYPE_INT;
        case KEYWORD_NUMBER:
            return DTYPE_NUMBER;
        case KEYWORD_NIL:
            return DTYPE_NIL;
        default:
            break;
    }
    return DTYPE_UNKNOWN;
}

int dtype_token(parser_t *parser)
{
    switch (TOKEN_T) 
    {
        case TOKEN_STR_LIT:
            return DTYPE_STRING;
        case TOKEN_INT_LIT:
            return DTYPE_INT;
        case TOKEN_NUM_LIT:
            return DTYPE_NUMBER;
        default:
            break;
    } // switch()

    if ((TOKEN_T == TOKEN_KEYWORD) && (TOKEN_KW_T == KEYWORD_NIL)) {
        return DTYPE_NIL;
    }
    return DTYPE_UNKNOWN;
}

bool is_term_type_valid(int term_type, int expected_type)
{
    if (expected_type == DTYPE_NUMBER) {
        return (term_type == DTYPE_INT || term_type == DTYPE_NUMBER);
    } 
    return (term_type == expected_type);
}


// STARTING SEMANTIC ACTIONS
int check_arg_count(parser_t *parser)
{
    // Check variable count
    if (parser->curr_rhs == NULL || parser->curr_rhs->function == NULL) {
        return ERR_INTERNAL;
    }
    int expected_arg_count = parser->curr_rhs->function->num_params;
    if (parser->curr_arg_count+1 > expected_arg_count) {
        if (strcmp(parser->curr_rhs->key, "write") != 0) { // write function is variadic
            error_message("Parser", ERR_SEMANTIC_PROG, "invalid argument count");
            return ERR_SEMANTIC_PROG;
        }
    }
    return EXIT_OK;
}

int check_arg_type_literal(parser_t *parser)
{
    if (parser->curr_rhs == NULL || parser->curr_rhs->function == NULL) {
        return ERR_INTERNAL;
    }
    
    if (!strcmp(parser->curr_rhs->key, "write")) { // write function is variadic
        return EXIT_OK;
    }
   
    // Check variable type
    int expected_type = parser->curr_rhs->function->type_params[parser->curr_arg_count];
    if (!is_term_type_valid(dtype_token(parser), expected_type)) {
        error_message("Parser", ERR_SEMANTIC_PROG, "invalid argument type");
        return ERR_SEMANTIC_PROG;
    }
    return EXIT_OK;
}

int check_undefined_arg(parser_t *parser)
{
    if ((parser->curr_item = symtable_search(SYMTAB_L, TOKEN_REPR)) == NULL) {
        error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' not found in local symtab", TOKEN_REPR);
        return ERR_SEMANTIC_DEF; 
    } else if (parser->curr_item->const_var != NULL) {
        // Variable with same id found in this function - check if previously defined
        if (!parser->curr_item->const_var->defined) {
            error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' undefined", TOKEN_REPR);
            return ERR_SEMANTIC_DEF;
        }
    }
    return EXIT_OK;
}

int check_arg_type_id(parser_t *parser)
{
    if (parser->curr_item == NULL || parser->curr_item->const_var == NULL) {
        return ERR_INTERNAL;
    }

    if (!strcmp(parser->curr_rhs->key, "write")) { // write function is variadic
        return EXIT_OK;
    }


    // Check variable type
    int term_type = parser->curr_item->const_var->type;
    int expected_type = parser->curr_rhs->function->type_params[parser->curr_arg_count];
    if (!is_term_type_valid(term_type, expected_type)) {
        fprintf(stderr, "[%s(%s)] ->Expected: %d, is: %d\n\n", parser->curr_rhs->key, parser->curr_item->key,expected_type, dtype_token(parser));
        error_message("Pariiser", ERR_SEMANTIC_PROG, "invalid argument type");
        return ERR_SEMANTIC_PROG;
    }
    return EXIT_OK;
}

int check_arg_previous_def(parser_t *parser)
{
    // SEMANTIC ACTION - check if actual parameter had been defined in current block
    if (parser->curr_item == NULL) {
        return ERR_INTERNAL;
    }
    if ((parser->curr_item = symtable_search(SYMTAB_L, TOKEN_REPR)) == NULL) {
        error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' not found in local symtab", TOKEN_REPR);
        return ERR_SEMANTIC_DEF; 
    } else if (parser->curr_item->const_var != NULL) {
        // Variable with same id found in this function - check if previously defined
        if (!parser->curr_item->const_var->defined) {
            error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' undefined", TOKEN_REPR);
            return ERR_SEMANTIC_DEF;
        }
    }
    return EXIT_OK;
}

int check_function_redeclaration(parser_t *parser)
{
    // Considef FUNC_ITEM ~ parser->curr_item->function
    if (parser->curr_item == NULL || FUNC_ITEM == NULL) {
        return ERR_INTERNAL;
    }

    /** SEMANTIC ACTION - function redeclaration **/
    if (FUNC_ITEM->declared) { 
        /* Function redeclaration */
        error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of function '%s'", TOKEN_REPR);
        return ERR_SEMANTIC_DEF;
    }
    return EXIT_OK;
}

int check_function_call(parser_t *parser)
{ 
    // Consider SYMTAB_G ~ parser->global_symtable
    if (SYMTAB_G == NULL) {
        return ERR_INTERNAL;
    }

    if ((parser->curr_rhs = symtable_search(SYMTAB_G, TOKEN_REPR)) != NULL) {
        // Check previous declaration and definition
        if ((parser->curr_rhs->function != NULL) && !(parser->curr_rhs->function->declared)) {
            error_message("Parser", ERR_SEMANTIC_DEF, "called function '%s' "
                                                      "not previously declared", TOKEN_REPR);
            return ERR_SEMANTIC_DEF;
        }
    } else { // Function ID not found - func call invalid
        error_message("Parser", ERR_SEMANTIC_DEF, "called function '%s' "
                                          "not previously declared nor defined", TOKEN_REPR);
        return ERR_SEMANTIC_DEF;
    }
    return EXIT_OK;
}

int check_function_call_arg_count(parser_t *parser)
{
    if (parser->curr_rhs == NULL || parser->curr_rhs->function == NULL) {
        return ERR_INTERNAL;
    }
    if (strcmp(parser->curr_rhs->key, "write") == 0) { // write is variadic
        return EXIT_OK;
    }

    int num_params = parser->curr_rhs->function->num_params;
    if (num_params != parser->curr_arg_count) {
        error_message("Parser", ERR_SEMANTIC_PROG, "invalid number of arguments "
                      "in function '%s'", parser->curr_rhs->key);
        fprintf(stderr, "expected: %d, has: %d\n", parser->curr_arg_count, num_params);
        return ERR_SEMANTIC_PROG;
    }
    return EXIT_OK;
}

int check_arg_def(parser_t *parser)
{
    int b_id = parser->curr_block_id;
    int b_depth = parser->curr_block_depth;
    if ((parser->curr_item = most_recent_var(SYMTAB_L, TOKEN_REPR, b_id, b_depth, true)) == NULL) {
        error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' is not defined here", TOKEN_REPR);
        return ERR_SEMANTIC_DEF;    
    }
    return EXIT_OK;
}

int check_function_redefinition(parser_t *parser)
{
    if (parser->curr_func == NULL) {
        return ERR_INTERNAL;
    }
    /** SEMANTIC ACTION - function redefinition **/
    if (CURR_FUNC != NULL && CURR_FUNC->defined) { 
        /* Function redefinition */
        error_message("Parser", ERR_SEMANTIC_DEF, "redefinition of function '%s'", TOKEN_REPR);
        return ERR_SEMANTIC_DEF;
    }
    return EXIT_OK;
}

int check_param_redeclaration(parser_t *parser)
{
    if ((parser->curr_item = symtable_search(SYMTAB_L, TOKEN_REPR)) != NULL) { 
        // item already exists
        error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of variable '%s'", TOKEN_REPR);
        return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
    }
    return EXIT_OK;
}

int check_param_mismatch(parser_t *parser, int param_index)
{
    /** SEMANTIC ACTION - check param type mismatch **/
    if ((parser->curr_func != NULL) && (CURR_FUNC->declared && !CURR_FUNC->defined)) {
        
        if (param_index+1 > CURR_FUNC->num_params) {
            error_message("Parser", ERR_SEMANTIC_PROG, "param count mismatch");
            fprintf(stderr, "Expected: %d, is: %d", CURR_FUNC->num_params, param_index+1);
            return ERR_SEMANTIC_PROG;                
        }
        
        if (CURR_FUNC->type_params[param_index] != dtype_keyword(TOKEN_KW_T)) {
            error_message("Parser", ERR_SEMANTIC_PROG, "function param type mismatch");
            return ERR_SEMANTIC_PROG;
        }
    }
    return EXIT_OK;
}

int check_param_count_mismatch(parser_t *parser)
{
    /** SEMANTIC ACTION **/ 
    if ((parser->curr_func != NULL) && (CURR_FUNC->num_params != 0)) {
        error_message("Parser", ERR_SEMANTIC_PROG, "function param count mismatch");
        return ERR_SEMANTIC_PROG;
    }
    return EXIT_OK;
}

int check_no_return_values(parser_t *parser)
{
    if (parser->curr_func == NULL) {
        return ERR_INTERNAL;
    }

    if (CURR_FUNC->num_ret_types != 0) {
        error_message("Parser", ERR_SEMANTIC_PROG, "expected 0 return values");
        return ERR_SEMANTIC_PROG;
    }
    return EXIT_OK;
}

int check_return_value_count(parser_t *parser, int ret_type_index)
{
    /** SEMANTIC ACTION - check if function declaration has more params **/
    if (parser->curr_func != NULL && CURR_FUNC->num_ret_types > ret_type_index) {
        error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
        return ERR_SEMANTIC_PROG; 
    }
    return EXIT_OK;
}

int check_return_values(parser_t *parser, int ret_type_index)
{
    if ((parser->curr_func != NULL) && (CURR_FUNC->declared && !(CURR_FUNC->defined))) {
        
        // Check return value count
        if (ret_type_index+1 > CURR_FUNC->num_ret_types) {
            error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
            return ERR_SEMANTIC_PROG;                
        }
        
        // Check current return value type
        if (CURR_FUNC->ret_types[ret_type_index] != dtype_keyword(TOKEN_KW_T)) {
            error_message("Parser", ERR_SEMANTIC_PROG, "function return type mismatch");
            return ERR_SEMANTIC_PROG;
        }
    }
    return EXIT_OK;
}

int check_variable_redeclaration(parser_t *parser)
{
    // Create new item in local symtable - check semantics (redeclaration)
    if (would_be_var_redeclared(SYMTAB_L, TOKEN_REPR, parser->curr_block_id)) {
        error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of variable '%s'", TOKEN_REPR);
        return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
    }
    return EXIT_OK;
}

int check_undeclared_var_or_func(parser_t *parser, symtable_item_t *item_dec)
{
    int b_id = parser->curr_block_id;
    int b_depth = parser->curr_block_depth;
    if (!(item_dec = most_recent_var(SYMTAB_L, TOKEN_REPR, b_id, b_depth, false))) {
        error_message("Parser", ERR_SEMANTIC_DEF, "undeclared variable/function '%s'", TOKEN_REPR);
        return ERR_SEMANTIC_DEF;
    } 
    return EXIT_OK;
}

// TODO: solve error output


