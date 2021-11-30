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
#include "include/bottom_up_sa.h"

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
/*	)  */{ ERR ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>',ERR,'>', ERR,ERR,'>', '>'},
/*	id */{ ERR ,'>', '>' , '>'  ,'>','>', '>','>','>', '>', '>', '>', '>',ERR,'>', ERR,ERR,'>', '>'},
/*	f  */{ ERR ,ERR, ERR , ERR  ,ERR,ERR, ERR,ERR,ERR, ERR, ERR, ERR, ERR,'=',ERR, ERR,ERR,ERR, ERR},
/*	,  */{ '<' ,'<', '<' , '<'  ,'<','<', '<','<','<', '<', '<', '<', '<','<','=', '<','<','=', ERR},
/*	$  */{ '<' ,'<', '<' , '<'  ,'<','<', '<','<','<', '<', '<', '<', '<','<',ERR, '<','<',ERR, ERR}};

/**
 *	@brief Function reduces terminal on the top of the stack,
 *		   according to given rules
 *	@param stack Stack filled with terminals and non-terminals
 *	@return 1 if reduction was successful, elsewhere 0
 */
int reduce_terminal(PA_stack *stack){
	/** Reduce terminal */
	PA_item_t items[4];
	PA_item_t top_item;
	int operands_count = 0;
	
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
	printf("Operands count: %d\n ",operands_count);
	/*Reduce terminal 
	  E -> id
	  E -> int
	  E -> num
	  E -> str
	 */
	if(operands_count == 1){
		//1.Check item type
		//2.Reduce
		if((items[0].item_type == 1) && (items[0].terminal->type == TOKEN_ID ||
										 items[0].terminal->type == TOKEN_INT_LIT ||
										 items[0].terminal->type == TOKEN_NUM_LIT ||
										 items[0].terminal->type == TOKEN_STR_LIT)){
			destroy_token(items[operands_count-1].terminal);
			PA_item_t reduced_terminal;
			reduced_terminal.non_terminal.expr_type = EXPR;
			printf("Zredukoval som ID\n");
			PA_stack_push(stack,reduced_terminal,0);
			PA_stack_top(stack,&top_item);
			printf("Vrchol zasobnika: %d\n\n", top_item.item_type);
			return 1;
		}
	}
	else if(operands_count == 2){
		PA_item_t reduced_terminal;
		//printf("Token type: %d\n", items[1].terminal->type);
		if(	((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_STRLEN)) &&
			(items[0].item_type == 0) ){
		
			printf("Zredukoval binarny operator\n");
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
		//USE E->E+E
		PA_item_t reduced_terminal;
		if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_PLUS)) &&
			(items[2].item_type == 0) ){
		
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E-E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_MINUS)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E*E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_MUL)) &&
			(items[2].item_type == 0) ){
		
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E/E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_DIV)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E//E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_INT_DIV)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E..E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_CONCAT)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E < E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_LT)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E > E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_GT)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E <= E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_LE)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E >= E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_GE)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		//USE E->E == E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_EQ)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		
		//USE E->E ~= E
		else if( (items[0].item_type == 0) &&
			((items[1].item_type == 1) && (items[1].terminal->type == TOKEN_NOT_EQ)) &&
			(items[2].item_type == 0) ){
			printf("Token type: %d\n", items[1].terminal->type);
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[1].terminal);
			return 1;
		}
		
		//USE E -> )E(
		else if(((items[0].item_type == 1) && (items[0].terminal->type == TOKEN_R_PAR)) &&
				 (items[1].item_type == 0) && 
				((items[2].item_type == 1) && (items[2].terminal->type == TOKEN_L_PAR))){
			//printf("Token type: %d\n", items[1].terminal->type);
		
			printf("Zredukoval som zatvorky\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
			destroy_token(items[0].terminal);
			destroy_token(items[2].terminal);
			return 1;
		}
	}
	return 0;
}

int analyze_bottom_up(FILE *f){
	/** 1. Create stack */
	//Static allocation
	PA_stack stack;
	/** 2. Init the stack */
	PA_stack_init(&stack);
	/** 3. Push $ sign on the stack */
	/** 3.1 Generate $ token */
	PA_item_t item,top_terminal, token_in, handle;
	item.terminal = generate_empty_token();
	/** 3.2 Push $ on the top of the stack */
	PA_stack_push(&stack, item, 1);

	int i=0;
	int accepted = 0;
	int reduction = 0;
	do{
		printf("Run %d\n",i);
		/** 4. Get terminal from the top of the stack and from the input */
		//Stack top
		PA_stack_top_terminal(&stack,&top_terminal);
		//Token in
		if(!reduction){
			token_in.terminal = get_next_token(f);
			reduction = 0;
		}
		printf("Type of token:%d\n",token_in.terminal->type);
		/**5. Look the operator priority in the precedence table */
		switch(precedence_table[get_index(top_terminal.terminal->type)][get_index(token_in.terminal->type)]){
			case '<':
				printf("Tlacim na zasobnik\n\n");
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
				printf("Rovnaka priorita, pushni terminal na zasobnik.\n\n");
				PA_stack_push(&stack,token_in,1);
			reduction = 0;
				break;
			case '>': 
				//Pri redukcii chceme testovat podmienku
				//Ale nechceme nacitat dalsi tokeny, pretoze predchadzajuci nebol spracovany
				//Nechcem testovat ci je dno zasobniku prazdne lebo sa tam vyskytuje neterminal
				printf("Redukujem\n\n");
				if(!reduce_terminal(&stack)){
					printf("Error: No rule for reduction\n");
					return 0;
				}
				
				reduction = 1;
				PA_stack_top_terminal(&stack,&top_terminal);
				if((top_terminal.terminal->type == TOKEN_EOF) && (token_in.terminal->type == TOKEN_EOF)){
						accepted = 1;
				}
				break;
			case ERR: printf("Chyba\n\n");return 1;break;//Dealloc the stack
		}
	i++;
	printf("Type of token:%d\n",token_in.terminal->type);
	PA_stack_top(&stack,&item);
	printf("Vrchol zasobnika: %d\n", item.item_type);
	printf("---------------------------------------------\n");
	}while(((top_terminal.terminal->type != TOKEN_EOF) || (token_in.terminal->type != TOKEN_EOF)) && (!accepted));
	
	/** Check if the PA was successful */
	if( top_terminal.terminal->type != TOKEN_EOF ){
		printf("Chyba! stack is not empty \n");//Dealloc the stack
		return 0;
	}
	if((top_terminal.terminal->type == TOKEN_EOF) && (token_in.terminal->type == TOKEN_EOF)){
		printf("USPECH, PLATNY VYRAZ\n");
		destroy_token(top_terminal.terminal);
		destroy_token(token_in.terminal);
	}
	return 1; 	
}

int get_index(int token){
	switch(token){
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
		//TODO Add funcion control, check the symtable
		//case TOKEN_ID		: return 16;
		case TOKEN_COLON	: return 17;
		case TOKEN_EOF		: return 18;
	};
   return -1;	
}
