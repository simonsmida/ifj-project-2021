/**
 * @file  bottom_up_sa.c
 * @brief Operator precedence parser implementation
 * 
 * @author  Filip Bučko
 * @date  	23.11.2021
 *
 * Compiled : gcc version 9.3.0
 */

#include <stdio.h>
#include "include/scanner.h"
#include "include/bottom_up_sa.h"
#include "include/error.h"


char precedence_table[19][19] = 
{
		/************************************** Input *********************************************/
/*stack  |  #  | * |  /  |  //  | + | - | .. | < | > | <= | >= | == | ~= | ( | ) | id | f | , | $ */
/*	#  */{ ERR ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	*  */{ '<' ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	/  */{ '<' ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	// */{ '<' ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	+  */{ '<' ,'<', '<' , '<'  ,'>','>', '>','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	-  */{ '<' ,'<', '<' , '<'  ,'>','>', '>','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	.. */{ '<' ,'<', '<' , '<'  ,'<','<', '<','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	<  */{ '<' ,'<', '<' , '<'  ,'<','<', '<','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	>  */{ '<' ,'<', '<' , '<'  ,'<','<', '<','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	<= */{ '<' ,'<', '<' , '<'  ,'<','<', '<','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	>= */{ '<' ,'<', '<' , '<'  ,'<','<', '<','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	== */{ '<' ,'<', '<' , '<'  ,'<','<', '<','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	~= */{ '<' ,'<', '<' , '<'  ,'<','<', '<','>','>', '>', '>', '>', '>','<','>', '<','<','>', '>'},
/*	(  */{ '<' ,'<', '<' , '<'  ,'<','<', '<','<','<', '<', '<', '<', '<','<','=', '<','<','=', ERR},
/*	)  */{ ERR ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>',ERR,'>', END,ERR,'>', '>'},
/*	id */{ ERR ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>',ERR,'>', END,ERR,'>', '>'},
/*	f  */{ ERR ,ERR, ERR , ERR  ,ERR,ERR, ERR,ERR,ERR, ERR, ERR, ERR, ERR,'=',ERR, ERR,ERR,ERR, ERR},
/*	,  */{ '<' ,'<', '<' , '<'  ,'<','<', '<','<','<', '<', '<', '<', '<','<','=', '<','<','=', ERR},
/*	$  */{ '<' ,'<', '<' , '<'  ,'<','<', '<','<','<', '<', '<', '<', '<','<',ERR, '<','<',ERR, EPT}};

/**
 *	@brief Function reduces terminal on the top of the stack,
 *		   according to given rules
 *	@param stack Stack filled with terminals and non-terminals
 *	@return 1 if reduction was successful, elsewhere 0
 */
int reduce_terminal(PA_stack *stack,symtable_t *local_symtab){
	/** Reduce terminal */
	PA_item_t items[4];
	PA_item_t top_item;
	int operands_count = 0;
	int zero_div_error = 0;	
	while(1){
		PA_stack_top(stack, &top_item);
		if(top_item.item_type == 2){
			PA_stack_pop(stack);
			break;
		}
		else{
			items[operands_count] = top_item;
			operands_count++;
			PA_stack_pop(stack);
		}
	}
	//printf("Operands count: %d\n ",operands_count);
	/*Reduce terminal 
	  E -> id
	  E -> int
	  E -> num
	  E -> str
	 */
	if(operands_count == 1){
		//1.Check item type
		//2.Reduce
		if( (items[0].item_type == 1) && 
		    (items[0].terminal->type == TOKEN_ID	   ||
			 items[0].terminal->type == TOKEN_INT_LIT  ||
			 items[0].terminal->type == TOKEN_NUM_LIT  ||
			 items[0].terminal->type == TOKEN_STR_LIT  ||
			(items[0].terminal->type == TOKEN_KEYWORD  &&
			(items[0].terminal->attribute->keyword_type == KEYWORD_NIL))
			)){
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the item type -> whether it's a variable or literal */
			PA_item_t reduced_terminal;
			if (is_literal(items[0].terminal)){
			/*	1.1 Assign the type of the item to the newly created non-terminal */
				int literal_dtype = get_data_type(items[0].terminal);
				reduced_terminal.non_terminal.dtype = literal_dtype;
				/** If literal is int, assign int value to nonterminal */
				if (literal_dtype == 1){
					reduced_terminal.non_terminal.int_value    = items[0].terminal->attribute->integer;
				}
				/** If literal is number, assign double value to nonterminal */
				else if (literal_dtype == 2){
					reduced_terminal.non_terminal.double_value = items[0].terminal->attribute->number;
				}
			}
			/** Item is a variable */
			else{
			/*	1.2 Check if it was declared */
					char *var_id = items[0].terminal->attribute->string;
					symtable_item_t *search = symtable_search(local_symtab, var_id);
					/** Variable was not declared -> was not found in symtable */
					if (search == NULL){
						error_message("Parser", ERR_SEMANTIC_DEF,  "Variable '%s' is not declared.",var_id);
						destroy_token(items[operands_count-1].terminal);
						return ERR_SEMANTIC_DEF;
					}
					/** Variable was declared
					 *	Assign the type of the variable to the non-terminal */
					else{
						reduced_terminal.non_terminal.dtype = search->const_var->type;
					}
			}
			/*------------------------------------------------------------------------*/
			/*
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 *	free in code generator
			 **/
			destroy_token(items[operands_count-1].terminal);
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			PA_stack_top(stack,&top_item);
			return 1;
		}
	}
	else if(operands_count == 2){
		PA_item_t reduced_terminal;
		if(	((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_STRLEN)) &&
			(items[0].item_type == 0) ){
			/** Semantic action 
			 *	1. Check the item type -> whether it's a variable or literal
			 *	2. If the item is variable -> check if it was declared
			 *	2.1 If the item was id -> get the type from symtab
		 	 *	2.2 If the item was literal -> get the type from token type 	 
			 *	3. Only accepted type is string
			 *	4. Result of the operation # should be integer, so
			 *	   the type of reduced non-terminal will be int 
			 *	5. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
		
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			return 1;
		}
		else{
			while(operands_count > 0){
				operands_count--;
				if(items[operands_count].item_type == 1){
					destroy_token(items[operands_count].terminal);
				}
			}
			return 0;
		}
	}
	else if(operands_count == 3){
		/** RULE: E->E+E */
		PA_item_t reduced_terminal;
		if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_PLUS)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					if (first_op == DTYPE_INT ){
						reduced_terminal.non_terminal.dtype = DTYPE_INT;
					}
					else{
						reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					}
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation + with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int + int -> int
			 *		num + num -> num
			 *		num + int -> num
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int + num -> num
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E-E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_MINUS)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					if (first_op == DTYPE_INT ){
						reduced_terminal.non_terminal.dtype = DTYPE_INT;
					}
					else{
						reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					}
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation + with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '-' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '-' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int - int -> int
			 *		num - num -> num
			 *		num - int -> num
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int - num -> num
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E*E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_MUL)) &&
			(items[2].item_type == 0) ){
		
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					if (first_op == DTYPE_INT ){
						reduced_terminal.non_terminal.dtype = DTYPE_INT;
					}
					else{
						reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					}
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation * with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '*' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '*' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int * int -> int
			 *		num * num -> num
			 *		num * int -> num
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int * num -> num
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E/E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_DIV)) &&
			(items[2].item_type == 0) ){
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			/** Attention first and second operand may seem in reverse order 
			 *	but they are not because of the stack */
			int first_op  = items[2].non_terminal.dtype; //first operand data type
			int second_op = items[0].non_terminal.dtype; //second operand data type
			non_terminal_t divisor = items[0].non_terminal; //divisor
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
					
				if(first_op == second_op){
					if(first_op == DTYPE_INT){
						if(divisor.int_value != 0){
							reduced_terminal.non_terminal.dtype = DTYPE_INT;
							//call generator for INT_DIV
						}
						else{
							goto zero_division_error;
						}
					}
					else if(first_op == DTYPE_NUMBER){
						if(divisor.double_value != 0){
							reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
							//call generator for DIV
						}
						else{
							goto zero_division_error;
						}
					}
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					if(divisor.double_value != 0){
						reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
						//convert first
						//call generator
					}
					else{
						goto zero_division_error;
					}
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					if(divisor.int_value != 0){
						reduced_terminal.non_terminal.dtype = DTYPE_NUMBER;
						//convert second
						//call generator
					}
					else{
						goto zero_division_error;
					}
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation / with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '/' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '/' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}

			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int / int -> int
			 *		num / num -> num
			 *		num / int -> num
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int / num -> num
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E//E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_INT_DIV)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			/** Attention first and second operand may seem in reverse order 
			 *	but they are not because of the stack */
			int first_op  = items[2].non_terminal.dtype; //first operand data type
			int second_op = items[0].non_terminal.dtype; //second operand data type
			non_terminal_t divisor = items[0].non_terminal; //divisor
			if ( (first_op  == DTYPE_INT) && (first_op  == DTYPE_INT) ){
				if (divisor.int_value != 0){
					reduced_terminal.non_terminal.dtype = DTYPE_INT;
					//call generator
				}
				else{
					goto zero_division_error;
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation / with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '//' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '//' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}

			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int // int -> int
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E..E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_CONCAT)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			/** Attention first and second operand may seem in reverse order 
			 *	but they are not because of the stack */
			int first_op  = items[2].non_terminal.dtype; //first operand data type
			int second_op = items[0].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_STRING) && (first_op  == DTYPE_STRING) ){
					reduced_terminal.non_terminal.dtype = DTYPE_INT;
					//call generator
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation / with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '//' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '//' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}

			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		str .. str -> str
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E < E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_LT)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation + with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int < int -> bool
			 *		num < num -> bool
			 *		num < int -> bool
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int < num -> bool
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E > E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_GT)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation + with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int > int -> bool
			 *		num > num -> bool
			 *		num > int -> bool
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int > num -> bool
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E <= E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_LE)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation + with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int <= int -> bool
			 *		num <= num -> bool
			 *		num <= int -> bool
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int <= num -> bool
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E >= E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_GE)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
				/** Runtime error, operation + with nil*/
				error_message("Parser", ERR_RUNTIME_NIL,  "Runtime error, unexpected nil value in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_RUNTIME_NIL;
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int >= int -> bool
			 *		num >= num -> bool
			 *		num >= int -> bool
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int >= num -> bool
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E == E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_EQ)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					if (first_op == DTYPE_INT ){
						reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					}
					else{
						reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					}
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//generate code
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int == int -> bool
			 *		num == num -> bool
			 *		num == int -> bool
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int == num -> bool
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 * 		+ all combinations with nil
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		
		//USE E->E ~= E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_NOT_EQ)) &&
			(items[2].item_type == 0) ){
			
			/** Semantic action */
			/*------------------------------------------------------------------------*/
			/**	1. Check the non-terminal type */
			int first_op  = items[0].non_terminal.dtype; //first operand data type
			int second_op = items[2].non_terminal.dtype; //second operand data type
			if ( (first_op  == DTYPE_INT || first_op  == DTYPE_NUMBER) &&
				 (second_op == DTYPE_INT || second_op == DTYPE_NUMBER)){
				
				if(first_op == second_op){
					if (first_op == DTYPE_INT ){
						reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					}
					else{
						reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					}
					//everything ok call generator
				}
				else if(first_op == DTYPE_INT && second_op == DTYPE_NUMBER){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert first
					//call generator
				}
				else if(first_op == DTYPE_NUMBER && second_op == DTYPE_INT){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//convert second
					//call generator
				}
			}
			else if (first_op == DTYPE_NIL || second_op == DTYPE_NIL){
					reduced_terminal.non_terminal.dtype = DTYPE_BOOL;
					//generate code
			}
			else{
				//error 6
				/** Incompatible types error */
				error_message("Parser", ERR_SEMANTIC_TC,  "Incompatible types, in '+' operation");
				destroy_token(items[1].terminal);
				return ERR_SEMANTIC_TC;
			}
			 /*	2. Only accepted types in operation
			 *	Allowed types in operations:
			 *		int ~= int -> bool
			 *		num ~= num -> bool
			 *		num ~= int -> bool
			 *    		   |---> inttofloat -> send instruction for converting the data type
			 *		int ~= num -> bool
			 *   	 |---> inttofloat -> send instruction for converting the data type
			 * 		+ all combinations with nil
			 *	4. Send instruction for generating the code with token
			 *	TODO Don't forget too remove destroy_token above, while generating!
			 **/
			
			/** END of Semantic action */
			/*------------------------------------------------------------------------*/
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		
		//USE E -> )E(
		else if(((items[0].item_type == 1) && (items[0].terminal->type == TOKEN_R_PAR)) &&
				 (items[1].item_type == 0) && 
				((items[2].item_type == 1) && (items[2].terminal->type == TOKEN_L_PAR))){
			//Pass just the attributes of the non_terminal
				
			reduced_terminal.non_terminal.dtype = items[1].non_terminal.dtype;
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[0].terminal);
			destroy_token(items[2].terminal);
			return 1;
		}
	}
	if(zero_div_error){
		zero_division_error:
		//error 9
		error_message("Parser", ERR_RUNTIME_ZERODIV,  "Zero division error.");
		destroy_token(items[1].terminal);
		return ERR_RUNTIME_ZERODIV;
	}
	return 0;
}

