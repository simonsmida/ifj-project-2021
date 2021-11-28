#include "include/recursive_descent.h"
#include "include/error.h"
#include "include/scanner.h"
#include "include/parser.h"

#define STRING_TOKEN_T token_type_to_str(parser->token->type)
#define TOKEN_REPR parser->token->attribute->string

#define GET_TOKEN() do {                         \
    destroy_token(parser->token);                \
    parser->token = get_next_token(parser->src); \
    if (parser->token == NULL) {                 \
        error_message("Fatal", ERR_INTERNAL, "INTERNAL INTERPRET ERROR!");\
        return ERR_INTERNAL;\
    }\
} while(0)

// TODO: make me func
// TODO: error message for specific nonterminal rule function
#define CHECK_RESULT_VALUE(_value) do {                                         \
    if (result != (_value)) {                                                   \
        if (parser->token->type == TOKEN_ERROR) {                               \
            error_message("Scanner", result, "unknown token '%s'", TOKEN_REPR); \
        } else {                                                                \
            error_message("Parser", result, "compile time error");              \
        }                                                                       \
        return result;                                                          \
    }                                                                           \
} while(0)

#define CHECK_RESULT_VALUE_SILENT(_value) do {                   \
    if (result != (_value)) {                                    \
        return result;                                           \
    }                                                            \
} while(0)

#define CHECK_TOKEN_ERROR() do {                            \
    if (parser->token->type == TOKEN_ERROR) {               \
        error_message("Scanner", ERR_LEX, "unknown token '%s'", TOKEN_REPR); \
        return ERR_LEX; /* scanner handles error message */ \
    }                                                       \
} while(0)

#define CHECK_TOKEN_TYPE(_type) do {                                    \
    if (parser->token->type != (_type)) {                               \
        error_message("Parser", ERR_SYNTAX, "expected: '%s', is: '%s'", \
                      token_type_to_str(_type),                         \
                      STRING_TOKEN_T);                                  \
        return ERR_SYNTAX;                                              \
    }                                                                   \
} while(0)

#define CHECK_KEYWORD(_type) do {                                       \
    if (parser->token->attribute->keyword_type != (_type)) {            \
        error_message("Parser", ERR_SYNTAX, "expected: '%s', is: '%s'", \
                      token_type_to_str(_type),                         \
                      STRING_TOKEN_T);                                  \
        return ERR_SYNTAX;                                              \
    }                                                                   \
} while(0)

#define TOKEN_KW_TYPE parser->token->attribute->keyword_type 

#define PARSER_EAT() do { \
    GET_TOKEN();          \
    CHECK_TOKEN_ERROR();  \
} while(0)

#define IS_DTYPE(_keyword) \
    (_keyword) == KEYWORD_NIL     || \
    (_keyword) == KEYWORD_NUMBER  || \
    (_keyword) == KEYWORD_INTEGER || \
    (_keyword) == KEYWORD_STRING     \

// Starting nonterminal <prog>
int prog(parser_t *parser)
{
    int result;
    
    // Rule 1: <prog> → <prolog> <seq> 'EOF'
    
    CHECK_TOKEN_TYPE(TOKEN_KEYWORD);
    CHECK_KEYWORD(KEYWORD_REQUIRE);

    // <prolog>
    result = prolog(parser); 
    CHECK_RESULT_VALUE_SILENT(EXIT_OK);

    // <seq>
    result = seq(parser);
    CHECK_RESULT_VALUE_SILENT(EXIT_OK);

    // Check End Of File
    CHECK_TOKEN_TYPE(TOKEN_EOF);

    // TODO: generate instruction for program end
    //
    return EXIT_OK; 
}

// Nonterminal <prolog>
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
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

// Nonterminal <seq>
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
                return seq(parser); 
            }
            // Invalid keyword 
            return ERR_SYNTAX;
        
        case TOKEN_ID:

            // Rule 5: <seq> → <func_call> <seq>
            
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
    
    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", TOKEN_REPR);    
    return ERR_SYNTAX;
}



// Nonterminal <func_dec>
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
                CHECK_TOKEN_TYPE(TOKEN_ID);   // TODO 

                // Add 'id' to the global symtable
                parser->curr_item = symtable_insert(parser->global_symtable, TOKEN_REPR);
                if (parser->curr_item == NULL) {
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

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", TOKEN_REPR);    
    return ERR_SYNTAX;
}

