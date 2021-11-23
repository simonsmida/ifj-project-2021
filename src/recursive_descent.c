#include "include/recursive_descent.h"
#include "include/error.h"
#include "include/scanner.h"
#include "include/parser.h"

#define STRING_TOKEN_T token_type_to_str(parser->token->type)

#define GET_TOKEN() parser->token = get_next_token(parser->src)

// TODO: error message for specific nonterminal rule function
#define CHECK_RESULT_VALUE(_value) do {                          \
    if (result != (_value)) {                                    \
        error_message("Parser", result, "compile time error\n"); \
        return result;                                           \
    }                                                            \
} while(0)

#define CHECK_TOKEN_ERROR() do {                            \
    if (parser->token->type == TOKEN_ERROR) {               \
        return ERR_LEX; /* scanner handles error message */ \
    }                                                       \
} while(0)

#define CHECK_TOKEN_TYPE(_type) do {                                                        \
    if (parser->token->type != (_type)) {                                                   \
        error_message("Parser", ERR_SYNTAX, "unexpected token, expected: '%s', is: '%s'\n", \
            STRING_TOKEN_T, token_type_to_str(_type));                                      \
        return ERR_SYNTAX;                                                                  \
    }                                                                                       \
} while(0)

#define CHECK_KEYWORD(_type) do {                                                             \
    if (parser->token->attribute->keyword_type != (_type)) {                                  \
        error_message("Parser", ERR_SYNTAX, "unexpected keyword, expected: '%s', is: '%s'\n", \
            STRING_TOKEN_T, token_type_to_str(_type));                                        \
        return ERR_SYNTAX;                                                                    \
    }                                                                                         \
} while(0)

#define TOKEN_KW_TYPE parser->token->attribute->keyword_type 

#define PARSER_EAT() do { \
    GET_TOKEN();          \
    CHECK_TOKEN_ERROR();  \
} while(0)


// Starting nonterminal <prog>
int prog(parser_t *parser)
{
    int result;
    
    // Rule 1: <prog> → <prolog> <func_dec> <func_def> <func_call> 'EOF'
    
    switch (parser->token->type) 
    {
        case TOKEN_KEYWORD: // expects keyword 'require'

            CHECK_KEYWORD(KEYWORD_REQUIRE);

            // <prolog>
            result = prolog(parser); 
            CHECK_RESULT_VALUE(EXIT_OK);

            // <func_dec>
            result = func_dec(parser);
            CHECK_RESULT_VALUE(EXIT_OK);

            // <func_def>
            result = func_def(parser);
            CHECK_RESULT_VALUE(EXIT_OK);

            // <func_call>
            result = func_call(parser);
            CHECK_RESULT_VALUE(EXIT_OK);
            
            // Check End Of File
            CHECK_TOKEN_TYPE(TOKEN_EOF);

            // TODO: generate instruction for program end
            //
            return EXIT_OK; // TODO: break?
        default:
            break;
    } 
    
    return ERR_SYNTAX;
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
            
            // TODO: STRCMP / *a = *b?
            // Check if next token is precisely "ifj21"
            if (strcmp(parser->token->attribute->string, "ifj21")) {
                return ERR_SYNTAX;
            }
            
            PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

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
            
                // RULE 3: 
                // <func_dec> → 'global' 'id' ':' 'function' '(' <param_fdec> ')' ':' <ret_type_list> <func_dec>
            
                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_ID);       // 'id'
                
                // Add 'id' to the global symtable
                parser->curr_item = symtable_insert(parser->global_symtable, 
                                                    parser->token->attribute->string);
                if (parser->curr_item == NULL) {
                    return ERR_SEMANTIC_DEF; // TODO: check internal error
                }
                 
                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_COLON);    // ':'

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
                CHECK_KEYWORD(KEYWORD_FUNCTION);  // 'function'
                
                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_L_PAR);     // '('

                // <param_fdec>
                result = param_fdec(parser); 
                CHECK_RESULT_VALUE(EXIT_OK);

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_R_PAR);     // ')'

                PARSER_EAT();
                CHECK_TOKEN_TYPE(TOKEN_COLON);     // ':'
                
                // <ret_type_list>
                result = ret_type_list(parser); 
                CHECK_RESULT_VALUE(EXIT_OK);

                PARSER_EAT();

                return func_dec(parser); // calls itself
            } else if (TOKEN_KW_TYPE == KEYWORD_FUNCTION) {
                // RULE 4: <func_dec> → ε
                
                PARSER_EAT();
                return EXIT_OK;
            } else {
                break; // TODO: verify if error really happens 
            }
        case TOKEN_EOF: 
        // case TOKEN_EXPR: // TODO: check this
            
            // TODO: CONTROL SWITCH TO PRECEDENCE SA
            
            // RULE 4: <func_dec> → ε
            
            PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

    return ERR_SYNTAX;
}