int analyze_bottom_up(parser_t *parser){
	//Dealloc the read token from recursive descent
	destroy_token(parser->token);
	
	/** 1. Create stack */
	//Static allocation
	PA_stack stack;
	
	/** 2. Init the stack and parser token */
	PA_stack_init(&stack);
	parser -> token = NULL;
	
	/** 3. Push $ sign on the stack */
	/** 3.1 Generate $ token */
	PA_item_t item,top_terminal, token_in, handle;
	token_in.terminal = NULL;
	item.terminal = generate_empty_token();
    int prev_token_type = -1;

	/** 3.2 Push $ on the top of the stack */
	PA_stack_push(&stack, item, 1);


	int reduction = 0; //Variable determines, whether to get next token
	do{
		/** 4. Get terminal from the top of the stack and from the input */
		//Stack top
		PA_stack_top_terminal(&stack,&top_terminal);
		
		//Token in
		if(!reduction){
            /** Store type of previous token */
            if (token_in.terminal != NULL) {
                prev_token_type = token_in.terminal->type;
            }
			
			token_in.terminal = get_next_token(parser -> src);
			
			/** If the generated token is a function id, return read token
			  	and control to recursive descent parser */
			if(token_in.terminal->type == TOKEN_EOF){
					parser -> token = token_in.terminal;
					/** Dealloc the stack */
					PA_stack_destroy(&stack);
					error_message("Parser", ERR_SYNTAX,  "EOF while processing the expression"); 
					return ERR_SYNTAX;
			}
			if(token_in.terminal->type == TOKEN_ID){
				symtable_item_t *id = symtable_search(parser->global_symtable, token_in.terminal->attribute->string);
				if ((id != NULL) && (id -> function != NULL) ){
					parser -> token = token_in.terminal;
					/** Dealloc the stack */
					PA_stack_destroy(&stack);
					return EXIT_FUNC_ID;
				}
			}
			/** If the generated token has not supported type, transfrom it as $,
			 	and return read token and control to recursive descent parser */	
			else if(switch_context(token_in.terminal)){
				parser->token = copy_token(token_in.terminal);
				token_in.terminal -> type = TOKEN_EOF;
				reduction = 1;
			}else{
				reduction = 0;
			}
		}
		//printf("Type of token:%d\n",token_in.terminal->type);
		/**5. Look the operator priority in the precedence table */
		switch(precedence_table[get_index(top_terminal.terminal)][get_index(token_in.terminal)]){
			case '<':
				PA_stack_top(&stack,&item);
				handle.handle = '<';
				if( item.item_type == 1){
					/** 5.1 Push the intial char of the handle first */
					PA_stack_push(&stack,handle,2);
					
					/** 5.2 Push terminal from input on the stack */
					PA_stack_push(&stack,token_in,1);
				}else if (item.item_type == 0){
					/** 5.1 Push the intial char of the handle first
					 	If there is a non-terminal on the top of the stack,
					 	push behind the non-terminal */
					PA_stack_pop(&stack);
					PA_stack_push(&stack,handle,2);
					PA_stack_push(&stack,item,0);
					
					/** 5.2 Push terminal from input on the stack */
					PA_stack_push(&stack,token_in,1);
				}
				reduction = 0;
				break;
			case '=': 
				/** Shift terminal on the top of the stack */
				PA_stack_push(&stack,token_in,1);
				reduction = 0;
				break;
			case '>': 
				/** While reducing the terminal, we don't want to get
				 *	new terminal from the stream. Therefore we set the reduction variable
				 *	to 1, to prevent the new token from reading from stdin */
				if( parser->curr_func == NULL || parser->curr_func->function == NULL ||
					parser->curr_func->function->local_symtable == NULL ){
					error_message("Parser", ERR_INTERNAL,  "Internal error.");
					destroy_token(token_in.terminal);
					PA_stack_destroy(&stack);
					return ERR_INTERNAL;
				}
				int reduction_result = reduce_terminal(&stack,SYMTAB_L);
				if(reduction_result == ERR_REDUCTION ){
					destroy_token(token_in.terminal);
					PA_stack_destroy(&stack);
                    return (prev_token_type == TOKEN_ID) ? EXIT_ID_BEFORE :
					(error_message("Parser", ERR_SYNTAX,  "No reduction rule."),ERR_SYNTAX);
				}
				else if(reduction_result == ERR_SEMANTIC_TC ){
					destroy_token(token_in.terminal);
					PA_stack_destroy(&stack);
					return ERR_SEMANTIC_TC;
				}
				else if(reduction_result == ERR_RUNTIME_NIL ){
					destroy_token(token_in.terminal);
					PA_stack_destroy(&stack);
					return ERR_RUNTIME_NIL;
				}
				reduction = 1;
				
				PA_stack_top_terminal(&stack,&top_terminal);
				break;
			case ERR: 
				PA_stack_destroy(&stack);
				parser -> token = token_in.terminal;
                return (prev_token_type == TOKEN_ID) ? EXIT_ID_BEFORE :
				(error_message("Parser", ERR_SYNTAX,  "Missing operator."),ERR_SYNTAX);
			case EPT:
				/** Dealloc the stack */
				PA_stack_destroy(&stack);
				return EXIT_EMPTY_EXPR; 
			case END:
				parser -> token = token_in.terminal;
				/** Dealloc the stack */
				PA_stack_destroy(&stack);
				return EXIT_OK;
		}
	}while(((top_terminal.terminal->type != TOKEN_EOF) || (token_in.terminal->type != TOKEN_EOF)));
	
	/** Check if the PA was successful */
	if((top_terminal.terminal->type == TOKEN_EOF) && (token_in.terminal->type == TOKEN_EOF)){
		destroy_token(top_terminal.terminal);
		destroy_token(token_in.terminal);
	}
	return EXIT_OK; 	
}

