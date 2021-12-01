/**
 * @file  bottom_up_sa.c
 * @brief Operator precedence parser header file
 * 
 * @author  Filip Bučko
 * @date  	23.11.2021
 *
 * Compiled : gcc version 9.3.0
 */

#ifndef  BOTTOM_UP_PARSER
#define BOTTOM_UP_PARSER

#include "PA_stack.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

#define ERR '0'
#define SUCCESS 1


/**
 *	@brief Function returns index to the precedence table,
 *		   according to the type of the terminal
 *	@param token Terminal of given string
 *	@return index to precedence table
 */
int get_index(int token);

/**
 *	@brief Function returns if the given token
 *		   is a keyword which terminates the 
 *		   operator precedence parser
 *	@param token For identifying token type
 *	@return 1 if token is a keyword, else 0
 */
int is_input_keyword(token_t* token);

int analyze_bottom_up(FILE *f,parser_t *parser);

/**
 *	@brief Function reduces terminal on the top of the stack,
 *		   according to given rules
 *	@param stack Stack filled with terminals and non-terminals
 *	@return 1 if reduction was successful, elsewhere 0
 */
int reduce_terminal(PA_stack *stack);
#endif /** BOTOM_UP_PARSER */