// Nonterminal <func_def>
int func_def(parser_t *parser)
{
    int result;

    
    switch (parser->token->type)
    {
        case TOKEN_KEYWORD: // 'function'
            // Expected keyword is 'function' 
            CHECK_KEYWORD(KEYWORD_FUNCTION);

            // RULE 5: <func_def> → <func_head> <stat_list> 'end' <func_def>
            
            // <func_head> 
            result = func_head(parser);
            CHECK_RESULT_VALUE(EXIT_OK);
            
            // <stat_list>
            result = stat_list(parser);
            CHECK_RESULT_VALUE(EXIT_OK);
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_KEYWORD); 
            CHECK_KEYWORD(KEYWORD_END);  // 'end'

            PARSER_EAT();

            return func_def(parser); // calls itself
        
        case TOKEN_EOF: // TODO: beware, expr was here
            
            // RULE 6: <func_def> → ε
            
            PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

    return ERR_SYNTAX;
}

// TODO: fix me pls
int func_call(parser_t *parser) {return 1;}

#if 0
// Nonterminal <func_call>
int func_call(parser_t *parser)
{
    
    switch (parser->token->type)
    {
        case TOKEN_EXPR:
            
            // RULE 7: <func_call> → 'expr' <func_call>
            
            // TODO: CONTROL SWITCH TO PRECEDENCE SA
            
            PARSER_EAT();
            return func_call(parser);

        case TOKEN_EOF:

            // RULE 8: <func_call> → ε
            
            PARSER_EAT();
            return EXIT_OK; 
    }

    return ERR_SYNTAX;
}
#endif

// Nonterminal <func_head>
int func_head(parser_t *parser) 
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_KEYWORD:
            // Expected keyword is 'function' 
            CHECK_KEYWORD(KEYWORD_FUNCTION);
            
            // RULE 9: <func_head> → 'function' 'id' '(' <param_fdef> ')' ':' <ret_type_list>
            
            /* TODO wtf redundant bs 
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_FUNCTION); // 'function'
            */

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
            // TODO: add to symtable

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_L_PAR); // '('
            
            // <param_fdef>
            result = param_fdef(parser); 
            CHECK_RESULT_VALUE(EXIT_OK); 

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); // ')'
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_R_PAR); // ':'
            
            // <ret_type_list>
            result = ret_type_list(parser); 
            CHECK_RESULT_VALUE(EXIT_OK);

            PARSER_EAT();
            return EXIT_OK;
        default:
           break; 
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

            // RULE 10: <param_fdef> → 'id' <dtype> <param_fdef_n>
            // TODO: add id to symtable

            // <dtype>
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            // <param_fdef_n>
            result = param_fdef_n(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();

            return EXIT_OK;

        case TOKEN_R_PAR:
            
            // RULE 11: <param_fdef> → ε
            
            PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

    return ERR_SYNTAX;
}

// Nonterminal <param_fdef_n>
int param_fdef_n(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_COMMA:

            // RULE 12: <param_fdef_n> → ',' 'id' ':' <dtype> <param_fdef_n>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'
            // TODO: add to symtable
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_COLON); // ':'

            // <dtype>
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();

            return param_fdef_n(parser); // calls itself

        case TOKEN_R_PAR:
            
            // RULE 13: <param_fdef_n> → ε

            PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

    return ERR_SYNTAX;
}

