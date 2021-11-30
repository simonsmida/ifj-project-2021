/**
 * @file  PA_stack.c
 * @brief Stack implementation for precedenet analysis
 * 
 * @author  Filip Bučko
 * @date  	20.11.2021
 *
 * Compiled : gcc version 9.3.0
 */

#include <stdlib.h>
#include <stdio.h>
#include "include/PA_stack.h" 

/** 
 * @brief Function initializes the stack for operator precedence parsing
 * 
 * @param stack Pointer to stack structure, which will be intialized
 */
void PA_stack_init(PA_stack *stack){
	if( stack != NULL ){
		stack -> top_index = -1;
		/** Init all pointers to the NULL */
		for(int i = 0; i < MAX_STACK_SIZE; i++){
			stack -> items[i].terminal  = NULL;
		}
	}
}

/** 
 * @brief Function checks if the stack is empty or not
 * 
 * @param stack Pointer to stack structure
 *
 * @return Funtion returns non-zero value, if the stack is empty.
 *		   If the stack pointer is not valid, function returns -1.
 */
int PA_stack_empty(const PA_stack *stack){
	if( stack != NULL ){
		return (stack -> top_index == -1);
	}
	return -1;
}

/** 
 * @brief Function checks if the stack is not full
 * 
 * @param stack Pointer to stack structure
 *
 * @return Funtion returns non-zero value, if the stack is full.
 *		   If the stack pointer is not valid, function returns -1.
 */
int PA_stack_full(const PA_stack *stack){
	if( stack != NULL ){
		return (stack -> top_index == (MAX_STACK_SIZE-1) );
	}
	return -1;
}
/** 
 * @brief Function returns the item on the top of the stack
 * 
 * @param stack Pointer to stack structure
 *
 * @return Funtion returns PA_item_t structure on the top of the stack.
 */
void PA_stack_top(const PA_stack *stack, PA_item_t* item){
	if( !PA_stack_empty(stack) ){
#if 0
		/** 1. If there is terminal on the top of the stack create a deep copy */
		if ( stack -> items[stack -> top_index].item_type ){
			token_t *copy;
			copy = copy_token(stack -> items[stack -> top_index].terminal);
			(*item).terminal = copy;
		}
#endif
		/** In the case of unitialized values, diferentiate which item is used and assign only that item */
		(*item).terminal = stack -> items[stack -> top_index].terminal;
		(*item).non_terminal = stack -> items[stack -> top_index].non_terminal;
		(*item).handle = stack -> items[stack -> top_index].handle;
		(*item).item_type = stack -> items[stack -> top_index].item_type;
	}
}

/** 
 * @brief Function returns the terminal
 *	      which is closest to the top of the stack
 * 
 * @param stack Pointer to stack structure
 * @param item  Pointer to item structure
 *
 * @return If the terminal was found on stack returns 1, else 0
 */
int PA_stack_top_terminal(const PA_stack *stack, PA_item_t* item){
	if ( !PA_stack_empty(stack) ){
		/** 1. Set the iteration index, which does not change the top index */
		int index = stack -> top_index;
		/** 2. Start searching the nearest terminal */
		while(index > -1){
			if ( stack -> items[index].item_type == 1 ){
#if 0
				/** 3. Dealloc the structure which will be overwrited */
				PA_item_destroy(*item);
				/** 4. We are finding the nearest terminal so we have to the deep copy */
				token_t *copy;
				copy = copy_token(stack -> items[stack -> top_index].terminal);
				(*item).terminal = copy;
				(*item).non_terminal = stack -> items[stack -> top_index].non_terminal;
				(*item).item_type = stack -> items[stack -> top_index].item_type;
#endif
				(*item).terminal = stack -> items[index].terminal;
				(*item).non_terminal = stack -> items[index].non_terminal;
				(*item).item_type = stack -> items[index].item_type;
				return 1;
			}
			index--;
		}
	}
	return 0;
}

/** 
 * @brief Function deallocates all dynamic elements in the stack
 *		  and empties it.
 * 
 * @param stack Pointer to stack structure
 */
void PA_stack_destroy(PA_stack *stack){
	while (stack->top_index != -1){
		PA_item_t item;
		PA_stack_top(stack,&item);
		if(item.item_type == 1){
			destroy_token(item.terminal);
		}
		PA_stack_pop(stack);	
	}
}

/** 
 * @brief Function deallocates all dynamic elements in the item of the stack.
 * 
 * @param item  Pointer to the item which will be deallocated
 */
void PA_item_destroy(PA_item_t item){
	if ( item.terminal != NULL ){
		destroy_token(item.terminal);
	}
}

/** 
 * @brief Function removes the item from the top of the stack
 * 
 * @param stack Pointer to stack structure
 */
void PA_stack_pop(PA_stack *stack){
	if( !PA_stack_empty(stack) ){
		/*if( stack -> items[stack->top_index].item_type == 1 ){
			destroy_token(stack -> items[stack -> top_index].terminal);
		}*/
		stack -> top_index--;
	}
}

/** 
 * @brief Function will push the item on the top of the stack
 * 
 * @param stack Pointer to stack structure
 * @param item  Item structure
 * @param type  Type of item being pushed (Terminal/Nonterminal)
 */
void PA_stack_push(PA_stack *stack, PA_item_t item,int type){
	/** 1. For assigning the structure create a deep copy*/
	if ( !PA_stack_full(stack) ){
		stack -> top_index++;
		/*if(type == 1){
			token_t *copy;
			copy = copy_token(item.terminal);
			stack -> items[stack -> top_index].terminal = copy;
		}*/
		stack -> items[stack -> top_index].terminal = item.terminal;
		stack -> items[stack -> top_index].non_terminal = item.non_terminal;
		stack -> items[stack -> top_index].handle = item.handle;
		stack -> items[stack -> top_index].item_type = type;
	}
}