// Nonterminal <func_def>
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
        
        PARSER_EAT();
        return EXIT_OK; 
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

// Nonterminal <func_call>
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

    return ERR_SYNTAX;
}

// Nonterminal <arg>
int arg(parser_t *parser)
{
    int result;

    if (parser->token->type == TOKEN_ID) {
        
        // RULE 10: <arg> → <val> <arg_n>
        
        // <val>
        result = val(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);

        // <arg_n>
        PARSER_EAT(); 
        result = arg_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_R_PAR) {
    
        // RULE 11: <arg> → ε

        return EXIT_OK;

    } else {
        
        // RULE 10: <arg> → <val> <arg_n>
        
        // TODO: expecting expr
        // context switch
        return EXIT_OK;
    }
}

// Nonterminal <arg_n>
int arg_n(parser_t *parser)
{
    int result;
    if (parser->token->type == TOKEN_COMMA) {
    
        // RULE 14: <arg> → ',' <val> <arg_n>
    
        // <val>
        PARSER_EAT(); 
        result = val(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);
        
        // <arg_n> calls itself
        PARSER_EAT();
        return arg_n(parser);

    } else if (parser->token->type == TOKEN_R_PAR) {
    
        // RULE 15: <arg> → ε

        return EXIT_OK;
    }

    return ERR_SYNTAX;
}

// Nonterminal <val>
int val(parser_t *parser)
{
    if (parser->token->type == TOKEN_ID) {
    
        // RULE 12: <val> → 'id'
        // TODO: symtab
        
        return EXIT_OK;
    } else {
        
        // RULE 13: <arg> → 'expr'
        
        // TODO: expected expr
        // context switch
        return EXIT_OK;
    }
}

// Nonterminal <func_head>
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
            // TODO: add to symtable and check deallocation necessity
            // Add 'id' to the global symtable
            parser->curr_item = symtable_insert(parser->global_symtable, 
                                                parser->token->attribute->string);
            if (parser->curr_item == NULL) {
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

            // TODO: PARSER_EAT();
            return EXIT_OK;

        default: break; 
    }

    return ERR_SYNTAX;
}

// <param_fdef>
int param_fdef(parser_t *parser)
{
    int result;
    
    switch (parser->token->type)
    {
        case TOKEN_ID:

            // RULE 10: <param_fdef> → 'id' ':' <dtype> <param_fdef_n>
            // TODO: add id to symtable
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_COLON); // ':'

            // <dtype>
            PARSER_EAT();
            result = dtype(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            // <param_fdef_n>
            PARSER_EAT();
            result = param_fdef_n(parser);
            CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
            
            // TODO: PARSER_EAT();

            return EXIT_OK;

        case TOKEN_R_PAR:
            
            // RULE 11: <param_fdef> → ε
            
            // TODO: PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

// Nonterminal <param_fdef_n>
int param_fdef_n(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_COMMA:

            // RULE 19: <param_fdef_n> → ',' 'id' ':' <dtype> <param_fdef_n>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
            // TODO: add to symtable
            
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

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

// Nonterminal <param_fdec>
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

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

// Nonterminal <param_fdec_n>
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

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}


// Nonterminal <ret_type_list>
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

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}

// Nonterminal <ret_type_list_n>
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
        // TODO: add to symtable 
        
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_EOF) {
        
        // RULE 28: <ret_type_list_n> → ε
        
        return EXIT_OK;
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);    
    return ERR_SYNTAX;
}


// <stat_list>
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
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                PARSER_EAT();
                return stat_list(parser); // calls itself  

            case KEYWORD_END:
            case KEYWORD_ELSE:
                
                // RULE 30: <stat_list> → ε
                
                return EXIT_OK;

            default:
                break;    
        } // switch()
    } else if (parser->token->type == TOKEN_ID) {
        // TODO: add to symtab
        
        // RULE 29: <stat_list> → <stat> <stat_list>
        
        // <stat>
        result = stat(parser);
        CHECK_RESULT_VALUE(EXIT_OK); 
                
        PARSER_EAT();
        return stat_list(parser); // calls itself  
    } 
    
    return ERR_SYNTAX;
}

