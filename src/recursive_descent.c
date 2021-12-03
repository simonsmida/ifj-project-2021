#include <stdbool.h>
#include <string.h>

#include "include/recursive_descent.h"
#include "include/bottom_up_sa.h"
#include "include/error.h"
#include "include/scanner.h"
#include "include/parser.h"



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

/**
 * Starting nonterminal <prog>
 * @param parser pointer to the parser structure
 */
int prog(parser_t *parser)
{
    int result;
    
    // Define built-in functions
    if ((result = define_every_builtin_function(parser)) != EXIT_OK) {
        error_message("FATAL", ERR_INTERNAL, "failed to load built-in functions");
        return result;
    }

    // Rule 1: <prog> → <prolog> <seq> 'EOF'
    
    // <prolog>
    result = prolog(parser); 
    CHECK_RESULT_VALUE_SILENT(EXIT_OK);

    // <seq>
    result = seq(parser);
    CHECK_RESULT_VALUE_SILENT(EXIT_OK);

    // Check End Of File
    CHECK_TOKEN_TYPE(TOKEN_EOF);

    // TODO: generate instruction for program end
    
    return EXIT_OK; 
}


/**
 * @brief Nonterminal <prolog>
 * @param parser pointer to the parser structure
 */
int prolog(parser_t *parser)
{
    switch (TOKEN_T) 
    {
        case TOKEN_KEYWORD:

            // Expected keyword is 'require' 
            CHECK_KEYWORD(KEYWORD_REQUIRE);
            
            // Rule 2: <prolog> → 'require' 'ifj21'
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_STR_LIT);

            // Check if scanner correctly set attribute's string value
            if (parser->token->attribute->string == NULL) {
                return ERR_LEX;
            }
            
            // Check if next token is precisely "ifj21"
            if (strcmp(TOKEN_REPR, "ifj21")) {
                error_message("Parser", ERR_SYNTAX, "expected: \"%s\", is: \"%s\"", "ifj21", TOKEN_REPR);
                return ERR_SYNTAX;
            }
            
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
 */
int seq(parser_t *parser) 
{
    int result;

    switch (TOKEN_T) 
    {
        case TOKEN_KEYWORD: 

            if (TOKEN_KW_T == KEYWORD_GLOBAL) {

                // Rule 3: <seq> → <func_dec> <seq>

                // <func_dec>
                result = func_dec(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK);
                
                // <seq> calls itself
                return seq(parser); 
            
            } else if (TOKEN_KW_T == KEYWORD_FUNCTION) {
                
                // Rule 4: <seq> → <func_def> <seq>
                
                // <func_def>
                result = func_def(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK);
                
                // <seq> calls itself
                PARSER_EAT();
                return seq(parser); 
            }
            break; // Must end up in error

        case TOKEN_ID:

            // Rule 5: <seq> → <func_call> <seq>
            
            // Nondeterminism is not present - if not func_call, its syntax error
            // <func_call> we dont want to eat, 'id' is current token
            result = func_call(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK);

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
               
                HANDLE_SYMTABLE_FUNCTION();
                
                /** SEMANTIC ACTION - function redeclaration **/
                if (FUNC_ITEM->declared) { 
                    /* Function redeclaration */
                    error_message("Parser", ERR_SEMANTIC_DEF, "redeclaration of function '%s'", TOKEN_REPR);
                    return ERR_SEMANTIC_DEF;
                }

                 
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
                CHECK_RESULT_VALUE_SILENT(EXIT_OK);
                
                // Already eaten - just check validity 
                CHECK_TOKEN_TYPE(TOKEN_R_PAR); /* ')' */

                // <ret_type_list>
                PARSER_EAT();
                result = ret_type_list(parser); 
                CHECK_RESULT_VALUE_SILENT(EXIT_OK);
                
                // If everything went well function is now declared
                // TODO: check if parser->curr_item was not changed
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
 */
int func_def(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_KEYWORD) {
            
        // Expected keyword is 'function' 
        CHECK_KEYWORD(KEYWORD_FUNCTION);

        // RULE 8: <func_def> → <func_head> <stat_list> 'end'
        
        // <func_head> 
        result = func_head(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        // <stat_list>
        result = stat_list(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        // No need too eat - current token is 'end'
        CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
        CHECK_KEYWORD(KEYWORD_END);  // 'end'
        
        return EXIT_OK; 
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <func_call>
 * @param parser pointer to the parser structure
 */
int func_call(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_ID) {

        // RULE 9: <func_call> → 'id' '(' <arg> ')'
        
        /** SEMANTIC ACTION - check whether called function was prev. defined/declared **/
        if ((parser->curr_item = symtable_search(SYMTAB_G, TOKEN_REPR)) != NULL) {
            // Check declaration and definition
            if (!(FUNC_ITEM->declared)) {
                error_message("Parser", ERR_SEMANTIC_DEF, "called function '%s' "
                "was not previously declared", TOKEN_REPR);
                return ERR_SEMANTIC_DEF;
            }
        } else { // Function ID not found
            error_message("Parser", ERR_SEMANTIC_DEF, "called function '%s' "
            "was not previously declared nor defined", TOKEN_REPR);
            return ERR_SEMANTIC_DEF;
        }
        
        PARSER_EAT(); /* '(' */
        CHECK_TOKEN_TYPE(TOKEN_L_PAR); 
        
        // <arg>
        PARSER_EAT();
        result = arg(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
        
        // we dont need to eat, ')' is current token
        CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
        
        /** SEMANTIC ACTION - check function call argument count **/ 
        if (FUNC_ITEM->num_params != parser->curr_arg_count) {
            error_message("Parser", ERR_SEMANTIC_PROG, "invalid number of arguments");
            return ERR_SEMANTIC_PROG;
        }

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
 */
int arg(parser_t *parser)
{
    int result;
    if (TOKEN_T == TOKEN_ID) {
        
        // RULE 10: <arg> → <term> <arg_n>
        
        // TODO: LOCAL SYMTAB
        // Check if this variable was previously defined
        // Check if its name does not conflict with function

        // <term>
        result = term(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        parser->curr_arg_count += 1;
        
        // <arg_n>
        PARSER_EAT(); 
        result = arg_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        return EXIT_OK;

    } else if (IS_LITERAL(TOKEN_T) || IS_NIL(TOKEN_T)) {
        
        // RULE 10: <arg> → <term> <arg_n>
        
        // <term>
        result = term(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);

        // <arg_n>
        PARSER_EAT(); 
        result = arg_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
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
 */
int arg_n(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_COMMA) {
    
        // RULE 15: <arg> → ',' <term> <arg_n>
    
        // <term>
        PARSER_EAT(); 
        result = term(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        // <arg_n> calls itself
        PARSER_EAT();
        return arg_n(parser);

    } else if (TOKEN_T == TOKEN_R_PAR) {
    
        // RULE 16: <arg> → ε

        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <term>
 * @param parser pointer to the parser structure
 */
int term(parser_t *parser)
{
    if (TOKEN_T == TOKEN_ID) {
    
        // RULE 12: <term> → 'id'
        // TODO: local symtab - check arg type semantics
        
        parser->curr_arg_count += 1;

        return EXIT_OK;

    } else if (IS_LITERAL(TOKEN_T) || IS_NIL(TOKEN_T)) {
        
        // RULE 13: <term> → 'literal' ... 'literal' = str_lit|int_lit|num_lit
        // RULE 14: <term> → 'nil'

        if (FUNC_ITEM->type_params[parser->curr_arg_count] != dtype_token(parser)) {
            error_message("Parser", ERR_SEMANTIC_PROG, "invalid argument type");
            return ERR_SEMANTIC_PROG;
        }
        parser->curr_arg_count += 1;
        
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <func_head>
 * @param parser pointer to the parser structure
 */
int func_head(parser_t *parser) 
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_KEYWORD:
            // Expected keyword is 'function' 
            CHECK_KEYWORD(KEYWORD_FUNCTION);
            
            // RULE 9: <func_head> → 'function' 'id' '(' <param_fdef> ')' <ret_type_list>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
            
            HANDLE_SYMTABLE_FUNCTION();

            /** SEMANTIC ACTION - function redefinition **/
            if (FUNC_ITEM->defined) { 
                /* Function redefinition */
                error_message("Parser", ERR_SEMANTIC_DEF, "redefinition of function '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF;
            }



            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_L_PAR); // '('
            
            // <param_fdef>
            PARSER_EAT();
            result = param_fdef(parser); 
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 

            // TODO: PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); // ')'
            
            // <ret_type_list>
            PARSER_EAT();
            result = ret_type_list(parser); 
            CHECK_RESULT_VALUE_SILENT(EXIT_OK);
             
            // If everyting went well, function is now declared and defined
            FUNC_ITEM->declared = true;
            FUNC_ITEM->defined = true;

            return EXIT_OK;

        default: break; 
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <param_fdef>
 * @param parser pointer to the parser structure
 */
int param_fdef(parser_t *parser)
{
    int result;
    
    switch (TOKEN_T)
    {
        case TOKEN_ID:

            // RULE 10: <param_fdef> → 'id' ':' <dtype> <param_fdef_n>
            
            /** SEMANTIC ACTION - check invalid variable name **/
            if (symtable_search(SYMTAB_G, TOKEN_REPR)) {
                error_message("Parser", ERR_SEMANTIC_DEF, "invalid variable name '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
            }

            // TODO: add param ID into the symtable
            
            PARSER_EAT(); /* : */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            if ((parser->curr_item != NULL) && (FUNC_ITEM->declared && !(FUNC_ITEM->defined))) {
                if (FUNC_ITEM->type_params[0] != dtype_keyword(TOKEN_KW_T)) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "function param type mismatch");
                    return ERR_SEMANTIC_PROG;
                }
            } else if ((parser->curr_item != NULL) && !(FUNC_ITEM->declared)) {
                // Insert param into symtable(s) only if not already defined
                if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
            }

            // <param_fdef_n>
            PARSER_EAT();
            result = param_fdef_n(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            return EXIT_OK;

        case TOKEN_R_PAR:
            
            // RULE 11: <param_fdef> → ε
            
            /** SEMANTIC ACTION**/ 
            if ((parser->curr_item != NULL) && (FUNC_ITEM->num_params != 0)) {
                error_message("Parser", ERR_SEMANTIC_PROG, "function param count mismatch");
                return ERR_SEMANTIC_PROG;
            }
            
            return EXIT_OK;

        default: break;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <param_fdef_n>
 * @param parser pointer to the parser structure
 */
int param_fdef_n(parser_t *parser)
{
    int result;
    static int param_index = 1;

    switch (TOKEN_T)
    {
        case TOKEN_COMMA:

            // RULE 19: <param_fdef_n> → ',' 'id' ':' <dtype> <param_fdef_n>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
            
            /** SEMANTIC ACTION - check invalid variable name **/
            if (symtable_search(SYMTAB_G, TOKEN_REPR)) {
                error_message("Parser", ERR_SEMANTIC_DEF, "invalid variable name '%s'", TOKEN_REPR);
                return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
            }
            
            PARSER_EAT(); /* ':' */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            /** SEMANTIC ACTION - check function's param validity **/
            // If function is already declared check corresponding parameters
            if ((parser->curr_item != NULL) && (FUNC_ITEM->declared && !(FUNC_ITEM->defined))) {
                
                if (FUNC_ITEM->num_params-1 < param_index) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "param count mismatch");
                    return ERR_SEMANTIC_PROG;                
                }
                
                // Check current parameter's data type
                if (FUNC_ITEM->type_params[param_index] != dtype_keyword(TOKEN_KW_T)) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "function param type mismatch");
                    return ERR_SEMANTIC_PROG;
                }

            } else if ((parser->curr_item != NULL) && !(FUNC_ITEM->declared)) {
                // Insert param into symtable(s) only if not already defined
                if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
            }
             
            param_index++;

            PARSER_EAT();
            return param_fdef_n(parser); // calls itself

        case TOKEN_R_PAR:
            
            // RULE 20: <param_fdef_n> → ε

            /** SEMANTIC ACTION - check if function declaration has more params **/
            if (FUNC_ITEM->num_params > param_index) {
                error_message("Parser", ERR_SEMANTIC_PROG, "param count mismatch");
                return ERR_SEMANTIC_PROG;
            
            }
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
 */
int param_fdec(parser_t *parser)
{
    int result;
    
    switch (TOKEN_T)
    {
        case TOKEN_KEYWORD:

            if (IS_DTYPE(TOKEN_KW_T)) {

                // RULE 21: <param_fdec> → <dtype> <param_fdec_n>

                // <dtype>
                result = dtype(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                
                // Insert into symtable(s)
                if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
                symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  

                // <param_fdec_n>
                PARSER_EAT();
                result = param_fdec_n(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                
                return EXIT_OK;
            }
            break; // TODO: beware, must end up in error

        case TOKEN_R_PAR:
            
            // RULE 22: <param_fdec> → ε
            
            return EXIT_OK;

        default: break;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <param_fdec_n>
 * @param parser pointer to the parser structure
 */
int param_fdec_n(parser_t *parser)
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_COMMA:

            // RULE 23: <param_fdec_n> → ',' <dtype> <param_fdec_n>
            
            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); // TODO: check me 
            
            // Insert into symtable(s)
            if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
            symtable_insert_new_function_param(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
            
            PARSER_EAT();
            return param_fdec_n(parser); // calls itself

        case TOKEN_R_PAR:
            
            // RULE 24: <param_fdec_n> → ε
            return EXIT_OK;
        
        default: break;
    } // switch()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <ret_type_list>
 * @param parser pointer to the parser structure
 */
int ret_type_list(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_COLON) {
        
        // RULE 25: <ret_type_list> → ':' <dtype> <ret_type_list_n>
        
        /* ':' */ 
        CHECK_TOKEN_TYPE(TOKEN_COLON);

        // <dtype>
        PARSER_EAT();
        result = dtype(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
        
        // If function was previously declared check validity of return value types
        if ((parser->curr_item != NULL) && (FUNC_ITEM->declared && !(FUNC_ITEM->defined))) {
            if (FUNC_ITEM->ret_types[0] != dtype_keyword(TOKEN_KW_T)) {
                error_message("Parser", ERR_SEMANTIC_PROG, "function return type mismatch");
                return ERR_SEMANTIC_PROG;
            }
        } else if ((parser->curr_item != NULL) && !(FUNC_ITEM->declared)) {
            // Insert param into symtable(s) only if not already defined
            if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
            symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
        }
        
        // <ret_type_list_n>
        PARSER_EAT();
        result = ret_type_list_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);  // TODO: check me
        
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

                /** SEMANTIC ACTION**/ 
                if ((parser->curr_item != NULL) && (FUNC_ITEM->num_ret_types != 0)) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "function return type count mismatch");
                    return ERR_SEMANTIC_PROG;
                }
                
                // TODO: PARSER_EAT();
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) {
        
        // RULE 26: <ret_type_list> → ε
        
        /** SEMANTIC ACTION**/ 
        if ((parser->curr_item != NULL) && (FUNC_ITEM->num_ret_types != 0)) {
            error_message("Parser", ERR_SEMANTIC_PROG, "function return type count mismatch");
            return ERR_SEMANTIC_PROG;
        }
        
        // TODO: add to symtable
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_EOF) {
        
        // RULE 26: <ret_type_list> → ε
        
        /** SEMANTIC ACTION**/ 
        if ((parser->curr_item != NULL) && (FUNC_ITEM->num_ret_types != 0)) {
            error_message("Parser", ERR_SEMANTIC_PROG, "function return type count mismatch");
            return ERR_SEMANTIC_PROG;
        }
        
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <ret_type_list_n>
 * @param parser pointer to the parser structure
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
                
                // RULE 28: <ret_type_list_n> → ε
                
                /** SEMANTIC ACTION - check if function declaration has more params **/
                if (FUNC_ITEM->num_ret_types > ret_type_index) {
                    error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
                    return ERR_SEMANTIC_PROG;
                
                }
                ret_type_index = 1; 
                
                // TODO: PARSER_EAT();
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_COMMA) {

        // RULE 27: <ret_type_list_n> → ',' <dtype> <ret_type_list_n>
            
        // <dtype>
        PARSER_EAT();
        result = dtype(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
        
        /** SEMANTIC ACTION - check function's return values validity **/
        if ((parser->curr_item != NULL) && (FUNC_ITEM->declared && !(FUNC_ITEM->defined))) {
            
            if (FUNC_ITEM->num_ret_types-1 < ret_type_index) {
                error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
                return ERR_SEMANTIC_PROG;                
            }
            
            // Check current parameter's data type
            if (FUNC_ITEM->ret_types[ret_type_index] != dtype_keyword(TOKEN_KW_T)) {
                error_message("Parser", ERR_SEMANTIC_PROG, "function return type mismatch");
                return ERR_SEMANTIC_PROG;
            }

        } else if ((parser->curr_item != NULL) && !(FUNC_ITEM->declared)) {
            // Insert param into symtable(s) only if not already defined
            if (parser->curr_item == NULL) return ERR_INTERNAL; // TODO: FIX THIS
            symtable_insert_new_function_ret_type(SYMTAB_G, dtype_keyword(TOKEN_KW_T), parser->curr_item->key);  
        }
         
        ret_type_index++;
        
        PARSER_EAT();
        return ret_type_list_n(parser); // calls itself          
    
    } else if (TOKEN_T == TOKEN_ID) { // TODO: check table, maybe bs
        
        /** SEMANTIC ACTION - check if function declaration has more params **/
        if (FUNC_ITEM->num_ret_types > ret_type_index) {
            error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
            return ERR_SEMANTIC_PROG;
        
        }
        ret_type_index = 1; 
        
        // RULE 28: <ret_type_list_n> → ε
        // TODO:  add ret ID into the symtable 
        // - dont forget to also add return types
        
        return EXIT_OK;

    } else if (TOKEN_T == TOKEN_EOF) {
        
        // RULE 28: <ret_type_list_n> → ε
        
        /** SEMANTIC ACTION - check if function declaration has more params **/
        if (FUNC_ITEM->num_ret_types > ret_type_index) {
            error_message("Parser", ERR_SEMANTIC_PROG, "return type count mismatch");
            return ERR_SEMANTIC_PROG;
        
        }
        ret_type_index = 1; 
        
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <stat_list>
 * @param parser pointer to the parser structure
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

                // RULE 29: <stat_list> → <stat> <stat_list>
                
                // <stat>
                result = stat(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                
                return stat_list(parser); // calls itself  

            case KEYWORD_END:
            case KEYWORD_ELSE:
                
                // RULE 30: <stat_list> → ε
                
                return EXIT_OK;

            default:break;    
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) {
        // TODO: add to symtab
        
        // RULE 29: <stat_list> → <stat> <stat_list>
        
        // <stat>
        result = stat(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                
        return stat_list(parser); // calls itself  
    } 
    
    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);    
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <stat>
 * @param parser pointer to the parser structure
 */
int stat(parser_t *parser)
{
    int result;

    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T) 
        {
            case KEYWORD_LOCAL:
                // TODO: ret values of function
                // RULE 31: <stat> → 'local' 'id' ':' <dtype> <var_def>

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
                // TODO symtable
                
                /** SEMANTIC ACTION - check invalid variable name **/
                if (symtable_search(SYMTAB_G, TOKEN_REPR)) {
                    error_message("Parser", ERR_SEMANTIC_DEF, "invalid variable name '%s'", TOKEN_REPR);
                    return ERR_SEMANTIC_DEF; // TODO: check this, chyba 3?
                }

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_COLON); // ':'
                
                // <dtype>
                PARSER_EAT();
                result = dtype(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 

                // <var_def>
                PARSER_EAT();
                result = var_def(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                
                return EXIT_OK;
            
            case KEYWORD_IF:
                
                // RULE 32: <stat> → 'if' 'expr' 'then' <stat_list> <else> 'end'
                
                // Current token should be 'if'
                result = analyze_bottom_up(parser);
                if ((result == EXIT_EMPTY_EXPR) || (result == ERR_SYNTAX)) {
                    error_message("Parser", ERR_SYNTAX, "expression parsing failed");
                    return ERR_SYNTAX; // missing or invalid  expression
                }
                
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); // 'then'
                CHECK_KEYWORD(KEYWORD_THEN);

                // <stat_list>
                PARSER_EAT();
                result = stat_list(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                // <else> - do not eat, we need this token
                result = else_nt(parser); 
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                // Do not eat, 'end' is current token
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD);
                CHECK_KEYWORD(KEYWORD_END);
                
                PARSER_EAT(); // to get next statement 
                return EXIT_OK;
            
            case KEYWORD_WHILE:
                
                // RULE 33: <stat> → 'while' 'expr' 'do' <stat_list> 'end'
                
                // Current token should be 'while'
                result = analyze_bottom_up(parser);
                if ((result == EXIT_EMPTY_EXPR) || (result == ERR_SYNTAX)) {
                    error_message("Parser", ERR_SYNTAX, "expression parsing failed");
                    return ERR_SYNTAX; // missing or invalid  expression
                }

                /* 'do' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
                CHECK_KEYWORD(KEYWORD_DO);

                // <stat_list>
                PARSER_EAT(); 
                result = stat_list(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 

                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); // 'end'
                CHECK_KEYWORD(KEYWORD_END);

                PARSER_EAT(); // to get next statement 
                return EXIT_OK;

            case KEYWORD_RETURN:
                
                // RULE 35: <stat> → 'return' 'expr' <expr_list>

                // Current token should be 'return'
                // TODO: semantic error handling
                if ((result = analyze_bottom_up(parser)) == ERR_SYNTAX) {
                    error_message("Parser", ERR_SYNTAX, "expression parsing failed");
                    return ERR_SYNTAX;
                }

                // Currently we either parser expression, or the expression was
                // empty, in either case we continue further - unlike with if
                // and while

                // Switch context to precedence analysis
                // If no expression is present, returns current token back

                // <expr_list>
                result = expr_list(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ID) {

        // Beware - nondeterminism is possible here! - solved adhoc 
        // RULE 34: <stat> → 'id' <id_n> '=' 'expr' <expr_list>
        //          <stat> → 'id' <id_n> '=' 'id' '(' term_list ')'
        //          <stat> → 'id' '(' term_list ')' 
         
        if ((parser->curr_item = symtable_search(SYMTAB_G, TOKEN_REPR)) != NULL) {
            // Current ID is function id
            if (FUNC_ITEM != NULL) {
                result = func_call(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                PARSER_EAT();
                return EXIT_OK;
            }
        }
        
        // Current id is variable
        // TODO: symtab

        // <id_n> 
        PARSER_EAT();
        result = id_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
        
        /* '=' */
        CHECK_TOKEN_TYPE(TOKEN_ASSIGN);
        
        // *ATTENTION* - nondeterminism handling - func id vs var id
        result = analyze_bottom_up(parser);
        if (result == EXIT_FUNC_ID) {
            // TODO: - add to symtable + check semantics
            result = func_call(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK);
            PARSER_EAT();
            return EXIT_OK;

        } else if (result == EXIT_OK) {
        
            result = expr_list(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK);
            return EXIT_OK;
        
        } else if ((result == EXIT_ID_BEFORE) && (TOKEN_T == TOKEN_L_PAR)) {
            
            // Potential calling of undefined/undeclared function 
            
            // <arg>
            PARSER_EAT();
            result = arg(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            // we dont need to eat, ')' is current token
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
            error_message("Parser", ERR_SEMANTIC_DEF, "called function "
            "was not previously declared nor defined"); // TODO: ak chces id fcie, povedz PA
            return ERR_SEMANTIC_DEF;

        } else {
            error_message("Parser", ERR_SYNTAX, "expression parsing failed");
            return ERR_SYNTAX; // missing or invalid  expression
        }
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

/**
 * @brief Nonterminal <else>
 * @param parser pointer to the parser structure
 */
int else_nt(parser_t *parser)
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_KEYWORD:
            if (TOKEN_KW_T == KEYWORD_ELSE) {

                // RULE 38: <else> → 'else' <stat_list>
                // Current token is keyword 'else'
                
                // <stat_list> 
                PARSER_EAT();
                result = stat_list(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 

                return EXIT_OK;
            
            } else if (TOKEN_KW_T == KEYWORD_END) {
                
                // RULE 39: <else> → ε

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
 */
int var_def(parser_t *parser)
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
                
                // RULE 41: <var_def> → ε

                return EXIT_OK;

            default: break;
        } // switch()

    } else if (TOKEN_T == TOKEN_ASSIGN) {

        // RULE 40: <var_def> → '=' 'expr'
        
        // Current token is '='
        // *ATTENTION* - nondeterminism handling - func id vs var id
        result = analyze_bottom_up(parser);
        if (result == EXIT_FUNC_ID) {
            // TODO: - add to symtable + check semantics
            result = func_call(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK);
            PARSER_EAT();
            return EXIT_OK;

        } else if (result == EXIT_OK) {
        
            result = expr_list(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK);
            return EXIT_OK;

        } else if ((result == EXIT_ID_BEFORE) && (TOKEN_T == TOKEN_L_PAR)) {
            // Potential calling of undefined/undeclared function 
            
            // <arg>
            PARSER_EAT();
            result = arg(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            // we dont need to eat, ')' is current token
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
            error_message("Parser", ERR_SEMANTIC_DEF, "called function "
            "was not previously declared nor defined"); // TODO: ak chces id fcie, povedz PA
            return ERR_SEMANTIC_DEF;

        } else {
            error_message("Parser", ERR_SYNTAX, "expression parsing failed");
            return ERR_SYNTAX; // missing or invalid  expression
        }
    } else if (TOKEN_T == TOKEN_ID) {
        
        // RULE 41: <var_def> → ε

        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <id_n>
 * @param parser pointer to the parser structure
 */
int id_n(parser_t *parser)
{
    int result;

    switch (TOKEN_T)
    {
        case TOKEN_COMMA:
            
            // RULE 42: <id_n> → ',' 'id' <id_n>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id' 
            // TODO symtab

            PARSER_EAT(); // TODO: has to be here
            return id_n(parser);

        case TOKEN_ASSIGN:

            // RULE 43: <id_n> → ε

            return EXIT_OK;

        case TOKEN_L_PAR: // TODO: adhoc - not in grammar
            // Check if current statement is function call of undefined
            // function or syntax error
            
            // <arg>
            PARSER_EAT();
            result = arg(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            // we dont need to eat, ')' is current token
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); 
            error_message("Parser", ERR_SEMANTIC_DEF, "called function "
            "was not previously declared nor defined"); // TODO: ak chces id fcie, povedz PA
            return ERR_SEMANTIC_DEF;

        default: break;
    } // switch()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <expr_list>
 * @param parser pointer to the parser structure
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

                // RULE 37: <expr_list> → ε
                
                return EXIT_OK;

            default: break;
        } // switch() 

    } else if (TOKEN_T == TOKEN_COMMA) {
            
        // RULE 36: <expr_list> → ',' 'expr' <expr_list>

        // Current token is ','
        if ((result = analyze_bottom_up(parser)) == ERR_SYNTAX) {
            return ERR_SYNTAX;
        }
        
        return expr_list(parser);
    
    } else if (TOKEN_T == TOKEN_ID) {

        // RULE 37: <expr_list> → ε
        
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <dtype>
 * @param parser pointer to the parser structure
 */
int dtype(parser_t *parser)
{
    if (TOKEN_T == TOKEN_KEYWORD) {
        switch (TOKEN_KW_T) 
        {
            case KEYWORD_NIL:     // RULE 44: <dtype> → 'nil'
            case KEYWORD_NUMBER:  // RULE 45: <dtype> → 'number'
            case KEYWORD_INTEGER: // RULE 46: <dtype> → 'integer'
            case KEYWORD_STRING:  // RULE 47: <dtype> → 'string'
                return EXIT_OK;
            default: break;
        } // switch()
    } // if()

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);
    return ERR_SYNTAX;
}

// TODO: consider idea of looking at 'expr' terminal as a NONTERMINAL - thus
// appropriate function must be called no matter what, because it is its time
