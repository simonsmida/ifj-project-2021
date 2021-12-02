#include "include/recursive_descent.h"
#include "include/bottom_up_sa.h"
#include "include/error.h"
#include "include/scanner.h"
#include "include/parser.h"

#define STRING_TOKEN_T token_type_to_str(parser->token->type)
#define STRING_KW_T kw_type_to_str(parser->token->attribute->keyword_type)

#define TOKEN_REPR parser->token->attribute->string
#define TOKEN_KW_TYPE parser->token->attribute->keyword_type 

#define GET_TOKEN() do {                                                   \
    destroy_token(parser->token);                                          \
    parser->token = get_next_token(parser->src);                           \
    if (parser->token == NULL) {                                           \
        error_message("Fatal", ERR_INTERNAL, "INTERNAL INTERPRET ERROR!"); \
        return ERR_INTERNAL;                                               \
    }                                                                      \
} while(0)

// TODO: make me func
// TODO: error message for specific nonterminal rule function
#define CHECK_RESULT_VALUE(_value) do {                                         \
    if (result != (_value)) {                                                   \
        if (parser->token->type == TOKEN_ERROR) {                               \
            error_message("Scanner", result, "unknown token '%s'", TOKEN_REPR); \
        } else {                                                                \
            error_message("Parser", result, "unexpected token '%s' (%s)",       \
                    TOKEN_REPR, STRING_TOKEN_T);                                \
        }                                                                       \
        return result;                                                          \
    }                                                                           \
} while(0)

#define CHECK_RESULT_VALUE_SILENT(_value) do { \
    if (result != (_value)) {                  \
        return result;                         \
    }                                          \
} while(0)

#define CHECK_TOKEN_ERROR() do {                                             \
    if (parser->token->type == TOKEN_ERROR) {                                \
        error_message("Scanner", ERR_LEX, "unknown token '%s'", TOKEN_REPR); \
        return ERR_LEX; /* scanner handles error message */                  \
    }                                                                        \
} while(0)

#define CHECK_TOKEN_TYPE(_type) do {                                    \
    if (parser->token->type != (_type)) {                               \
        error_message("Parser", ERR_SYNTAX, "expected: '%s', is: '%s'", \
                      token_type_to_str(_type), STRING_TOKEN_T);        \
        return ERR_SYNTAX;                                              \
    }                                                                   \
} while(0)

#define CHECK_KEYWORD(_type) do {                                               \
    if (parser->token->attribute->keyword_type != (_type)) {                    \
        error_message("Parser", ERR_SYNTAX, "expected keyword: '%s', is: '%s'", \
                      kw_type_to_str(_type), STRING_KW_T);                      \
        return ERR_SYNTAX;                                                      \
    }                                                                           \
} while(0)


#define PARSER_EAT() do { \
    GET_TOKEN();          \
    CHECK_TOKEN_ERROR();  \
} while(0)

#define IS_DTYPE(_keyword)           \
    (_keyword) == KEYWORD_NIL     || \
    (_keyword) == KEYWORD_NUMBER  || \
    (_keyword) == KEYWORD_INTEGER || \
    (_keyword) == KEYWORD_STRING     

#define IS_LITERAL(_token)        \
    (_token) == TOKEN_NUM_LIT  || \
    (_token) == TOKEN_INT_LIT  || \
    (_token) == TOKEN_STR_LIT    

#define IS_NIL(_token) \
    (((_token) == (TOKEN_KEYWORD)) && ((TOKEN_KW_TYPE) == (KEYWORD_NIL)))

/**
 * Starting nonterminal <prog>
 * @param parser pointer to the parser structure
 */