// Nonterminal <stat>
int stat(parser_t *parser)
{
    int result;
    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE) 
        {
            case KEYWORD_LOCAL:

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
                // TODO: handle expression - switch context 

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); // 'then'
                CHECK_KEYWORD(KEYWORD_THEN);

                // <stat_list>
                // TODO: eat or not
                result = stat_list(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                // <else>
                PARSER_EAT();
                result = else_nt(parser); // TODO: WATCH OUT
                CHECK_RESULT_VALUE(EXIT_OK); 

                PARSER_EAT(); /* 'end' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD);
                CHECK_KEYWORD(KEYWORD_END);
        
                return EXIT_OK;
            
            case KEYWORD_WHILE:
                
                // RULE 33: <stat> → 'while' 'expr' 'do' <stat_list> 'end'
                
                // Current token should be 'while'
                // TODO: Handle expression - switch context 

                PARSER_EAT(); /* 'then' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD);
                CHECK_KEYWORD(KEYWORD_THEN);

                PARSER_EAT(); /* 'do' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
                CHECK_KEYWORD(KEYWORD_DO);

                // <stat_list> 
                result = stat_list(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 

                PARSER_EAT(); /* 'end' */
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); // 'end'
                CHECK_KEYWORD(KEYWORD_END);

                return EXIT_OK;

            case KEYWORD_RETURN:

                // RULE 35: <stat> → 'return' <expr> <expr_list>
                
                // <expr> 
                result = expr_nt(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                // <expr_list> 
                PARSER_EAT();
                result = expr_list(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                return EXIT_OK;

            default: break;
        } // switch()

    } else if (parser->token->type == TOKEN_ID) {

        // RULE 34: <stat> → 'id' <id_n> '=' 'expr' <expr_list>
        
        // TODO: SYMTABLE
        
        PARSER_EAT();
        result = id_n(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK); 
        
        /* '=' */
        CHECK_TOKEN_TYPE(TOKEN_ASSIGN);
        
        PARSER_EAT();
        // TODO: SWITCH CONTEXT
        
        // TODO: EAT OR NOT
        result = expr_list(parser);
        CHECK_RESULT_VALUE_SILENT(EXIT_OK);

        return EXIT_OK;
    }

    return ERR_SYNTAX;
}

// Nonterminal <expr_nt>
int expr_nt(parser_t *parser)
{
    if (parser->token->type == TOKEN_KEYWORD) {
        switch (TOKEN_KW_TYPE)
        {
            case KEYWORD_END:
            case KEYWORD_LOCAL:
            case KEYWORD_IF:
            case KEYWORD_WHILE:
            case KEYWORD_RETURN:
            case KEYWORD_ELSE: 

                // RULE 49: <expr> → ε
                
                return EXIT_OK;

            default: break;

        } // switch()
    } else if (parser->token->type == TOKEN_ID) {
                
        // RULE 49: <expr> → ε
        
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_COMMA) {
        
        // RULE 49: <expr> → ε
        
        return EXIT_OK;
    }
    
    // RULE 48: <expr> → 'expr'
    // TODO: context switch
    
    return EXIT_OK;
}

// Nonterminal <else>
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

    return ERR_SYNTAX;
} 

// Nonterminal <var_def>
int var_def(parser_t *parser)
{
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

        // TODO: switch context
        
        return EXIT_OK;

    } else if (parser->token->type == TOKEN_ID) {
        
        // RULE 41: <var_def> → ε

        return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// Nonterminal <id_n>
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
    }

    return ERR_SYNTAX;
}

// Nonterminal <expr_list>
int expr_list(parser_t *parser)
{
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

        // Current token should be ','
        // TODO: handle expression - switch context - sanity needed?

        PARSER_EAT();
        return expr_list(parser);
    
    } else if (parser->token->type == TOKEN_ID) {

        // RULE 37: <expr_list> → ε
        
        return EXIT_OK;
    }

    return ERR_SYNTAX;
}

// Nonterminal <dtype>
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
    }

    error_message("Parser", ERR_SYNTAX, "unexpected token '%s'", STRING_TOKEN_T);   
    return ERR_SYNTAX;
}

// TODO: consider idea of looking at 'expr' terminal as a NONTERMINAL - thus
// appropriate function must be called no matter what, because it is its time
