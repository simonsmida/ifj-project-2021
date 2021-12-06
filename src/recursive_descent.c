#include <stdbool.h>
#include <string.h>

#include "include/recursive_descent.h"
#include "include/bottom_up_sa.h"
#include "include/error.h"
#include "include/scanner.h"
#include "include/parser.h"
#include "include/code_generator.h"


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

/**
 * @brief Starting nonterminal <prog>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int prog(parser_t *parser)
{
    int result;

    // RULE 1: <prog> → <prolog> <seq> 'EOF'
    
    // <prolog>
    result = prolog(parser); 
    CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

    // <seq>
    result = seq(parser);
    CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

    // Check End Of File
    CHECK_TOKEN_TYPE(TOKEN_EOF);

    // TODO: generate instruction for program end
    
    return EXIT_OK; 
}


/**
 * @brief Nonterminal <prolog>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int prolog(parser_t *parser)
{
    int result;

    switch (TOKEN_T) 
    {
        case TOKEN_KEYWORD:
            
            // Rule 2: <prolog> → 'require' 'ifj21'

            // Expected keyword is 'require' 
            CHECK_KEYWORD(KEYWORD_REQUIRE);
            
            PARSER_EAT(); /* expected 'ifj21' */
            CHECK_TOKEN_TYPE(TOKEN_STR_LIT);

            // Check if scanner correctly set attribute's string value
            if (parser->token->attribute->string == NULL) {
                return ERR_LEX; // TODO: check error code
            }
            
            // Check if next token is precisely "ifj21"
            if (strcmp(TOKEN_REPR, "ifj21")) {
                error_message("Parser", ERR_SYNTAX, "expected: \"%s\", is: \"%s\"", "ifj21", TOKEN_REPR);
                return ERR_SYNTAX;
            }
	        generate_head();
            
            
            // Define built-in functions
            if ((result = define_every_builtin_function(parser)) != EXIT_OK) {
                error_message("FATAL", ERR_INTERNAL, "failed to load built-in functions");
                return result;
            }
	        generate_built_in_functions();

            PARSER_EAT();
            return EXIT_OK;

        default: break;

    } // switch()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <seq>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int seq(parser_t *parser) 
{
    int result;

    switch (TOKEN_T) 
    {
        case TOKEN_KEYWORD: 

            if (TOKEN_KW_T == KEYWORD_GLOBAL) {

                // RULE 3: <seq> → <func_dec> <seq>

                // <func_dec>
                result = func_dec(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                
                // <seq> calls itself
                return seq(parser); 
            
            } else if (TOKEN_KW_T == KEYWORD_FUNCTION) {
                
                // RULE 4: <seq> → <func_def> <seq>
                
                // <func_def>
                result = func_def(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                
                // <seq> calls itself
                PARSER_EAT();
                return seq(parser); 
            }
            break; // Must end up in error

        case TOKEN_ID: // RULE 5: <seq> → <func_call> <seq>
            
            // Nondeterminism not present - if not func_call, its syntax error
            result = func_call(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

            // <seq> calls itself
            PARSER_EAT();
            return seq(parser);

        case TOKEN_EOF:
            
            // RULE 6: <func_dec> → ε
            
            return EXIT_OK;

        default: break; 
    } // switch()
    
    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <func_dec>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int func_dec(parser_t *parser)
{
    int result;

    switch (TOKEN_T) 
    {
        case TOKEN_KEYWORD: // 'global' or 'function'

            if (TOKEN_KW_T == KEYWORD_GLOBAL) {

                // RULE 7: <func_dec> → 'global' 'id' ':' 'function' '(' <param_fdec> ')' <ret_type_list>
            
                PARSER_EAT(); /* 'id' */
                CHECK_TOKEN_TYPE(TOKEN_ID);   
                
                // If not already in global symtab - insert it
                HANDLE_SYMTABLE_FUNC_DEC();
                
                ////////////////////////////////////////////////////////////////////////////////////////////
                /** SEMANTIC ACTION - function redeclaration **/
                if (FUNC_ITEM->declared) { 
                    /* Function redeclaration */
                    error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of function '%s'", TOKEN_REPR);
                    return ERR_SEMANTIC_DEF;
                }
                ////////////////////////////////////////////////////////////////////////////////////////////
                
                // Continue parsing 
                PARSER_EAT(); /* ':' */
                CHECK_TOKEN_TYPE(TOKEN_COLON);    

                PARSER_EAT(); /* 'function' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
                CHECK_KEYWORD(KEYWORD_FUNCTION);  
                
                PARSER_EAT(); /* '(' */
                CHECK_TOKEN_TYPE(TOKEN_L_PAR);     

                // <param_fdec>
                PARSER_EAT();
                result = param_fdec(parser); 
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                
                // Already eaten - just check validity 
                CHECK_TOKEN_TYPE(TOKEN_R_PAR); /* ')' */

                // <ret_type_list>
                PARSER_EAT();
                result = ret_type_list(parser); 
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                
                // If everything went well function is now declared
                FUNC_ITEM->declared = true;
                return EXIT_OK;

            } else if (TOKEN_KW_T == KEYWORD_FUNCTION) {
                
                // RULE 4: <func_dec> → ε
                
                return EXIT_OK;
            }
            break;

        case TOKEN_EOF: 
            
            // RULE 4: <func_dec> → ε
            
            return EXIT_OK;
        
        default: break;
    } // switch()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <func_def>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int func_def(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_KEYWORD) {
        
        // RULE 8: <func_def> → <func_head> <stat_list> 'end'
            
        // Expected keyword is 'function' 
        CHECK_KEYWORD(KEYWORD_FUNCTION);
        
        // <func_head> - block_depth = -1
        result = func_head(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

        // Function body starts here 
        parser->curr_block_depth += 1; // increase block depth
        parser->curr_block_id += 1; // unique block id

        // TODO: decrement scope when while ends, or if ends, or else inds

        // <stat_list>
        result = stat_list(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
        
        // No need to eat - current token is 'end'
        CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
        CHECK_KEYWORD(KEYWORD_END);
        generate_function_end();

        // Reset current block depth and id - exiting function definition 
        parser->curr_block_id = 0;
        parser->block_temp_id = 0;
        parser->curr_block_depth = -1;
        return EXIT_OK; 
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <func_call>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int func_call(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_ID) { // RULE 9: <func_call> → 'id' '(' <arg> ')'
        
        //////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION - check whether called function was prev. defined/declared **/
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
        //////////////////////////////////////////////////////////////////////////////////

        char func_id[100];
        strcpy(func_id, parser->token->attribute->string);

        PARSER_EAT(); /* '(' */
        CHECK_TOKEN_TYPE(TOKEN_L_PAR); 
        
        // <arg>
        PARSER_EAT();
        result = arg(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        
        // we dont need to eat, ')' is current token
        CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
        
        ///////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION - check function call argument count **/ 
        if (parser->curr_rhs->function->num_params != parser->curr_arg_count) {
            if (strcmp(parser->curr_rhs->key, "write") != 0) { // write is variadic
                error_message("Parser", ERR_SEMANTIC_PROG, "invalid number of arguments in function '%s'", func_id);
                return ERR_SEMANTIC_PROG;
            }
        }
        ///////////////////////////////////////////////////////////////////////////////////

        // Reset current argument count
        parser->curr_arg_count = 0;
        generate_function_call(func_id);
        return EXIT_OK;
    }
    
    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <arg>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int arg(parser_t *parser)
{
    int result;
    generate_createframe();
    if (TOKEN_T == TOKEN_ID) { // RULE 10: <arg> → <term> <arg_n>
        
        // Check if this variable was previously defined
        // Check if its name does not conflict with function
        // Must be defined in current scope

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // SEMANTIC ACTION - check if actual parameter had been defined in current scope
        symtable_item_t *item;
        /* OLD
        if ((item = symtable_search(SYMTAB_L, TOKEN_REPR)) == NULL) {
            error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' not in this function", TOKEN_REPR);
            return ERR_SEMANTIC_DEF; // TODO:
        } else if ((item->const_var != NULL)) {
            // Variable must be defined and in current block to be used
            if ((!item->const_var->defined) && (item->const_var->block_id == parser->curr_block_id)) {
                error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' not declared", TOKEN_REPR);
            }
            return ERR_SEMANTIC_DEF;
        }
        */
        // TODO: adhoc
        if (parser->curr_block_depth == 0) {
            parser->block_temp_id = parser->curr_block_id;
            parser->curr_block_id = 1;
        } 
        int b_id = parser->curr_block_id;
        int b_depth = parser->curr_block_depth;
        if ((item = most_recent_var(SYMTAB_L, TOKEN_REPR, b_id, b_depth, true)) == NULL) {
            printf("block depth -> %d, id=%d\n\n", parser->curr_block_depth, b_id);
            error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' is not defined here", TOKEN_REPR);
            return ERR_SEMANTIC_DEF;    
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        
        // <term>
        result = term(parser, 1);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
        
        // <arg_n>
        PARSER_EAT(); 
        result = arg_n(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
        
        return EXIT_OK;

    } else if (IS_LITERAL(TOKEN_T) || IS_NIL(TOKEN_T)) { // RULE 10: <arg> → <term> <arg_n>
        
        // <term>
        result = term(parser, 1);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

        // <arg_n>
        PARSER_EAT(); 
        result = arg_n(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
        
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_R_PAR) {
    
        // RULE 11: <arg> → ε
        
        return EXIT_OK;
    }
    
    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <arg_n>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int arg_n(parser_t *parser)
{
    int result;
    static int num_param = 2;
    if (TOKEN_T == TOKEN_COMMA) { // RULE 15: <arg> → ',' <term> <arg_n>
    
        // <term>
        PARSER_EAT(); 
        result = term(parser, num_param);
	    num_param++;
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

        
        // <arg_n> calls itself
        PARSER_EAT();
        return arg_n(parser);

    } else if (TOKEN_T == TOKEN_R_PAR) { 
        
        // RULE 11: <arg> → ε
        num_param = 2;
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <term>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int term(parser_t *parser, int num_param)
{
    if (TOKEN_T == TOKEN_ID) { // RULE 12: <term> → 'id'
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // SEMANTIC ACTION - check if actual parameter had been defined in current block
        symtable_item_t *item;
        if ((item = symtable_search(SYMTAB_L, TOKEN_REPR)) == NULL) {
            error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' not found in local symtab", TOKEN_REPR);
            return ERR_SEMANTIC_DEF; 
        } else if (item->const_var != NULL) {
            // Variable with same id found in this function - check if previously defined
            if (!item->const_var->defined) {
                error_message("Parser", ERR_SEMANTIC_DEF, "variable '%s' undefined", TOKEN_REPR);
                return ERR_SEMANTIC_DEF;
            }
        }
        // Check variable type
        int term_type = item->const_var->type;
        int expected_type = parser->curr_rhs->function->type_params[parser->curr_arg_count];
        //printf("arg -> %s, func: %s\n", TOKEN_REPR, parser->curr_rhs->key);
        if (!is_term_type_valid(term_type, expected_type)) {
            if (strcmp(parser->curr_rhs->key, "write") != 0) { // write function is variadic
                //printf("Arg count: %d, id: %s, type: %d\n", parser->curr_arg_count, TOKEN_REPR, item->const_var->type);
                error_message("Parser", ERR_SEMANTIC_PROG, "invalid argument type");
                return ERR_SEMANTIC_PROG;
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        generate_pass_param_to_function(parser->token , parser->curr_arg_count);
        generate_pass_param_to_operation(parser->token , TOKEN_T); 
        // Keep track of arguments
        parser->curr_arg_count += 1;
        return EXIT_OK;

    } else if (IS_LITERAL(TOKEN_T) || IS_NIL(TOKEN_T)) {

        generate_pass_param_to_operation(parser->token, 0);
        // RULE 15: <term> → 'literal' ... 'literal' = str_lit|int_lit|num_lit
        // RULE 16: <term> → 'nil'
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check variable type
        int expected_type = parser->curr_rhs->function->type_params[parser->curr_arg_count];
        //printf("arg -> %s, func: %s\n", TOKEN_REPR, parser->curr_rhs->key);
        if (!is_term_type_valid(dtype_token(parser), expected_type)) {
            if (strcmp(parser->curr_rhs->key, "write") != 0) { // write function is variadic
                //printf("Arg count2: %d\n", parser->curr_arg_count);
                error_message("Parser", ERR_SEMANTIC_PROG, "invalid argument type");
                return ERR_SEMANTIC_PROG;
            }
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        
        // Keep track of arguments
        parser->curr_arg_count += 1;
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <func_head>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int func_head(parser_t *parser) 
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_KEYWORD: 
            
            // RULE 17: <func_head> → 'function' 'id' '(' <param_fdef> ')' <ret_type_list>
            
            // Expected keyword is 'function' 
            CHECK_KEYWORD(KEYWORD_FUNCTION);
            
            PARSER_EAT(); /* 'id' */
            CHECK_TOKEN_TYPE(TOKEN_ID);
            generate_function_label(TOKEN_REPR);
            // Create item in global symtable - insert func ID 
            HANDLE_SYMTABLE_FUNC_DEF();
            
            //////////////////////////////////////////////////////////////////////////////////////////
            /** SEMANTIC ACTION - function redefinition **/
            if (CURR_FUNC != NULL && CURR_FUNC->defined) { 
                /* Function redefinition */
                error_message("Parser", ERR_SEMANTIC_DEF, "redefinition of function '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF;
            }
            //////////////////////////////////////////////////////////////////////////////////////////

            PARSER_EAT(); /* '(' */
            CHECK_TOKEN_TYPE(TOKEN_L_PAR);
            
            // <param_fdef>
            PARSER_EAT();
            result = param_fdef(parser); 
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

            // TODO: PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); // ')'
            
            // <ret_type_list>
            PARSER_EAT();
            result = ret_type_list(parser); 
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
             
            // If everyting went well, function is now declared and defined
            CURR_FUNC->declared = true;
            CURR_FUNC->defined = true;
            return EXIT_OK;

        default: break; 
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <param_fdef>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int param_fdef(parser_t *parser)
{
    int result;
    
    switch (TOKEN_T)
    {
        case TOKEN_ID: // RULE 18: <param_fdef> → 'id' ':' <dtype> <param_fdef_n>
            
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            // TODO: required?
            /** SEMANTIC ACTION - check invalid variable name **/
            if (symtable_search(SYMTAB_G, TOKEN_REPR)) {
                error_message("Parser", ERR_SEMANTIC_DEF, "invalid variable name '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
            }
	        generate_var_declaration_function(parser->token->attribute->string, 1);
            
            // Create new item in local symtable - check semantics (redeclaration)
            symtable_item_t *item;
            if ((item = symtable_search(SYMTAB_L, TOKEN_REPR)) != NULL) { 
                // item already exists
                error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of variable '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
            }
            ////////////////////////////////////////////////////////////////////////////////////////////// 

            // Insert current variable ID into newly created item in local symtable
            if ((item = symtable_insert_const_var(SYMTAB_L, TOKEN_REPR)) == NULL) {
                return ERR_INTERNAL;
            }
            

            // Continue parsing 
            PARSER_EAT(); /* : */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
            
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            /** SEMANTIC ACTION - check param type mismatch **/
            if ((parser->curr_func != NULL) && (CURR_FUNC->declared && !CURR_FUNC->defined)) {
                ////printf("%s() has %d params declared", parser->curr_func->key, CURR_FUNC->num_params); 
                
                if (CURR_FUNC->num_params < 1) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "param count mismatch");
                    return ERR_SEMANTIC_PROG;                
                }
                
                if (CURR_FUNC->type_params[0] != dtype_keyword(TOKEN_KW_T)) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "function param type mismatch");
                    return ERR_SEMANTIC_PROG;
                }
            } else if ((parser->curr_func != NULL) && !(CURR_FUNC->declared)) {
                // Insert param into symtable(s) only if not already defined
                if (parser->curr_func == NULL) return ERR_INTERNAL; // TODO: FIX THIS
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), CURR_F_KEY);  
            }
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            
            // Store useful data about current parameter
            item->const_var->is_var   = true;
            item->const_var->declared = true;
            item->const_var->defined  = true;
            item->const_var->block_depth = parser->curr_block_depth;
            item->const_var->block_id    = parser->curr_block_id;
            item->const_var->type = dtype_keyword(TOKEN_KW_T);

            // <param_fdef_n>
            PARSER_EAT();
            result = param_fdef_n(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
            
            return EXIT_OK;

        case TOKEN_R_PAR: // RULE 11: <param_fdef> → ε
            
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            /** SEMANTIC ACTION **/ 
            if ((parser->curr_func != NULL) && (CURR_FUNC->num_params != 0)) {
                error_message("Parser", ERR_SEMANTIC_PROG, "function param count mismatch");
                return ERR_SEMANTIC_PROG;
            }
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            
            return EXIT_OK;

        default: break;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <param_fdef_n>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int param_fdef_n(parser_t *parser)
{
    int result;
    static int param_index = 1;

    switch (TOKEN_T)
    {
        case TOKEN_COMMA: // RULE 20: <param_fdef_n> → ',' 'id' ':' <dtype> <param_fdef_n>
            
            PARSER_EAT();/* 'id' */
            CHECK_TOKEN_TYPE(TOKEN_ID); 
	        generate_var_declaration_function(TOKEN_REPR, param_index + 1);
            
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            /** SEMANTIC ACTION - check invalid variable name **/
            if (symtable_search(SYMTAB_G, TOKEN_REPR)) {
                error_message("Parser", ERR_SEMANTIC_DEF, "invalid variable name '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
            }
            
            // Create new item in local symtable - check semantics (redeclaration)
            symtable_item_t *item;
            if ((item = symtable_search(SYMTAB_L, TOKEN_REPR)) != NULL) {
                error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of variable '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
            }
            ////////////////////////////////////////////////////////////////////////////////////////////// 

            // Insert current variable ID into newly created item in local symtable
            if ((item = symtable_insert_const_var(SYMTAB_L, TOKEN_REPR)) == NULL) {
                return ERR_INTERNAL;
            }
            generate_var_declaration(TOKEN_REPR, TOKEN_T);
            PARSER_EAT(); /* ':' */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE(result, EXIT_OK); 
            
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            /** SEMANTIC ACTION - check function's param validity **/
            // If function is already declared check corresponding parameters
            if ((parser->curr_func != NULL) && (CURR_FUNC->declared && !(CURR_FUNC->defined))) {
                
                // Check param count mismatch
                if (param_index+1 > CURR_FUNC->num_params) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "param count mismatch");
                    return ERR_SEMANTIC_PROG;                
                }
                
                // Check current parameter's data type
                if (CURR_FUNC->type_params[param_index] != dtype_keyword(TOKEN_KW_T)) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "function param type mismatch");
                    return ERR_SEMANTIC_PROG;
                }

            } else if ((parser->curr_func != NULL) && !(CURR_FUNC->declared)) {
                // Insert param into symtable(s) only if not already defined
                if (parser->curr_func == NULL) return ERR_INTERNAL; // TODO: FIX THIS
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), CURR_F_KEY);  
            }
            ////////////////////////////////////////////////////////////////////////////////////////////// 
            
            // Store useful data about current parameter
            item->const_var->is_var   = true;
            item->const_var->declared = true;
            item->const_var->defined  = true;
            item->const_var->block_depth = parser->curr_block_depth;
            item->const_var->block_id    = parser->curr_block_id;
            item->const_var->type = dtype_keyword(TOKEN_KW_T);
            // Keep track of new parameters
            param_index++;

            PARSER_EAT();
            return param_fdef_n(parser); // calls itself

        case TOKEN_R_PAR: // RULE 19: <param_fdef_n> → ε

            ////////////////////////////////////////////////////////////////////////////////////////////// 
            /** SEMANTIC ACTION - check if function declaration has more params **/
            if (CURR_FUNC->num_params > param_index) {
                error_message("Parser", ERR_SEMANTIC_PROG, "param count mismatch");
                return ERR_SEMANTIC_PROG;
            }
            //////////////////////////////////////////////////////////////////////////////////////////////
            // Static variable - keep track of param index 
            param_index = 1; 
            return EXIT_OK;

        default: break;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <param_fdec>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int param_fdec(parser_t *parser)
{
    int result;
    
    switch (TOKEN_T)
    {
        case TOKEN_KEYWORD:

            if (IS_DTYPE(TOKEN_KW_T)) { // RULE 22: <param_fdec> → <dtype> <param_fdec_n>

                // <dtype>
                result = dtype(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                
                // Insert into symtable(s)
                if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  

                // <param_fdec_n>
                PARSER_EAT();
                result = param_fdec_n(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                
                return EXIT_OK;
            }
            break; // TODO: beware, must end up in error

        case TOKEN_R_PAR: // RULE 23: <param_fdec> → ε
            
            return EXIT_OK;

        default: break;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <param_fdec_n>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int param_fdec_n(parser_t *parser)
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_COMMA: // RULE 24: <param_fdec_n> → ',' <dtype> <param_fdec_n>
            
            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); // TODO: check me 
            
            // Insert into symtable(s)
            if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
            symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
            
            PARSER_EAT();
            return param_fdec_n(parser); // calls itself

        case TOKEN_R_PAR: // RULE 25: <param_fdec_n> → ε
            return EXIT_OK;
        
        default: break;
    } // switch()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <ret_type_list>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int ret_type_list(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_COLON) { // RULE 26: <ret_type_list> → ':' <dtype> <ret_type_list_n>
        
        /* ':' */ 
        CHECK_TOKEN_TYPE(TOKEN_COLON);

        // <dtype>
        PARSER_EAT();
        result = dtype(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        // If function was previously declared check validity of return value types
        if ((parser->curr_func != NULL) && (CURR_FUNC->declared && !(CURR_FUNC->defined))) {
            
            // Check return value count
            if (CURR_FUNC->num_ret_types < 1) {
                error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
                return ERR_SEMANTIC_PROG;                
            }

            if (CURR_FUNC->ret_types[0] != dtype_keyword(TOKEN_KW_T)) {
                error_message("Parser", ERR_SEMANTIC_PROG, "function return type mismatch");
                return ERR_SEMANTIC_PROG;
            }
            
        } else if ((parser->curr_item != NULL) && (FUNC_ITEM != NULL)) {
            // Insert param into symtable(s) only if not already defined
            if (!FUNC_ITEM->defined) {// TODO: check this
                symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
            }
        } 
        //////////////////////////////////////////////////////////////////////////////////////////////

        // Continue parsing
        // <ret_type_list_n>
        PARSER_EAT();
        result = ret_type_list_n(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_KEYWORD) {
                
        switch (TOKEN_KW_T) 
        {
            case KEYWORD_FUNCTION:
            case KEYWORD_RETURN:
            case KEYWORD_WHILE:
            case KEYWORD_GLOBAL:
            case KEYWORD_LOCAL:
            case KEYWORD_END:
            case KEYWORD_IF:
            
                // RULE 26: <ret_type_list> → ε

                //////////////////////////////////////////////////////////////////////////////////////////////
                /** SEMANTIC ACTION**/ 
                if ((parser->curr_func != NULL) && (CURR_FUNC->num_ret_types != 0)) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "function return type count mismatch");
                    return ERR_SEMANTIC_PROG;
                }
                //////////////////////////////////////////////////////////////////////////////////////////////
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) { // RULE 27: <ret_type_list> → ε
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION**/ 
        if ((parser->curr_func != NULL) && (CURR_FUNC->num_ret_types != 0)) {
            error_message("Parser", ERR_SEMANTIC_PROG, "function return type count mismatch");
            return ERR_SEMANTIC_PROG;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_EOF) { // RULE 27: <ret_type_list> → ε
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION **/ 
        if ((parser->curr_func != NULL) && (CURR_FUNC->num_ret_types != 0)) {
            error_message("Parser", ERR_SEMANTIC_PROG, "function return type count mismatch");
            return ERR_SEMANTIC_PROG;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <ret_type_list_n>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int ret_type_list_n(parser_t *parser)
{
    int result;
    static int ret_type_index = 1;

    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T) 
        {
            case KEYWORD_FUNCTION:
            case KEYWORD_GLOBAL:
            case KEYWORD_LOCAL:
            case KEYWORD_RETURN:
            case KEYWORD_END:
            case KEYWORD_IF:
            case KEYWORD_WHILE:
                
                // RULE 29: <ret_type_list_n> → ε

                /*
                if (parser->curr_func != NULL && !parser->curr_func->function->defined) {
                    //printf("curr ret types: %d, curr rt index: %d\n\n", CURR_FUNC->num_ret_types, ret_type_index);
                } */

                //////////////////////////////////////////////////////////////////////////////////////////////
                /** SEMANTIC ACTION - check if function declaration has more params **/
                if ((parser->curr_func != NULL) && CURR_FUNC->num_ret_types > ret_type_index) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
                    return ERR_SEMANTIC_PROG;
                }
                //////////////////////////////////////////////////////////////////////////////////////////////
                ret_type_index = 1; 
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_COMMA) { // RULE 28: <ret_type_list_n> → ',' <dtype> <ret_type_list_n>
            
        // <dtype>
        PARSER_EAT();
        result = dtype(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
       
        //////////////////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION - check function's return values validity **/
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
            
            // If function was declared we dont want to insert its return values
        } else if ((parser->curr_item != NULL) && (FUNC_ITEM != NULL)) {
            // Insert param into symtable(s) - its declaration
            if (!FUNC_ITEM->defined) { // TODO: check this
                symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
            }
        }/* 
        } else if ((parser->curr_func != NULL) && !(CURR_FUNC->defined)) {
            // Insert param into symtable(s) only if not already defined
            if (parser->curr_func == NULL) return ERR_INTERNAL; // TODO: FIX THIS
            symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), CURR_F_KEY);  
        }*/
        //////////////////////////////////////////////////////////////////////////////////////////////
         
        ret_type_index++;
        
        PARSER_EAT();
        return ret_type_list_n(parser); // calls itself          
    
    } else if (TOKEN_T == TOKEN_ID) { // RULE 29: <ret_type_list_n> → ε
        // TODO: MAYBE BS - CHECK TAB/RULES

        //////////////////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION - check if function declaration has more params **/
        if (CURR_FUNC->num_ret_types > ret_type_index) {
            error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
            return ERR_SEMANTIC_PROG;
        
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        // Reset static variable
        ret_type_index = 1; 
        
        // TODO:  add ret ID into the symtable 
        // - dont forget to also add return types
        
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_EOF) { // RULE 29: <ret_type_list_n> → ε
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION - check if function declaration has more params **/
        if (CURR_FUNC->num_ret_types > ret_type_index) {
            error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
            return ERR_SEMANTIC_PROG;
        
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        // Reset static variable
        ret_type_index = 1; 
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <stat_list>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int stat_list(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T) 
        {
            case KEYWORD_LOCAL: 
            case KEYWORD_IF:
            case KEYWORD_WHILE:
            case KEYWORD_RETURN:

                // RULE 30: <stat_list> → <stat> <stat_list>
                
                // <stat>
                result = stat(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                
                return stat_list(parser); // calls itself  

            case KEYWORD_END:
            case KEYWORD_ELSE:
                
                // RULE 31: <stat_list> → ε
                
                return EXIT_OK;

            default:break;    
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) { // RULE 30: <stat_list> → <stat> <stat_list>
        // <stat>
        result = stat(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        return stat_list(parser); // calls itself  
    } 
    
    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);    
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <stat>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int stat(parser_t *parser)
{
    int result;
    char id_name[100];
    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T) 
        {
            case KEYWORD_LOCAL: // RULE 32: <stat> → 'local' 'id' ':' <dtype> <var_def> 

                PARSER_EAT(); /* 'id' */
                CHECK_TOKEN_TYPE(TOKEN_ID);

                ///////////////////////////////////////////////////////////////////////////////////////////// 
                /** SEMANTIC ACTION - check invalid variable name **/
                if (symtable_search(SYMTAB_G, TOKEN_REPR)) {
                    error_message("Parser", ERR_SEMANTIC_DEF, "invalid variable name '%s'", TOKEN_REPR);
                    return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
                }
                
                // Create new item in local symtable - check semantics (redeclaration)
                if (would_be_var_redeclared(SYMTAB_L, TOKEN_REPR, parser->curr_block_id)) {
                    error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of variable '%s'", TOKEN_REPR);
                    return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
                }

                symtable_item_t *item;
                // Insert current variable ID into newly created item in local symtable
                if ((item = symtable_insert_const_var(SYMTAB_L, TOKEN_REPR)) == NULL) {
                    return ERR_INTERNAL;
                }
                ////////////////////////////////////////////////////////////////////////////////////////////
                
                if (parser->curr_block_depth == 0) {
                    parser->block_temp_id = parser->curr_block_id;
                    parser->curr_block_id = 1;
                } 

                // Set current block info
                if (parser->curr_block_id == 1 && parser->curr_block_depth == 0) {
                    item->const_var->block_id = parser->block_temp_id;
                } else {
                    item->const_var->block_id = parser->curr_block_id;
                }

                item->const_var->block_depth = parser->curr_block_depth;

                
                strcpy(id_name, TOKEN_REPR);
                generate_var_declaration(id_name, TOKEN_T);

                PARSER_EAT(); /* ':' */
                CHECK_TOKEN_TYPE(TOKEN_COLON);
                
                // <dtype>
                PARSER_EAT();
                result = dtype(parser);
                CHECK_RESULT_VALUE(result, EXIT_OK); 
               
                ////printf("function id: %s\n", parser->curr_func->key); 
                ////printf("Curr id: %s\n", item->key);
                ////printf("Curr block: id [%d], depth: %d\n\n", parser->curr_block_id, parser->curr_block_depth);
                // TODO: print also item block info

                // Store useful data about current parameter
                item->const_var->is_var = true;
                item->const_var->declared = true; //TODO: definition
                item->const_var->type = dtype_keyword(TOKEN_KW_T);
                parser->curr_item = item; // TODO: do this in assignment case also
                
                // <var_def>
                PARSER_EAT();
                result = var_def(parser, id_name);
                item->const_var->defined = true; //TODO: definition !! not always

		        generate_pop_stack_to_var(id_name);

                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                
                return EXIT_OK;
            
            case KEYWORD_IF: // RULE 33: <stat> → 'if' 'expr' 'then' <stat_list> <else> 'end'
                
                // Current token is 'if' - switch context
                result = analyze_bottom_up(parser);
                CHECK_RESULT_VALUE(result, EXIT_OK);

                // 'then'
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
                CHECK_KEYWORD(KEYWORD_THEN);
                
                // Keep track of current block info
                if (parser->curr_block_id == 1 && parser->curr_block_depth == 0) {
                    parser->curr_block_id = parser->block_temp_id + 1;
                } else {
                    parser->curr_block_id += 1;
                }
                parser->curr_block_depth += 1;

                // <stat_list>
                PARSER_EAT();
                result = stat_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                
                // <else> - do not eat, we need this token
                result = else_nt(parser); 
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                 
                // Do not eat, 'end' is current token
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD);
                CHECK_KEYWORD(KEYWORD_END);

                // Leaving this block -> decrement depth
                parser->curr_block_depth -= 1;
                
                PARSER_EAT(); // to get next statement 
                return EXIT_OK;
            
            case KEYWORD_WHILE: // RULE 34: <stat> → 'while' 'expr' 'do' <stat_list> 'end'
                
                // Current token is 'while' - switch context
                result = analyze_bottom_up(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                /*
                if ((result == EXIT_EMPTY_EXPR) || (result == ERR_SYNTAX)) {
                    error_message("Parser", ERR_SYNTAX, "expression parsing failed");
                    return ERR_SYNTAX; // missing or invalid  expression
                }*/

                /* 'do' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
                CHECK_KEYWORD(KEYWORD_DO);
                
                // Keep track of current block info
                if (parser->curr_block_id == 1 && parser->curr_block_depth == 0) {
                    parser->curr_block_id = parser->block_temp_id + 1;
                } else {
                    parser->curr_block_id += 1;
                }
                parser->curr_block_depth += 1;

                // <stat_list>
                PARSER_EAT(); 
                result = stat_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); // 'end'
                CHECK_KEYWORD(KEYWORD_END);
                
                // Leaving this block -> decrement depth
                parser->curr_block_depth -= 1;

                PARSER_EAT(); // to get next statement 
                return EXIT_OK;

            case KEYWORD_RETURN: // RULE 36: <stat> → 'return' 'expr' <expr_list>

                // Current token is  'return'
                result = analyze_bottom_up(parser);
                if ((result != EXIT_EMPTY_EXPR) && (result != EXIT_OK)) {
                    printf("res: %d\n", result); 
                    return result; 
                }
                // CHECK_RESULT_VALUE(result, EXIT_OK); 

                // Currently we either parsed expression, or the expression was
                // empty, in both cases we continue further - unlike with if and while

                // Switch context to the precedence analysis
                // If no expression is present, returns current token back

                // <expr_list>
                result = expr_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                 
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) { 
        
        // RULE 35: <stat> → 'id' <id_n> '=' 'expr' <expr_list>

        // Beware - nondeterminism is possible here! - solved adhoc 
        //          <stat> → 'id' <id_n> '=' 'id' '(' term_list ')'
        //          <stat> → 'id' '(' term_list ')' 

        strcpy(id_name, parser->token->attribute->string);
        symtable_item_t *item; 
        if ((item = symtable_search(SYMTAB_G, TOKEN_REPR)) != NULL) {
            // Current ID is function id
            if ((item->function != NULL) && (item->function->declared)) {
                result = func_call(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
		        generate_pop_stack_to_var(id_name);
                PARSER_EAT();
                return EXIT_OK;
            }
        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////
        /** SEMANTIC ACTION **/
        // Current id is variable or undeclared function - its an assignment
        // Check whether it was at least declared
        symtable_item_t *item_dec; 
        int b_id = parser->curr_block_id;
        int b_depth = parser->curr_block_depth;
        if (!(item_dec = most_recent_var(SYMTAB_L, TOKEN_REPR, b_id, b_depth, false))) {
            error_message("Parser", ERR_SEMANTIC_DEF, "undeclared variable/function '%s'", TOKEN_REPR);
            return ERR_SEMANTIC_DEF;
        } 
        //////////////////////////////////////////////////////////////////////////////////////////////
        
        // <id_n> 
	strcpy(id_name, parser->token->attribute->string);
        PARSER_EAT();
        result = id_n(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        
        /* '=' */
        CHECK_TOKEN_TYPE(TOKEN_ASSIGN);
        
        // *ATTENTION* - nondeterminism handling - func id vs var id
        result = analyze_bottom_up(parser);
        switch (result) 
        { // TODO: Define const var
            case EXIT_OK:
                item_dec->const_var->defined = true;
                result = expr_list(parser);
		        generate_pop_stack_to_var(id_name);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                return EXIT_OK;

            case EXIT_FUNC_ID:
                //item_dec->const_var->defined = true; TODO: wtf
                // Semantic check handled by func_call()
                result = func_call(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                PARSER_EAT();
                return EXIT_OK;

            case EXIT_ID_BEFORE:
                if (TOKEN_T == TOKEN_L_PAR) {
                    // Potential calling of undefined/undeclared function 
                    
                    // <arg>
                    PARSER_EAT();
                    result = arg(parser);
                    CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                    
                    //////////////////////////////////////////////////////////////////////////////////////////////
                    // we dont need to eat, ')' is current token
                    CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
                    error_message("Parser", ERR_SEMANTIC_DEF, "called function not previously declared nor defined");
                    return ERR_SEMANTIC_DEF; 
                }
                error_message("Parser", ERR_SYNTAX, "expression parsing failed");
                return ERR_SYNTAX; // missing or invalid  expression

            default: // other errors
                return result;
        } // switch
    }
    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

/**
 * @brief Nonterminal <else>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int else_nt(parser_t *parser)
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_KEYWORD:
            if (TOKEN_KW_T == KEYWORD_ELSE) {

                // RULE 39: <else> → 'else' <stat_list>
                // Current token is keyword 'else'
                
                // Keep track of current block info
                if (parser->curr_block_id == 1 && parser->curr_block_depth == 0) {
                    parser->curr_block_id = parser->block_temp_id + 1;
                } else {
                    parser->curr_block_id += 1;
                }

                // <stat_list> 
                PARSER_EAT();
                result = stat_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

                return EXIT_OK;
            
            } else if (TOKEN_KW_T == KEYWORD_END) {
                
                // RULE 40: <else> → ε

                return EXIT_OK;
            }
            break; // TODO: beware must end up in error

        default: break;
    } // switch()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
} 


/**
 * @brief Nonterminal <var_def>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int var_def(parser_t *parser, char *id_name)
{
    int result;
    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T) 
        { 
            case KEYWORD_END:
            case KEYWORD_LOCAL:
            case KEYWORD_IF:
            case KEYWORD_WHILE:
            case KEYWORD_RETURN:
            case KEYWORD_ELSE:
                
                // RULE 42: <var_def> → ε

                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ASSIGN) { // RULE 41: <var_def> → '=' 'expr'
        
        // Current token is '='

        // *ATTENTION* - nondeterminism handling - func id vs var id
        result = analyze_bottom_up(parser);

        generate_pop_stack_to_var(id_name);
        switch (result) 
        {
            case EXIT_OK:
                result = expr_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                parser->curr_item->const_var->defined = true;
                return EXIT_OK;

            case EXIT_FUNC_ID:
                parser->curr_item->const_var->defined = true;
                // Semantic check handled by func_call()
                result = func_call(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                PARSER_EAT();
                return EXIT_OK;

            case EXIT_ID_BEFORE:
                if (TOKEN_T == TOKEN_L_PAR) {
                    // Potential calling of undefined/undeclared function 
                    
                    // <arg>
                    PARSER_EAT();
                    result = arg(parser);
                    CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                    
                    //////////////////////////////////////////////////////////////////////////////////////////////
                    // we dont need to eat, ')' is current token
                    CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
                    error_message("Parser", ERR_SEMANTIC_DEF, "called function not previously declared nor defined");
                    return ERR_SEMANTIC_DEF; 
                }
                error_message("Parser", ERR_SYNTAX, "expression parsing failed");
                return ERR_SYNTAX; // missing or invalid  expression

            default: // other errors
                return result;
        } // switch

    } else if (TOKEN_T == TOKEN_ID) {
        
        // RULE 42: <var_def> → ε

        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <id_n>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int id_n(parser_t *parser)
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_COMMA:
            
            // RULE 43: <id_n> → ',' 'id' <id_n>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id' 
            
            //////////////////////////////////////////////////////////////////////////////////////////////
            // Current id is variable - its an assignment
            // Check whether it was at least declared
            symtable_item_t *item_dec;
            int b_id = parser->curr_block_id;
            int b_depth = parser->curr_block_depth;
            if (!(item_dec = most_recent_var(SYMTAB_L, TOKEN_REPR, b_id, b_depth, false))) {
                error_message("Parser", ERR_SEMANTIC_DEF, "undeclared variable '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF;
            } 
            //////////////////////////////////////////////////////////////////////////////////////////////

            PARSER_EAT(); // TODO: has to be here
            return id_n(parser);

        case TOKEN_ASSIGN:

            // RULE 44: <id_n> → ε

            return EXIT_OK;

        case TOKEN_L_PAR: // TODO: adhoc - not in grammar
            // Check if current statement is function call of undefined
            // function or syntax error
            
            // <arg>
            PARSER_EAT();
            result = arg(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
            
            //////////////////////////////////////////////////////////////////////////////////////////////
            // we dont need to eat, ')' is current token
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
            error_message("Parser", ERR_SEMANTIC_DEF, "called function not previously declared nor defined");
            return ERR_SEMANTIC_DEF;

        default: break;
    } // switch()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <expr_list>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int expr_list(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T)
        {
            case KEYWORD_END:
            case KEYWORD_LOCAL:
            case KEYWORD_IF:
            case KEYWORD_WHILE:
            case KEYWORD_RETURN:
            case KEYWORD_ELSE: 

                // RULE 38: <expr_list> → ε
                
                return EXIT_OK;

            default: break;
        } // switch() 

    } else if (TOKEN_T == TOKEN_COMMA) {
            
        // RULE 37: <expr_list> → ',' 'expr' <expr_list>

        // Current token is ','
        result = analyze_bottom_up(parser);
        if ((result != EXIT_EMPTY_EXPR) && (result != EXIT_OK)) {
            printf("res: %d\n", result); 
            return result; 
        }
        //CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        
        return expr_list(parser);
    
    } else if (TOKEN_T == TOKEN_ID) {

        // RULE 38: <expr_list> → ε
        
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <dtype>
 * @param parser pointer to the parser structure
 * @return exit code
 */
int dtype(parser_t *parser)
{
    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T) 
        {
            case KEYWORD_NIL:     // RULE 45: <dtype> → 'nil'
            case KEYWORD_NUMBER:  // RULE 46: <dtype> → 'number'
            case KEYWORD_INTEGER: // RULE 47: <dtype> → 'integer'
            case KEYWORD_STRING:  // RULE 48: <dtype> → 'string'
                return EXIT_OK;
            default: break;
        } // switch()
    } // if()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}

// TODO: consider idea of looking at 'expr' terminal as a NONTERMINAL - thus
// appropriate function must be called no matter what, because it is its time
