#include "include/recursive_descent.h"
#include "include/error.h"
#include "include/scanner.h"

#define CHECK_RESULT_VALUE(_value) \
    if (result != (_value)) return result 

#define CHECK_TOKEN_ERROR() \
    if (parser->token->type == TOKEN_ERROR) return ERR_LEX

#define GET_TOKEN() \
        parser->token = get_next_token()

#define CHECK_TOKEN_TYPE(_type) \
    if (parser->token->type != (_type)) return ERR_SYNTAX;

// Starting nonterminal <prog>
int prog(parser_t *parser)
{
    int result;
    
    // <prog> → <prolog> <func_dec> <func_def> <func_call> 'EOF'
    
    switch (parser->token->type) 
    {
        case TOKEN_REQUIRE:

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
            CHECK_RESULT_VALUE(TOKEN_EOF);

            // TODO: generate instruction for program end
            //
            return EXIT_OK; // TODO: break?
    } 
    
    return ERR_SYNTAX;
}

// <prolog>
int prolog(parser_t *parser)
{
    // <prolog> → 'require' 'ifj21'
    
    switch (parser->token->type) 
    {
        case TOKEN_REQUIRE: 

            GET_TOKEN(); // TODO: parser eat?
            CHECK_TOKEN_ERROR();
            CHECK_TOKEN_TYPE(TOKEN_STR_LIT);

            // Check if scanner correctly set attribute's string value
            if (parser->token->attribute->string == NULL) {
                return ERR_LEX;
            }
            
            // Check if next token is precisely "ifj21"
            if (strcmp(parser->token->attribute->string, "ifj21") != 0) {
                return ERR_SYNTAX;
            }
            
            // TODO: check this
            GET_TOKEN(); 
            CHECK_TOKEN_ERROR();

            return EXIT_OK;
    }

    return ERR_SYNTAX;
}

// <func_dec>
int func_dec(parser_t *parser)
{
    int result;

    // <func_dec> → 'global' 'id' ':' 'function' '(' <param_fdec> ')' ':' <ret_type_list> <func_dec>
    
    switch (parser->token->type) 
    {
        case TOKEN_GLOBAL: // 'global'
    
            GET_TOKEN();
            CHECK_TOKEN_ERROR();
            CHECK_TOKEN_TYPE(TOKEN_ID);       // 'id'
            // TODO: store to symtable

            GET_TOKEN();
            CHECK_TOKEN_ERROR();
            CHECK_TOKEN_TYPE(TOKEN_COLON);    // ':'

            GET_TOKEN();
            CHECK_TOKEN_ERROR();
            CHECK_TOKEN_TYPE(TOKEN_FUNCTION); // 'function'

            GET_TOKEN();
            CHECK_TOKEN_ERROR();
            CHECK_TOKEN_TYPE(TOKEN_L_PAR);     // '('

            // <param_fdec>
            result = param_fdec(); 
            CHECK_RESULT_VALUE(EXIT_OK);

            GET_TOKEN();
            CHECK_TOKEN_ERROR();
            CHECK_TOKEN_TYPE(TOKEN_R_PAR);     // ')'

            GET_TOKEN();
            CHECK_TOKEN_ERROR();
            CHECK_TOKEN_TYPE(TOKEN_COLON);     // ':'

            // TODO: check this
            GET_TOKEN(); 
            CHECK_TOKEN_ERROR();

            return EXIT_OK;
        
        case TOKEN_EOF: 
        case TOKEN_FUNCTION: // 'function'
        case TOKEN_EXPR: // TODO: check this
            
            // TODO: check this
            GET_TOKEN(); 
            CHECK_TOKEN_ERROR();

            // <func_dec> → ε
            return EXIT_OK;
    }

    return ERR_EXIT;
}