/**
 *	@brief Function returns index to the precedence table,
 *		   according to the type of the terminal
 *	@param token Terminal of given string
 *	@return index to precedence table
 */
int get_index(token_t *token){
	switch(token->type){
		case TOKEN_STRLEN	: return  0;
		case TOKEN_MUL  	: return  1;
		case TOKEN_DIV  	: return  2;
		case TOKEN_INT_DIV  : return  3;
		case TOKEN_PLUS		: return  4;
		case TOKEN_MINUS	: return  5;
		case TOKEN_CONCAT   : return  6;
		case TOKEN_LT		: return  7;
		case TOKEN_GT  		: return  8;
		case TOKEN_LE		: return  9;
		case TOKEN_GE		: return 10;
		case TOKEN_EQ		: return 11;
		case TOKEN_NOT_EQ	: return 12;
		case TOKEN_L_PAR	: return 13;
		case TOKEN_R_PAR	: return 14;
		case TOKEN_ID		: return 15;
		case TOKEN_INT_LIT	: return 15;
		case TOKEN_NUM_LIT	: return 15;
		case TOKEN_STR_LIT	: return 15;
		case TOKEN_KEYWORD:
			if (token->attribute->keyword_type == KEYWORD_NIL){
				return 15;
			}
		case TOKEN_EOF		: return 18;
		default				: break;
	};
   return -1;	
}

