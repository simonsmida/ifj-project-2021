#include "include/error.h"

/**
 *  @brief Write formatted error message to stderr
 */
void error_message(const char *where, int err_code, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    
    fprintf(stderr, "[%s]: ", where);    
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n ↳ %s - error code: [%d]\n", textify(err_code), err_code);

    va_end(ap);
}

/**
 * @brief Returns textified representation of given error code
 */
char *textify(int err_code)
{
    switch (err_code)
    {
        case EXIT_OK:
            return TEXTIFY(EXIT_OK);        
        case ERR_LEX:
            return TEXTIFY(ERR_LEX);    
        case ERR_SYNTAX:
            return TEXTIFY(ERR_SYNTAX);        
        case ERR_SEMANTIC_DEF:
            return TEXTIFY(ERR_SEMANTIC_DEF);    
        case ERR_SEMANTIC_ASSIGN:
            return TEXTIFY(ERR_SEMANTIC_ASSIGN);        
        case ERR_SEMANTIC_PROG:
            return TEXTIFY(ERR_SEMANTIC_PROG);        
        case ERR_SEMANTIC_TC:
            return TEXTIFY(ERR_SEMANTIC_TC);    
        case ERR_SEMANTIC_OTHER:
            return TEXTIFY(ERR_SEMANTIC_OTHER);        
        case ERR_RUNTIME_NIL:
            return TEXTIFY(ERR_RUNTIME_NIL);    
        case ERR_RUNTIME_ZERODIV:
            return TEXTIFY(ERR_RUNTIME_ZERODIV);        
        case ERR_INTERNAL:
            return TEXTIFY(ERR_INTERNAL);
        default:
           return "Unknown error code";
    } 
}
