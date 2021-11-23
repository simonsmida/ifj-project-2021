/**
 * @file PA_stack.h
 * 
 * @brief  Functions and structures used in PA_stack
 * @author Filip Bučko
 * @date   20.11.2021
 */

#ifndef PA_STACK_H
#define PA_STACK_H

#include "scanner.h"
#include "symtable.h"

#define MAX_STACK_SIZE 100

/** Defining constants for recognising expressions and list of expressions */
typedef enum non_terminals_type{
	EXPR,
	EXPR_LIST
}non_terminal_type;

/** Structure for storing non-terminals */
typedef struct nonterminal{
	non_terminal_type type;
	int attribute;
}non_terminal_t;

/** Stack item structure */
typedef struct pa_item{
	//Terminal
	//TODO Probalby add ptr to token -> functions return ptr to tokens
	token_t terminal;
	//Nonterminal
	//TODO Probalby add ptr to non_terminal_t, for comparing with NULL
	non_terminal_t non_terminal;
	//Symtabptr
	symtable_t * symtable;	
}PA_item_t;

/** Stack structure */
typedef struct pa_stack{
	/** Array for storing the values */
	PA_item_t items[MAX_STACK_SIZE];
	/** Index of an element on the top of the stack */
	int top_index;
}PA_stack;

/** 
 * @brief Function initializes the stack for operator precedence parsing
 * 
 * @param stack Pointer to stack structure, which will be intialized
 */
void PA_stack_init(PA_stack *stack);

/** 
 * @brief Function checks if the stack is empty or not
 * 
 * @param stack Pointer to stack structure
 *
 * @return Funtion returns non-zero value, if the stack is empty.
 */
int PA_stack_empty(const PA_stack *stack);

/** 
 * @brief Function checks if the stack is not full
 * 
 * @param stack Pointer to stack structure
 *
 * @return Funtion returns non-zero value, if the stack is full.
 *		   If the stack pointer is not valid, function returns -1.
 */
int PA_stack_full(const PA_stack *stack);

/** 
 * @brief Function returns the item on the top of the stack
 * 
 * @param stack Pointer to stack structure
 * @param item  Pointer to item structure
 */
void PA_stack_top(const PA_stack *stack, PA_item_t* item);

/** 
 * @brief Function removes the item from the top of the stack
 * 
 * @param stack Pointer to stack structure
 */
void PA_stack_pop(PA_stack *stack);

/** 
 * @brief Function will push the item on the top of the stack
 * 
 * @param stack Pointer to stack structure
 * @param item  Item structure
 */
void PA_stack_push(PA_stack *stack, PA_item_t item);

void skuska(PA_stack *zasobnicek);
#endif /** PA_STACK_H */
