#include "include/error.h"

/**
 *  @brief Write formatted error message to stderr
 */
void error_message(const char *where, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    
    fprintf(stderr, "[%s]: ", where);    
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    va_end(ap);
}

