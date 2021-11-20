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
    fprintf(stderr, ", ERROR CODE [%d]\n", err_code);

    va_end(ap);
}