int prog(parser_t *parser)
{
    int result;
    
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
    switch (parser->token->type) 
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

    switch (parser->token->type) 
    {
        case TOKEN_KEYWORD: 

            if (TOKEN_KW_TYPE == KEYWORD_GLOBAL) {

                // Rule 3: <seq> → <func_dec> <seq>

                // <func_dec>
                result = func_dec(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK);
                
                // <seq> calls itself
                return seq(parser); 
            
            } else if (TOKEN_KW_TYPE == KEYWORD_FUNCTION) {
                
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

    switch (parser->token->type) 
    {
        case TOKEN_KEYWORD: // 'global' or 'function'

            if (TOKEN_KW_TYPE == KEYWORD_GLOBAL) {

                // We are inside function declaration
                parser->inside_func_dec = true;
                parser->declared_function = false;

                // RULE 7: <func_dec> → 'global' 'id' ':' 'function' '(' <param_fdec> ')' <ret_type_list>
            
                PARSER_EAT(); /* 'id' */
                CHECK_TOKEN_TYPE(TOKEN_ID);   
                
                // Create symtable item for current ID
                parser->curr_item = symtable_insert(parser->global_symtable, TOKEN_REPR);
                if (parser->curr_item == NULL) {
                    return ERR_INTERNAL;
                }

                // Insert current function ID into the symtable
                item_function_t *item;
                if (!(item = symtable_create_and_insert_function(parser->global_symtable, TOKEN_REPR))) {
                    error_message("Parser", ERR_SEMANTIC_DEF, "function '%s' not declared", TOKEN_REPR);
                    return ERR_SEMANTIC_DEF; // TODO: check internal error
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

                return EXIT_OK;

            } else if (TOKEN_KW_TYPE == KEYWORD_FUNCTION) {
                
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

    if (parser->token->type == TOKEN_KEYWORD) {
            
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

    if (parser->token->type == TOKEN_ID) {

        // RULE 9: <func_call> → 'id' '(' <arg> ')'
        
        // Check whether function was previously declared/defined
        if (!(parser->curr_item = symtable_search(parser->global_symtable, TOKEN_REPR))) {
            error_message("Parser", ERR_SEMANTIC_DEF, "function '%s' not declared", TOKEN_REPR);
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
    if (parser->token->type == TOKEN_ID) {
        
        // RULE 10: <arg> → <term> <arg_n>
        
        // TODO: LOCAL SYMTAB

        // <term>
        result = term(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);

        // <arg_n>
        PARSER_EAT(); 
        result = arg_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        return EXIT_OK;

    } else if (IS_LITERAL(parser->token->type) || IS_NIL(parser->token->type)) {
        
        // RULE 10: <arg> → <term> <arg_n>
        
        // <term>
        result = term(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);

        // <arg_n>
        PARSER_EAT(); 
        result = arg_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_R_PAR) {
    
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

    if (parser->token->type == TOKEN_COMMA) {
    
        // RULE 15: <arg> → ',' <term> <arg_n>
    
        // <term>
        PARSER_EAT(); 
        result = term(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        // <arg_n> calls itself
        PARSER_EAT();
        return arg_n(parser);

    } else if (parser->token->type == TOKEN_R_PAR) {
    
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
    if (parser->token->type == TOKEN_ID) {
    
        // RULE 12: <term> → 'id'
        // TODO: local symtab

        return EXIT_OK;

    } else if (IS_LITERAL(parser->token->type) || IS_NIL(parser->token->type)) {
        
        // RULE 13: <term> → 'literal' ... 'literal' = str_lit|int_lit|num_lit
        // RULE 14: <term> → 'nil'
        
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

    switch (parser->token->type)
    {
        case TOKEN_KEYWORD:
            // Expected keyword is 'function' 
            CHECK_KEYWORD(KEYWORD_FUNCTION);
            
            // RULE 9: <func_head> → 'function' 'id' '(' <param_fdef> ')' <ret_type_list>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
            
            // TODO: search for previous declaration or definition - if not
            // successfull or declared -> proceed, if defined -> sem. error

            // Create symtable item for current ID
            parser->curr_item = symtable_insert(parser->global_symtable, TOKEN_REPR);
            if (parser->curr_item == NULL) {
                return ERR_INTERNAL;
            }

            // Insert current function ID into the symtable
            item_function_t *item;
            if (!(item = symtable_create_and_insert_function(parser->global_symtable, TOKEN_REPR))) {
                error_message("Parser", ERR_SEMANTIC_DEF, "function '%s' not declared", TOKEN_REPR);
                return ERR_SEMANTIC_DEF; // TODO: check internal error
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
    
    switch (parser->token->type)
    {
        case TOKEN_ID:

            // RULE 10: <param_fdef> → 'id' ':' <dtype> <param_fdef_n>

            // TODO: add param ID into the symtable
            
            PARSER_EAT(); /* : */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            // <param_fdef_n>
            PARSER_EAT();
            result = param_fdef_n(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            return EXIT_OK;

        case TOKEN_R_PAR:
            
            // RULE 11: <param_fdef> → ε
            
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

    switch (parser->token->type)
    {
        case TOKEN_COMMA:

            // RULE 19: <param_fdef_n> → ',' 'id' ':' <dtype> <param_fdef_n>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
            
            // TODO: add param into the symtable
            
            PARSER_EAT(); /* ':' */
            CHECK_TOKEN_TYPE(TOKEN_COLON);

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();

            return param_fdef_n(parser); // calls itself

        case TOKEN_R_PAR:
            
            // RULE 20: <param_fdef_n> → ε
            
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
    
    switch (parser->token->type)
    {
        case TOKEN_KEYWORD:

            if (IS_DTYPE(TOKEN_KW_TYPE)) {

                // RULE 21: <param_fdec> → <dtype> <param_fdec_n>

                // <dtype>
                result = dtype(parser);
                CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
                PARSER_EAT();

                // <param_fdec_n>
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

    switch (parser->token->type)
    {
        case TOKEN_COMMA:

            // RULE 23: <param_fdec_n> → ',' <dtype> <param_fdec_n>
            
            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); // TODO: check me 
            
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

    if (parser->token->type == TOKEN_COLON) {
        
        // RULE 25: <ret_type_list> → ':' <dtype> <ret_type_list_n>
        
        /* ':' */ 
        CHECK_TOKEN_TYPE(TOKEN_COLON);

        // <dtype>
        PARSER_EAT();
        result = dtype(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
        
        // <ret_type_list_n>
        PARSER_EAT();
        result = ret_type_list_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);  // TODO: check me
        
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_KEYWORD) {
                
        switch (TOKEN_KW_TYPE) 
        {
            case KEYWORD_FUNCTION:
            case KEYWORD_RETURN:
            case KEYWORD_WHILE:
            case KEYWORD_GLOBAL:
            case KEYWORD_LOCAL:
            case KEYWORD_END:
            case KEYWORD_IF:
            
                // RULE 26: <ret_type_list> → ε
                
                // TODO: PARSER_EAT();
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (parser->token->type == TOKEN_ID) {
        
        // RULE 26: <ret_type_list> → ε
        
        // TODO: add to symtable
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_EOF) {
        
        // RULE 26: <ret_type_list> → ε
        
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
    
    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE) 
        {
            case KEYWORD_FUNCTION:
            case KEYWORD_GLOBAL:
            case KEYWORD_LOCAL:
            case KEYWORD_RETURN:
            case KEYWORD_END:
            case KEYWORD_IF:
            case KEYWORD_WHILE:
                
                // RULE 28: <ret_type_list_n> → ε
                
                // TODO: PARSER_EAT();
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (parser->token->type == TOKEN_COMMA) {

        // RULE 27: <ret_type_list_n> → ',' <dtype> <ret_type_list_n>
            
        // <dtype>
        PARSER_EAT();
        result = dtype(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
        
        PARSER_EAT();
        return ret_type_list_n(parser); // calls itself          
    
    } else if (parser->token->type == TOKEN_ID) { // TODO: check table, maybe bs
        
        // RULE 28: <ret_type_list_n> → ε
        // TODO:  add ret ID into the symtable 
        // - dont forget to also add return types
        
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_EOF) {
        
        // RULE 28: <ret_type_list_n> → ε
        
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

    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE) 
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

    } else if (parser->token->type == TOKEN_ID) {
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

    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE) 
        {
            case KEYWORD_LOCAL:
                // TODO: ret values of function
                // RULE 31: <stat> → 'local' 'id' ':' <dtype> <var_def>

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
                // TODO symtable

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_COLON); // ':'
                
                // <dtype>
                PARSER_EAT();
                result = dtype(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 

                // <var_def>
                PARSER_EAT();
                result = var_def(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 
                
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

    } else if (parser->token->type == TOKEN_ID) {

        // Beware - nondeterminism is possible here! - solved adhoc 
        // RULE 34: <stat> → 'id' <id_n> '=' 'expr' <expr_list>
        //          <stat> → 'id' <id_n> '=' 'id' '(' term_list ')'
         
		symtable_item_t *id = symtable_search(parser->global_symtable, TOKEN_REPR);
        if ((id != NULL) && (id->function != NULL)) { // <func_call>
            // TODO: - add to symtable + check semantics
            result = func_call(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            PARSER_EAT();
            return EXIT_OK;
        }

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
        }

        return ERR_SYNTAX;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s' (%s)", TOKEN_REPR, STRING_TOKEN_T);    
    return ERR_SYNTAX;
}


/**
 * @brief Nonterminal <expr_nt>
 * @param parser pointer to the parser structure
 */
int expr_nt(parser_t *parser)
{
    int result;
    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE)
        {
            case KEYWORD_END:
            case KEYWORD_LOCAL:
            case KEYWORD_IF:
            case KEYWORD_WHILE:
            case KEYWORD_RETURN:
            case KEYWORD_ELSE: 

                // RULE 50: <expr> → ε
                
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (parser->token->type == TOKEN_ID) {
                
        // RULE 50: <expr> → ε
        PARSER_EAT(); // important, to check if expr_list is not empty 
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_COMMA) {
        
        // RULE 50: <expr> → ε
        
        return EXIT_OK;
    }
    
    // RULE 49: <expr> → 'expr' ... func call not allowed here
    // Context switch - error is reported via precedence analysis
    if ((result = analyze_bottom_up(parser)) == ERR_SYNTAX) {
        return ERR_SYNTAX;
    }
    
    return EXIT_OK;
}


/**
 * @brief Nonterminal <else>
 * @param parser pointer to the parser structure
 */
int else_nt(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_KEYWORD:
            if (TOKEN_KW_TYPE == KEYWORD_ELSE) {

                // RULE 38: <else> → 'else' <stat_list>
                // Current token is keyword 'else'
                
                // <stat_list> 
                PARSER_EAT();
                result = stat_list(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 

                return EXIT_OK;
            
            } else if (TOKEN_KW_TYPE == KEYWORD_END) {
                
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
    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE) 
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

    } else if (parser->token->type == TOKEN_ASSIGN) {

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
        }
        
        return ERR_SYNTAX;

    } else if (parser->token->type == TOKEN_ID) {
        
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
    switch (parser->token->type)
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

    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE)
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

    } else if (parser->token->type == TOKEN_COMMA) {
            
        // RULE 36: <expr_list> → ',' 'expr' <expr_list>

        // Current token is ','
        if ((result = analyze_bottom_up(parser)) == ERR_SYNTAX) {
            return ERR_SYNTAX;
        }
        
        return expr_list(parser);
    
    } else if (parser->token->type == TOKEN_ID) {

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
    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE) 
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
