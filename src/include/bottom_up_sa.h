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

#define ERR '0'

/**
 *	@brief Function return index to the precedence table,
 *		   according to the type of the terminal
 *	@param token Terminal of given string
 *	@return 
 */
int get_index(int token);

int analyze_bottom_up(FILE *f,parser_t *parser);

/**
 *	@brief Function reduces terminal on the top of the stack,
 *		   according to given rules
 *	@param stack Stack filled with terminals and non-terminals
 *	@return 1 if reduction was successful, elsewhere 0
 */
int reduce_terminal(PA_stack *stack);
#endif /** BOTOM_UP_PARSER */
