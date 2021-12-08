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
#include "symtable.h"
#include "parser.h"
#include "code_generator.h"

#define ERR_REDUCTION   0
#define EXIT_FUNC_ID    42
#define EXIT_EMPTY_EXPR 404
#define EXIT_ID_BEFORE  69

#define ERR '0'
#define END '1'
#define EPT '2'
#define  ID '3'

/**
 *	@brief Function return index to the precedence table,
 *		   according to the type of the terminal
 *	@param token Terminal of given string
 *	@return 
 */
int get_index(token_t *token);


/**
 *	@brief Function reduces terminal on the top of the stack,
 *		   according to given rules
 *	@param stack Stack filled with terminals and non-terminals
 *	@param parser Structure passed from parser 
 *	@param local_symtab Local symtable of the current function 
 *	@return 1 if reduction was successful, elsewhere 0
 */
int reduce_terminal(PA_stack *stack,parser_t *parser, symtable_t *local_symtab);

/**
 *	@brief Function returns 1 if the given token
 *		   is not supported in precedence operator
 *		   parser, so it will terminate the 
 *		   operator precedence parser and gives 
 *		   gives the control back to the recursive descent
 *	@param token For identifying token type
 *	@return 1 if token is a keyword, else 0
 */
int switch_context(token_t* token);

/**
 *	@brief Function decides whether given token 
 *		   type is a literal or identifier.
 *	@param token_type For identifying token data type
 *	@return 1 if the given token type corresponds with literal.
 *			0 if the given token type corresponds with identifier.
 *		   -1 if the given token type is invalid.
 */
int is_literal(token_t* token);

/**
 *	@brief Function returns data type of the given token.
 *	@param token_type For identifying token data type
 *	@return data_type_t.
 */
int get_data_type(token_t* token);

int analyze_bottom_up(parser_t *parser);

#endif /** BOTOM_UP_PARSER */