// Nonterminal <param_fdec>
int param_fdec(parser_t *parser)
{
    int result;
    
    switch (parser->token->type)
    {
        case TOKEN_KEYWORD:
            if (TOKEN_KW_TYPE == KEYWORD_NIL     ||
                TOKEN_KW_TYPE == KEYWORD_NUMBER  ||
                TOKEN_KW_TYPE == KEYWORD_INTEGER ||
                TOKEN_KW_TYPE == KEYWORD_STRING) {
            
                // RULE 14: <param_fdec> → <dtype> <param_fdec_n>

                // <dtype>
                result = dtype(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                // <param_fdef_n>
                result = param_fdef_n(parser);
                CHECK_RESULT_VALUE(EXIT_OK); 
                
                PARSER_EAT();
                return EXIT_OK;
            }
            break; // TODO: beware, must end up in error
        case TOKEN_R_PAR:
            
            // RULE 15: <param_fdec> → ε
            
            PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

    return ERR_SYNTAX;
}

// Nonterminal <param_fdec_n>
int param_fdec_n(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_COMMA:

            // RULE 16: <param_fdec_n> → ',' <dtype> <param_fdec_n>
            
            // <dtype>
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return param_fdec_n(parser); // calls itself

        case TOKEN_R_PAR:
            
            // RULE 17: <param_fdec_n> → ε
            PARSER_EAT();
            return EXIT_OK;
        default:
            break;
    }

    return ERR_SYNTAX;
}


#if 0
// Nonterminal <ret_type_list>
int ret_type_list(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_NIL: 
        case TOKEN_NUMBER:
        case TOKEN_INTEGER:
        case TOKEN_STRING:

            // RULE 18: <ret_type_list> → <dtype> <ret_type_list_n>
            
            // <dtype>
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            // <ret_type_list_n>
            result = ret_type_list_n(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return EXIT_OK;

        case TOKEN_EOF:
        case TOKEN_GLOBAL:
        case TOKEN_ID: // TODO: HOW TO SYMTABLE
        case TOKEN_FUNCTION:
        case TOKEN_END:
        case TOKEN_LOCAL:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_RETURN:
        case TOKEN_EXPR: // TODO: HANDLE THIS - switch context ?
            
            // RULE 19: <ret_type_list> → ε
            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}

// <ret_type_list_n>
int ret_type_list_n(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_COMMA:

            // RULE 20: <ret_type_list_n> → ',' <dtype> <ret_type_list_n>
            
            // <dtype>
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return ret_type_list_n(parser); // calls itself          

        case TOKEN_EOF:
        case TOKEN_GLOBAL:
        case TOKEN_ID: // TODO: HOW TO SYMTABLE
        case TOKEN_FUNCTION:
        case TOKEN_END:
        case TOKEN_LOCAL:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_RETURN:
        case TOKEN_EXPR: // TODO: HANDLE THIS - switch context ?
            
            // RULE 21: <ret_type_list_n> → ε
            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// <stat_list>
int stat_list(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_ID: // TODO: HANDLE THIS SYMTAB
        case TOKEN_LOCAL: // TODO: MAYBE KEYWORD?
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_RETURN:

            // RULE 22: <stat_list> → <stat> <stat_list>
            
            // <stat>
            result = stat(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return stat_list(parser); // calls itself          

        case TOKEN_END:
        case TOKEN_ELSE:

            // RULE 23: <stat_list> → ε
            
            PARSER_EAT();
            return EXIT_OK;
    }
    
    return ERR_SYNTAX;
}

// <stat>
int stat(parser_t *parser)
{
    int result;

    switch (parser->token_type) 
    {
        case TOKEN_LOCAL:

            // RULE 24: <stat> → 'local' 'id' ':' <dtype> <var_def>

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id'

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_COLON); // ':'
            
            // <dtype>
            result = dtype(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 

            // <var_def>
            result = var_def(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return EXIT_OK;
        
        case TOKEN_IF:
            
            // RULE 25: <stat> → 'if' 'expr' 'then' <stat_list> <else> 'end'
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_EXPR); // 'expr' TODO: context switch + handle

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_THEN); // 'then'

            // <stat_list>
            result = stat_list(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            // <else>
            result = else_nt(parser); // TODO: WATCH OUT
            CHECK_RESULT_VALUE(EXIT_OK); 

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_END); // 'end'
    
            PARSER_EAT();
            return EXIT_OK;
        
        case TOKEN_WHILE:
            
            // RULE 26: <stat> → 'while' 'expr' 'do' <stat_list> 'end'
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_EXPR); // 'expr' TODO: context switch + handle

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_THEN); // 'then'
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_DO); // 'do'
           
            // <do> 
            result = do_nt(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_END); // 'end'

            PARSER_EAT();
            return EXIT_OK;

        case TOKEN_ID:
            
            // RULE 27: <stat> → <assign>
            
            // TODO: SYMTABLE

            // <assign> 
            result = assign(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return EXIT_OK;

        case TOKEN_RETURN:

            // RULE 28: <stat> → 'return' <ret_expr_list>

            // <ret_expr_list> 
            result = ret_expr_list(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// <else>
int else_nt(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_ELSE:

            // RULE 29: <else> → 'else' <stat_list>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ELSE); // 'else'

            // <stat_list> 
            result = stat_list(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 

            PARSER_EAT();
            return EXIT_OK;
        
        case TOKEN_END:
            
            // RULE 30: <else> → ε

            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
} 


// <var_def>
int var_def(parser_t *parser)
{
    int result;

    switch (parser->token->type) 
    { 
        case TOKEN_ASSIGN:
            
            // RULE 31: <var_def> → '=' 'expr'

            // TODO: switch context
            
            PARSER_EAT();
            return EXIT_OK;
        
        case TOKEN_ID: // TODO
        case TOKEN_END:
        case TOKEN_LOCAL:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_RETURN:
        case TOKEN_ELSE:
            
            // RULE 32: <var_def> → ε

            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// <assign>
int assign(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_ID:

            // RULE 33: <assign> → 'id' <id_n> '=' 'expr' <expr_list>

            // <id_n> 
            result = id_n(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ASSIGN); // '='

            // TODO: SWITCH CONTEXT

            // <expr_list> 
            result = expr_list(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// <id_n>
int id_n(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_COMMA:
            
            // RULE 34: <id_n> → ',' 'id' <id_n>
            
            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_ID); // 'id' TODO

            PARSER_EAT();
            return id_n(parser);

        case TOKEN_ASSIGN:

            // RULE 35: <id_n> → ε

            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// <expr_list>
int expr_list(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_EXPR: // TODO BS, HAS TO BE SOLVED
            
            // RULE 36: <expr_list> → 'expr' <expr_list>

            PARSER_EAT();
            return  = expr_list(parser); // calls itself
        
        case TOKEN_COMMA:
            
            // RULE 37: <expr_list> → ',' 'expr' <expr_list>

            PARSER_EAT();
            CHECK_TOKEN_TYPE(TOKEN_expr); // 'EXPR' TODO

            PARSER_EAT();
            
            return expr_list(parser);

        case TOKEN_ID: // TODO
        case TOKEN_END:
        case TOKEN_LOCAL:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_RETURN:
        case TOKEN_ELSE:

            // RULE 38: <expr_list> → ε
            
            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// <dtype>
int dtype(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_NIL:
            
            // RULE 39: <dtype> → 'nil'
            PARSER_EAT();
            return EXIT_OK;

        case TOKEN_NUMBER:
            
            // RULE 40: <dtype> → 'number'
            PARSER_EAT();
            return EXIT_OK;
        
        case TOKEN_INTEGER:
            
            // RULE 41: <dtype> → 'integer'
            PARSER_EAT();
            return EXIT_OK;
            
        case TOKEN_STRING:
            
            // RULE 42: <dtype> → 'string'
            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}


// <ret_expr_list>
int ret_expr_list(parser_t *parser)
{
    int result;

    switch (parser->token->type)
    {
        case TOKEN_EXPR: // TODO
            
            // RULE 43: <ret_expr_list> → 'expr' <expr_list>
            
            // <epr_list> 
            result = expr_list(parser);
            CHECK_RESULT_VALUE(EXIT_OK); 
            
            PARSER_EAT();
            return EXIT_OK;
        
        case TOKEN_ID:
        case TOKEN_END:
        case TOKEN_LOCAL:
        case TOKEN_IF:
        case TOKEN_WHILE:
        case TOKEN_RETURN:
        case TOKEN_ELSE:
            
            // RULE 44: <ret_expr_list> → ε
            PARSER_EAT();
            return EXIT_OK;
    }

    return ERR_SYNTAX;
}

#endif
