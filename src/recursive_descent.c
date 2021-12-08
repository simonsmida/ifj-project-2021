#include <stdbool.h>
#include <string.h>
#include "include/recursive_descent.h"


bool is_write = false;
bool first_used_function = true;


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
    SEMANTIC_ACTION(check_declared_function_defined, parser); 
    
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
            SEMANTIC_ACTION(check_declared_function_defined, parser);
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
                 
                SEMANTIC_ACTION(check_function_redeclaration, parser);
                
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
            SEMANTIC_ACTION(check_declared_function_defined, parser); 
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

        // <stat_list>
        result = stat_list(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
        
        // No need to eat - current token is 'end'
        CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
        CHECK_KEYWORD(KEYWORD_END);
        generate_function_end();

        // Reset current block depth and id - exiting function definition 
        parser->curr_func = NULL;
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
	char func_id[100];
    if (TOKEN_T == TOKEN_ID) { // RULE 9: <func_call> → 'id' '(' <arg> ')'
        
        /*** Check whether called function was declared ***/ 
        SEMANTIC_ACTION(check_function_call, parser);
        
        strcpy(func_id, TOKEN_REPR);
        if (!strcmp(func_id, "write")) {is_write = true;}

		if (parser->curr_func == NULL){
			if (first_used_function == true){
				generate_main_scope();
				first_used_function = false;
			}
		}
        
        PARSER_EAT(); /* '(' */
        CHECK_TOKEN_TYPE(TOKEN_L_PAR); 
        
        // <arg>
        PARSER_EAT();
        result = arg(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        
        // we dont need to eat, ')' is current token
        CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
        
        SEMANTIC_ACTION(check_function_call_arg_count, parser);
		
        if (is_write == false) {
            generate_function_call(func_id);
		}
		is_write = false;

        // Reset current argument count
        parser->curr_arg_count = 0;
		
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
	if (is_write == false) {
    	generate_createframe();
	}

    if (TOKEN_T == TOKEN_ID) { // RULE 10: <arg> → <term> <arg_n>
        
        // If current depth is 0 it means we
        // are in the main block of the function body - block_id = 1
        CHECK_MAIN_BLOCK();
        
        //SEMANTIC_ACTION(check_invalid_variable_name, parser);
        //SEMANTIC_ACTION(check_arg_def, parser);

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
        
        /*** Check colliding name with function name ***/
        SEMANTIC_ACTION(check_invalid_variable_name, parser);
        /*** Check argument's previous  definition ***/
        SEMANTIC_ACTION(check_arg_def, parser);
        /*** Check arguemt count validity ***/
        SEMANTIC_ACTION(check_arg_count, parser);
        /*** Check id data type compatibility ***/
        SEMANTIC_ACTION(check_arg_type_id, parser);
        
        // SEMANTIC_ACTION(check_undefined_arg, parser); 
        
        if (is_write) {
		    generate_built_in_write(parser->token, 
                                    parser->curr_func->key, 
                                    parser->curr_block_depth,
									parser->array_depth);
		} else {
            generate_pass_param_to_function(parser->token,parser->curr_func->key, parser->curr_block_depth, parser->array_depth, num_param);
		}

        // Keep track of arguments
        parser->curr_arg_count += 1;
        return EXIT_OK;

    } else if (IS_LITERAL(TOKEN_T) || IS_NIL(TOKEN_T)) {

      	if (is_write) {
		    generate_built_in_write(parser->token, 
                                    parser->curr_func->key, 
                                    parser->curr_block_depth,
									parser->array_depth);
		} else { // TODO: segfault curr_func
            //generate_pass_param_to_function(parser->token,parser->curr_func->key, parser->curr_block_depth, parser->array_depth, num_param);
		}

        // RULE 15: <term> → 'literal' ... 'literal' = str_lit|int_lit|num_lit
        // RULE 16: <term> → 'nil'
        /*** Check arguemt count validity ***/
        SEMANTIC_ACTION(check_arg_count, parser);
        
        /*** Check id data type compatibility ***/
        SEMANTIC_ACTION(check_arg_type_literal, parser);

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
            
            /*** Check current function's redefinition ***/
            SEMANTIC_ACTION(check_function_redefinition, parser); 

            PARSER_EAT(); /* '(' */
            CHECK_TOKEN_TYPE(TOKEN_L_PAR);
           
            // <param_fdef>
            PARSER_EAT();
            result = param_fdef(parser); 
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

            // Expecting token ')'
            CHECK_TOKEN_TYPE(TOKEN_R_PAR);
            
            // <ret_type_list>
            PARSER_EAT();
            result = ret_type_list(parser); 
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
             
            // If everyting went well, function is now declared and defined
            CURR_FUNC->declared = true;
            CURR_FUNC->defined = true;
            return EXIT_OK;

        default: break; 
    } // switch()

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
            
            generate_var_declaration_function(TOKEN_REPR, 
                                              parser->curr_func->key, 
                                              parser->curr_block_depth, 
                                              parser->array_depth, 1);
            
            /*** Check coliding name with function name ***/
            SEMANTIC_ACTION(check_invalid_variable_name, parser);
            /*** Check redeclaration attempt of current parameter***/
            SEMANTIC_ACTION(check_param_redeclaration, parser); 

            // Insert current variable ID into newly created item in local symtable
            if (!(parser->curr_item = symtable_insert_const_var(SYMTAB_L, TOKEN_REPR))) {
                return ERR_INTERNAL;
            }
            parser->curr_item->const_var->block_depth = -1;
            parser->curr_item->const_var->declared = true;
            parser->curr_item->const_var->defined = true;

            // Continue parsing 
            PARSER_EAT(); /* : */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
            
            /*** Check param count and param type ***/
            SEMANTIC_ACTION(check_param_mismatch, parser, 0);

            // If function not declared - insert this parameter 
            if (parser->curr_func != NULL && !CURR_FUNC->declared) {
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), CURR_F_KEY);  
            }
            
            // Set these: is_var, declared, defined, block_depth, block_id, type
            SET_PARAM_INFO();

            // <param_fdef_n>
            PARSER_EAT();
            result = param_fdef_n(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
            
            return EXIT_OK;

        case TOKEN_R_PAR: // RULE 11: <param_fdef> → ε
            
            /** SEMANTIC ACTION **/ 
            if ((parser->curr_func != NULL) && (CURR_FUNC->num_params != 0)) {
                error_message("Parser", ERR_SEMANTIC_PROG, "function param count mismatch");
                return ERR_SEMANTIC_PROG;
            }
            
            return EXIT_OK;

        default: break;
    } // switch()

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
	    	generate_var_declaration_function(TOKEN_REPR, 
                                              parser->curr_func->key, 
                                              parser->curr_block_depth, 
                                              parser->array_depth,  
                                              param_index + 1);
	        
            
            SEMANTIC_ACTION(check_invalid_variable_name, parser);
            SEMANTIC_ACTION(check_param_redeclaration, parser); 

            // Insert current variable ID into newly created item in local symtable
            if (!(parser->curr_item = symtable_insert_const_var(SYMTAB_L, TOKEN_REPR))) {
                return ERR_INTERNAL;
            }
            parser->curr_item->const_var->block_depth = -1;
            parser->curr_item->const_var->declared = true;
            parser->curr_item->const_var->defined = true;

            PARSER_EAT(); /* ':' */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE(result, EXIT_OK); 
            
            /*** Check param count and param type ***/
            SEMANTIC_ACTION(check_param_mismatch, parser, param_index);
            
            // Insert param into symtable(s) only if not declared
            if (parser->curr_func != NULL && !CURR_FUNC->declared) {
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), CURR_F_KEY);  
            }
            
            // Set these: is_var, declared, defined, block_depth, block_id, type
            SET_PARAM_INFO();
            
            // Keep track of new parameters
            param_index++;

            PARSER_EAT();
            return param_fdef_n(parser); // calls itself

        case TOKEN_R_PAR: // RULE 19: <param_fdef_n> → ε

            /** SEMANTIC ACTION - check if function declaration has more params **/
            if (CURR_FUNC->num_params > param_index+1) {
                error_message("Parser", ERR_SEMANTIC_PROG, "param count mismatch");
                return ERR_SEMANTIC_PROG;
            }
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
                char *key = parser->curr_item->key;
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), key);  

                // <param_fdec_n>
                PARSER_EAT();
                result = param_fdec_n(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                
                return EXIT_OK;
            }
            break;

        case TOKEN_R_PAR: // RULE 23: <param_fdec> → ε 
            return EXIT_OK;

        default: break;
    } // switch()

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
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
            
            // Insert into symtable(s)
            if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
            char *key = parser->curr_item->key;
            symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), key);  
            
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
       
        SEMANTIC_ACTION(check_return_values, parser, 0);
        
        // Distinguish between function declaration and definition
        if (parser->curr_func == NULL) {
            char *fdec_key = parser->curr_item->key;
            // Function declaration
            symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), fdec_key);  
        } else {
            char *fdef_key = parser->curr_func->key;
            // Function definition - insert only if not declared
            if (!CURR_FUNC->declared) {
                symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), fdef_key); 
            }
        }
        
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
                
                if (parser->curr_func != NULL) { // check for declared functions
                    SEMANTIC_ACTION(check_no_return_values, parser);
                }
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) { // RULE 27: <ret_type_list> → ε
       
        if (parser->curr_func != NULL) { // check for declared functions
            SEMANTIC_ACTION(check_no_return_values, parser);
        }
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_EOF) { // RULE 27: <ret_type_list> → ε
        
        if (parser->curr_func != NULL) { // check for declared functions
            SEMANTIC_ACTION(check_no_return_values, parser); 
        }

        /*** Check if every declared function was defined ***/
        SEMANTIC_ACTION(check_declared_function_defined, parser); 
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

                SEMANTIC_ACTION(check_return_value_count, parser, ret_type_index);
                ret_type_index = 1; 
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_COMMA) { // RULE 28: <ret_type_list_n> → ',' <dtype> <ret_type_list_n>
            
        // <dtype>
        PARSER_EAT();
        result = dtype(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
       
        /*** Check return value count and type mismatch ***/
        SEMANTIC_ACTION(check_return_values, parser, ret_type_index);

        // Distinguish between function declaration and definition
        if (parser->curr_func == NULL) {
            char *fdec_key = parser->curr_item->key;
            // Function declaration
            symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), fdec_key);  
        } else {
            char *fdef_key = parser->curr_func->key;
            // Function definition - insert only if not declared
            if (!CURR_FUNC->declared) {
                symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), fdef_key); 
            }
        }

        // Keep track of ret type index
        ret_type_index++;     
        
        PARSER_EAT();
        return ret_type_list_n(parser); // calls itself          
    
    } else if (TOKEN_T == TOKEN_ID) { // RULE 29: <ret_type_list_n> → ε
        
        /*** Check return value count ***/
        SEMANTIC_ACTION(check_return_value_count, parser, ret_type_index);
        // Reset static variable
        ret_type_index = 1; 
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_EOF) { // RULE 29: <ret_type_list_n> → ε
        
        /*** Check return value count ***/
        SEMANTIC_ACTION(check_return_value_count, parser, ret_type_index); 

        /*** Check if every declared function was defined ***/
        SEMANTIC_ACTION(check_declared_function_defined, parser); 
        
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

                
                SEMANTIC_ACTION(check_invalid_variable_name, parser);
                SEMANTIC_ACTION(check_variable_redeclaration, parser); 

                // Insert current variable ID into newly created item in local symtable
                if (!(parser->curr_item = symtable_insert_const_var(SYMTAB_L, TOKEN_REPR))) {
                    return ERR_INTERNAL;
                }
                            
                // Check if block depth is 0 -> then block_id must be set to 1
                CHECK_MAIN_BLOCK();
                parser->curr_item->const_var->block_depth = parser->curr_block_depth;
                // Set current block info
                if (parser->curr_block_id == 1) {
                    parser->curr_item->const_var->block_id = parser->block_temp_id;
                } else {
                    parser->curr_item->const_var->block_id = parser->curr_block_id;
                }
                //parser->curr_item->const_var->block_id = parser->curr_block_id;
                //fprintf(stderr, "(%s) -> [%d]: %d\n", parser->curr_item->key, parser->curr_item->const_var->block_id,
                  //                                    parser->curr_item->const_var->block_depth); 
                
                
                strcpy(id_name, TOKEN_REPR);
                char *key = parser->curr_func->key; //TODO: REMOVEME
                int depth = parser->curr_block_depth; // TODO: REMOVE
                int adp = parser->array_depth[depth]; // TODO: REMOVE
				if (parser->inside_while == false) {
                	generate_var_declaration(id_name, key, parser->array_depth , depth);
				} else {
					char some_string[200];
					sprintf(some_string,"DEFVAR LF@%s$%s$%d$%d\n", id_name, key, adp, depth);
					append_string(parser->buffer, some_string);
				}

                PARSER_EAT(); /* ':' */
                CHECK_TOKEN_TYPE(TOKEN_COLON);
                
                // <dtype>
                PARSER_EAT();
                result = dtype(parser);
                CHECK_RESULT_VALUE(result, EXIT_OK); 
               
                //printf("function id: %s\n", parser->curr_func->key); 
                //printf("Curr id: %s\n", parser->curr_item->key);
                //printf("Curr block: id [%d], depth: %d\n\n", parser->curr_block_id, parser->curr_block_depth);

                // Store useful data about current parameter
                parser->curr_item->const_var->is_var = true;
                parser->curr_item->const_var->declared = true; //TODO: definition
                parser->curr_item->const_var->type = dtype_keyword(TOKEN_KW_T);
                
                // <var_def>
                PARSER_EAT();
                result = var_def(parser, id_name);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                return EXIT_OK;
            
            case KEYWORD_IF: // RULE 33: <stat> → 'if' 'expr' 'then' <stat_list> <else> 'end'
                
                // Current token is 'if' - switch context
                result = analyze_bottom_up(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

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

                parser->array_depth[parser->curr_block_depth]++;
                generate_label_if(parser->curr_func->key, 
                                  parser->array_depth, 
                                  parser->curr_block_depth);

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

				// Generate code before decrementing depth
				generate_label_if_end(parser->curr_func->key, 
                                      parser->array_depth, 
                                      parser->curr_block_depth);

                // Leaving this block -> decrement depth
                parser->curr_block_depth -= 1;
                CHECK_MAIN_BLOCK();

                PARSER_EAT(); // to get next statement 
                return EXIT_OK;
            
            case KEYWORD_WHILE: // RULE 34: <stat> → 'while' 'expr' 'do' <stat_list> 'end'

				// We update block_depth before bottom_up analysis because of the need to print labels now
                parser->curr_block_depth += 1;
                parser->curr_block_id += 1;
                parser->array_depth[parser->curr_block_depth]++;

                parser->inside_while = true;
                generate_while_repeat_label(parser->curr_func->key, 
                                            parser->curr_block_depth, 
                                            parser->array_depth);
                
                // Current token is 'while' - switch context
                result = analyze_bottom_up(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

				generate_jump_while_end(parser->curr_func->key, 
                                        parser->curr_block_depth, 
                                        parser->array_depth);

                /* 'do' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
                CHECK_KEYWORD(KEYWORD_DO);
                
                /*
                // Keep track of current block info
                if (parser->curr_block_id == 1 && parser->curr_block_depth == 0) {
                    parser->curr_block_id = parser->block_temp_id + 1;
                } else {
                    parser->curr_block_id += 1;
                }*/

                // <stat_list>
                PARSER_EAT(); 
                result = stat_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); // 'end'
                CHECK_KEYWORD(KEYWORD_END);
                
                generate_while_end_label(parser->curr_func->key, 
                                         parser->curr_block_depth, 
                                         parser->array_depth, 
                                         parser->buffer);
                // We now get rid of this buffer, as it is no longer needed
                // But we will need it again in the future so we init it again
                destroy_buffer(parser->buffer);
                parser->buffer = init_buffer();
                
                // Leaving this block -> decrement depth
                parser->curr_block_depth -= 1;
                CHECK_MAIN_BLOCK();
		parser->inside_while = false;

                PARSER_EAT(); // to get next statement 
                return EXIT_OK;

            case KEYWORD_RETURN: // RULE 36: <stat> → 'return' 'expr' <expr_list>
                
                // Set this variable to be sure we are in return statement
                parser->is_in_return = true;

                // Current token is  'return'
                result = analyze_bottom_up(parser);
                switch (result)  {
                    case EXIT_EMPTY_EXPR: 
                        break;
                    case EXIT_OK:
                        parser->curr_ret_val_count += 1;
                        SEMANTIC_ACTION(check_ret_val_count, parser);
                        SEMANTIC_ACTION(check_ret_val_type, parser); 
                        break;
                    default:
                        return result;
                } // switch

                // Currently we either parsed expression, or the expression was
                // empty, in both cases we continue further - unlike with if and while

                // <expr_list>
                result = expr_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

                // Beware - returning less values is not an error
                SEMANTIC_ACTION(check_ret_val_count, parser);

                // Reset curr return value count and aux. variable
                parser->curr_ret_val_count = 0;
                parser->is_in_return = false;
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) { 
        
        // RULE 35: <stat> → 'id' <id_n> '=' 'expr' <expr_list>

        // Beware - nondeterminism is possible here! - solved adhoc 
        //          <stat> → 'id' <id_n> '=' 'id' '(' term_list ')'
        //          <stat> → 'id' '(' term_list ')' 

        strcpy(id_name, TOKEN_REPR);
        if ((parser->curr_item = symtable_search(SYMTAB_G, TOKEN_REPR)) != NULL) {
            // Current ID is function id
            if ((FUNC_ITEM != NULL) && (FUNC_ITEM->declared)) {
                result = func_call(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

				// generate_pop_stack_to_var(id_name, parser->curr_func->key, parser->curr_block_depth);

                PARSER_EAT();
                return EXIT_OK;
            }
        }
        
        // Current ID is variable id
        SEMANTIC_ACTION(check_undeclared_var_or_func, parser, false);
         
        // Create doubly linked list to store list of IDs 
        DLL_Init(&parser->list);
        DLL_InsertLast(&parser->list, parser->curr_item);
        DLL_Last(&parser->list);

        // <id_n> 
	    strcpy(id_name, TOKEN_REPR);
        PARSER_EAT();
        result = id_n(parser);
        CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
        
        /* '=' */
        CHECK_TOKEN_TYPE(TOKEN_ASSIGN);
        
        // *ATTENTION* - nondeterminism handling - func id vs var id
        result = analyze_bottom_up(parser);

        switch (result) 
        { 
            case EXIT_OK:
                generate_pop_stack_to_var(
                        id_name, 
                        parser->curr_func->key, 
                        parser->array_depth, 
                        parser->curr_block_depth);

                DLL_First(&(parser->list)); 
                DLL_GetValue(&(parser->list), &(parser->curr_item));
                /*** Check type compatibility between left and right hand side ***/
                SEMANTIC_ACTION(check_expr_type_compat, parser, parser->curr_item->const_var->type);
                parser->curr_item->const_var->defined = true;
                DLL_Next(&(parser->list));
                
                result = expr_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);

                DLL_Dispose(&parser->list);
                return EXIT_OK;

            case EXIT_FUNC_ID:
                
                // Semantic check handled by func_call()
                result = func_call(parser);
		generate_pop_stack_to_var(
                        id_name, 
                        parser->curr_func->key, 
                        parser->array_depth, 
                        parser->curr_block_depth);

                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                PARSER_EAT();
                DLL_Dispose(&parser->list);
                return EXIT_OK;

            case EXIT_ID_BEFORE:
                
                if (TOKEN_T == TOKEN_L_PAR) {  // Potential calling of undefined/undeclared function 
                    
                    // <arg>
                    PARSER_EAT();
                    result = arg(parser);
                    CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
                    
                    //////////////////////////////////////////////////////////////////////////////////////////
                    // we dont need to eat, ')' is current token
                    CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
                    error_message("Parser", ERR_SEMANTIC_DEF, "called function not previously declared nor defined");
                    DLL_Dispose(&parser->list);
                    return ERR_SEMANTIC_DEF; 
                }
                DLL_Dispose(&parser->list);
                error_message("Parser", ERR_SYNTAX, "expression parsing failed");
                return ERR_SYNTAX; // missing or invalid  expression

            default: // other errors
                DLL_Dispose(&parser->list);
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

            if (TOKEN_KW_T == KEYWORD_ELSE) { // Current token is keyword 'else'

                // RULE 39: <else> → 'else' <stat_list>

                // Keep track of current block info
                parser->curr_block_id += 1;
                /*
                if (parser->curr_block_id == 1 && parser->curr_block_depth == 0) {
                    parser->curr_block_id = parser->block_temp_id + 1;
                } else {
                }*/
				
                generate_label_else(parser->curr_func->key, parser->array_depth, parser->curr_block_depth);

                // <stat_list> 
                PARSER_EAT();
                result = stat_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 

                return EXIT_OK;
            
            } else if (TOKEN_KW_T == KEYWORD_END) {
                
                // RULE 40: <else> → ε
			    generate_label_else(parser->curr_func->key, parser->array_depth, parser->curr_block_depth);

                return EXIT_OK;
            }
            break; 

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
	generate_pop_stack_to_var(
                        id_name, 
                        parser->curr_func->key, 
                        parser->array_depth, 
                        parser->curr_block_depth);

        switch (result) 
        {
            case EXIT_OK:
                /*** Check type compatibility between left and right hand side ***/
                SEMANTIC_ACTION(check_expr_type_compat, parser, parser->curr_item->const_var->type);
		/*
       			generate_pop_stack_to_var(
                        id_name, 
                        parser->curr_func->key, 
                        parser->array_depth, 
                        parser->curr_block_depth);*/

                result = expr_list(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                parser->curr_item->const_var->defined = true;
                return EXIT_OK;

            case EXIT_FUNC_ID:
                // Semantic check handled by func_call()
                result = func_call(parser);
                CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);
                parser->curr_item->const_var->defined = true;
                PARSER_EAT();
                return EXIT_OK;

            case EXIT_ID_BEFORE:

                if (TOKEN_T == TOKEN_L_PAR) { // Potential calling of undefined/undeclared function 
                    
                    // <arg>
                    PARSER_EAT();
                    result = arg(parser);
                    CHECK_RESULT_VALUE_SILENT(result, EXIT_OK);  
                    // we dont need to eat, ')' is current token
                    CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
                    //////////////////////////////////////////////////////////////////////////////////////////////
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
            SEMANTIC_ACTION(check_undeclared_var_or_func, parser, false);

            DLL_InsertLast(&parser->list, parser->curr_item);
            DLL_Last(&parser->list);

            PARSER_EAT(); // TODO: has to be here
            return id_n(parser);

        case TOKEN_ASSIGN:

            // RULE 44: <id_n> → ε

            return EXIT_OK;

        case TOKEN_L_PAR: // TODO: adhoc - not in grammar

            // Check if current statement is function call of undefined  function or syntax error
    
            // <arg>
            PARSER_EAT();
            result = arg(parser);
            CHECK_RESULT_VALUE_SILENT(result, EXIT_OK); 
            // we dont need to eat, ')' is current token
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
            //////////////////////////////////////////////////////////////////////////////////////////////
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
        switch (result)  {
            case EXIT_EMPTY_EXPR: 
                break;
            case EXIT_OK:
                if (parser->is_in_return) {
                    parser->curr_ret_val_count += 1; 
                    SEMANTIC_ACTION(check_ret_val_count, parser);
                    SEMANTIC_ACTION(check_ret_val_type, parser); 
                } else {
                    DLL_GetValue(&(parser->list), &(parser->curr_item));
                    /*** Check type compatibility between left and right hand side ***/
                    SEMANTIC_ACTION(check_expr_type_compat, parser, parser->curr_item->const_var->type);
                    parser->curr_item->const_var->defined = true;
                    DLL_Next(&(parser->list));
                }
                break;
            default:
                return result;
        } // switch

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
