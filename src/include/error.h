/**
 * @file  error.h
 *
 * @brief Exit codes and error messages for IFJ21 compiler
 * @date 2021-10-11
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdarg.h>

#define EXIT_OK              0  ///< no error during compilation process
#define ERR_LEX              1  ///< invalid structure of current lexeme
#define ERR_SYNTAX           2  ///< syntax error
#define ERR_SEMANTIC_DEF     3  ///< undefined function/variable or redefinition attempt
#define ERR_SEMANTIC_ASSIGN  4  ///< error in assignment command (type incompatibility)
#define ERR_SEMANTIC_PROG    5  ///< invalid number/type of parameters or return values
#define ERR_SEMANTIC_TC      6  ///< type compatibility error
#define ERR_SEMANTIC_OTHER   7  ///< other semantic errors
#define ERR_RUNTIME_NIL      8  ///< unexpected value nil
#define ERR_RUNTIME_ZERODIV  9  ///< integer division by constant zero
#define ERR_INTERNAL         99 ///< not influenced by input program


/**
 *  @brief Write formatted error message to stderr
 *
 *  @param where    Compilation segment where an error occured
 *  @param err_code error code
 *  @param fmt      actual error message
 *  @param ...      ellipsis - variadic arguments
 */
void error_message(const char *where, int err_code, const char *fmt, ...);

#endif // ERROR_H
