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
    //printf("is: %d, expected: %d\n", term_type, expected_type);
    if (expected_type == DTYPE_NUMBER) {
        //printf("cool\n");
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
        error_message("Parser", ERR_SEMANTIC_PROG, "invalid argument type");
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
