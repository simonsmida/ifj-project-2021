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
    
    CHECK_TOKEN_TYPE(TOKEN_REQUIRE);

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
    // TODO: if (result != TOKEN_EOF) return ERR_SYNTAX;
    CHECK_RESULT_VALUE(TOKEN_EOF);
    
    // TODO: generate instruction for program end
    //
    return EXIT_OK;
}

// <prolog>
int prolog(parser_t *parser)
{
    // <prolog> → 'require' 'ifj21'
    
    if (parser->token->type == TOKEN_REQUIRE) {
        // Get next token TODO: parser_eat?
        parser->token = get_next_token();
        CHECK_TOKEN_ERROR();
        if (parser->token->type == TOKEN_STR_LIT) {
            // Check if scanner correctly set attribute's string value
            if (parser->token->attribute->string == NULL) {
                return ERR_LEX;
            }
            // Next token has to be precisely "ifj21"
            if (strcmp(parser->token->attribute->string, "ifj21") != 0) {
                return ERR_SYNTAX; // TODO: check exit code
            }
        } else { // next token is not "ifj21"
            return ERR_SYNTAX;
        }
        return EXIT_OK;
    }
    return ERR_SYNTAX;
}

// <func_dec>
int func_dec(parser_t *parser)
{
    // <func_dec> → 'global' 'id' ':' 'function' '(' <param_fdec> ')' ':' <ret_type_list> <func_dec>

    if (parser->token->type == TOKEN_GLOBAL)
}
