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

#define ERR '0'
/**
 *	@brief Function return index to the precedence table,
 *		   according to the type of the terminal
 *	@param token Terminal of given string
 *	@return 
 */
int get_index(int token);

int analyze_bottom_up(FILE *f);

#endif /** BOTOM_UP_PARSER */
