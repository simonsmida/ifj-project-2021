#include "include/recursive_descent.h"
#include "include/error.h"
#include "include/scanner.h"

#define CHECK_RESULT_NOT_OK() if (result != EXIT_OK) return result

// Starting nonterminal <prog>
int prog(parser_t *parser)
{
    int result;
    
    // <prog> → <prolog> <func_dec> <func_def> <func_call> 'EOF'
    
    if (token == require) {
        // <prolog>
        result = prolog(parser); 
        CHECK_RESULT_NOT_OK();
        // <func_dec>
        result = func_dec(parser);
        CHECK_RESULT_NOT_OK();
        // <func_def>
        result = func_def(parser);
        CHECK_RESULT_NOT_OK();
        // <func_call>
        result = func_call(parser);
        CHECK_RESULT_NOT_OK();
        
        // Check End Of File
        if (result != TOKEN_EOF) return ERR_SYNTAX;
        
        // TODO: generate instruction for program end
        //
        return EXIT_OK;
    }

    // Unexpected token occured - TODO: check error code
    return ERR_SYNTAX;
}

// <prolog>
int prolog(parser_t *parser)
{
    int result;
    
    // <prolog> → 'require' 'ifj21'
    
    if (token == require)
}