/**
 *	@brief Function decides whether given token 
 *		   type is a literal or identifier.
 *	@param token_type For identifying token data type
 *	@return 1 if the given token type corresponds with literal.
 *			0 if the given token type corresponds with identifier.
 *		   -1 if the given token type is invalid.
 */
int is_literal(token_t *token){
	switch(token->type){
		case TOKEN_INT_LIT	: return 1;
		case TOKEN_NUM_LIT	: return 1;
		case TOKEN_STR_LIT	: return 1;
		case TOKEN_KEYWORD  :
			if(token->attribute->keyword_type == KEYWORD_NIL)
				return 1;
		case TOKEN_ID		: return 0;
		default				: break;
	}
	return -1;
}

/**
 *	@brief Function returns data type of the given token.
 *	@param token_type For identifying token data type
 *	@return data_type_t.
 */
int get_data_type(token_t* token){
	switch(token->type){
		case TOKEN_INT_LIT	: return DTYPE_INT;
		case TOKEN_NUM_LIT	: return DTYPE_NUMBER;
		case TOKEN_STR_LIT	: return DTYPE_STRING;
		case TOKEN_KEYWORD  :
			if(token->attribute->keyword_type == KEYWORD_NIL){
				return DTYPE_NIL;
			}
		case TOKEN_ID		: //Search in symtab return 0;
		default				: break;
	}
	return -1;
}

/**
 *	@brief Function returns data type of the
 *		   given token, while using reduction
 *		   rule for IDs/literals.
 *	@param token_type For identifying token data type
 *	@return token data type.
 */
int get_type(token_t* token){
	switch(token->type){
		case TOKEN_STRLEN	: return  0;
		case TOKEN_MUL  	: return  1;
		case TOKEN_DIV  	: return  2;
		case TOKEN_INT_DIV  : return  3;
		case TOKEN_PLUS		: return  4;
		case TOKEN_MINUS	: return  5;
		case TOKEN_CONCAT   : return  6;
		case TOKEN_LT		: return  7;
		case TOKEN_GT  		: return  8;
		case TOKEN_LE		: return  9;
		case TOKEN_GE		: return 10;
		case TOKEN_EQ		: return 11;
		case TOKEN_NOT_EQ	: return 12;
		case TOKEN_L_PAR	: return 13;
		case TOKEN_R_PAR	: return 14;
		case TOKEN_ID		: return 15;
		case TOKEN_INT_LIT	: return 15;
		case TOKEN_NUM_LIT	: return 15;
		case TOKEN_STR_LIT	: return 15;
		case TOKEN_EOF		: return 18;
		default: break;
	};
   return -1;	
}
/**
 *	@brief Function returns 1 if the given token
 *		   is not supported in precedence operator
 *		   parser, so it will terminate the 
 *		   operator precedence parser and gives 
 *		   gives the control back to the recursive descent
 *	@param token For identifying token type
 *	@return 1 if token is a keyword, else 0
 */
int switch_context(token_t* token){
	switch(token->type){
		case TOKEN_L_PAR:
		case TOKEN_R_PAR:

		// Arithmetic operators
		case TOKEN_PLUS:
		case TOKEN_MINUS:
		case TOKEN_MUL:
		case TOKEN_DIV:

		// Relational operators
		case TOKEN_EQ:     
		case TOKEN_NOT_EQ: 
		case TOKEN_LT:     
		case TOKEN_LE:     
		case TOKEN_GT:     
		case TOKEN_GE:     
		case TOKEN_INT_DIV:
		
		// Other operators
		case TOKEN_STRLEN: 
		case TOKEN_CONCAT: 

		// Identifier
		case TOKEN_ID:
		case TOKEN_STR_LIT:
		case TOKEN_INT_LIT:
		case TOKEN_NUM_LIT: return 0;
		//case TOKEN_EOF: return 0;
		case TOKEN_KEYWORD:
			if (token->attribute->keyword_type == KEYWORD_NIL){
				return 0;
			}
		default: break;
	}
	return 1; 
}             
