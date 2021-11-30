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
	/*PA_stack_top(stack,&top_item);
	PA_stack_pop(stack);
	while(top_item.item_type != 2){
		PA_stack_top(stack,&items[operands_count]);
		PA_stack_pop(stack);
		operands_count++;
		PA_stack_top(stack,&top_item);
	}*/
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

	//Reduce terminal E -> id 
	if(operands_count == 1){
		//1.Check item type
		//2.Reduce
		destroy_token(items[operands_count-1].terminal);
		PA_item_t reduced_terminal;
		reduced_terminal.non_terminal.expr_type = EXPR;
		printf("Zredukoval som ID\n");
		PA_stack_push(stack,reduced_terminal,0);
		PA_stack_top(stack,&top_item);
		printf("Vrchol zasobnika: %d\n\n", top_item.item_type);
	}
	else if(operands_count == 3){
		//USE E->E+E
		PA_item_t reduced_terminal;
		if( (items[0].item_type == 0) &&
			(items[1].item_type == 1) &&
			(items[2].item_type == 0) ){
		
			printf("Zredukoval som vyraz\n");
			reduced_terminal.non_terminal.expr_type = EXPR;
			PA_stack_push(stack,reduced_terminal,0);
		}
	}
	return 1;
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

#if 0

	for (int i = 0; i < 4;i++)
	{	
		token_in.terminal = get_next_token(f);
		printf("Type of token:%d\n",token_in.terminal->type);
		destroy_token(token_in.terminal);
	}
		printf("TOKEN EOF %d\n",TOKEN_EOF);
	//Free empty token $
	PA_stack_top(&stack, &item);
	PA_stack_pop(&stack);
	if( item.item_type == 1 ){
		destroy_token(item.terminal);
	}
#endif 
#if 1
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
				/** 5.1 Push the initial char of the handle */
				printf("Tlacim na zasobnik\n\n");
				PA_stack_top(&stack,&item);
				handle.handle = '<';
				if( item.item_type == 1){
					PA_stack_push(&stack,handle,2);
					PA_stack_push(&stack,token_in,1);
				}else if (item.item_type == 0){
					PA_stack_pop(&stack);
					PA_stack_push(&stack,handle,2);
					PA_stack_push(&stack,item,0);
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
				reduce_terminal(&stack);
				reduction = 1;
				PA_stack_top_terminal(&stack,&top_terminal);
				if((top_terminal.terminal->type == TOKEN_EOF) && (token_in.terminal->type == TOKEN_EOF)){
					accepted = 1;
				}
				break;
			case ERR: printf("Chyba\n\n");return 1;break;//Dealloc the stack
			case END: printf("Accepted\n\n");//accepted = 1;
					  break;//Dealloc the stack
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
	}
	printf("handle %c\n",handle.handle);
	if((top_terminal.terminal->type == TOKEN_EOF) && (token_in.terminal->type == TOKEN_EOF)){
		printf("USPECH, PLATNY VYRAZ\n");
	}
#endif 
#if 0
	int buffer_token = 0;
	do{
		printf("%d. run\n",i);
		/** 4. Get terminal from the top of the stack and from the input */
		//Stack top
		PA_stack_top_terminal(&stack,&top_terminal);
		
		
		/////////////////////////////////////////////////////////////////////////
		printf("%d. Vrchol zasobnika:\n----------------\n",i);
		printf("Terminal type: %d\n",top_terminal.terminal->type);
		if (top_terminal.terminal->attribute->string != NULL){
			printf("Terminal name: %s\n",top_terminal.terminal->attribute->string);
		}else{printf("Terminal name: Nema :(\n");}
		/////////////////////////////////////////////////////////////////////////
		
		//Token in
		if(!buffer_token){
			token_in.terminal = get_next_token(f);
			buffer_token = 0;
		}
		
		/////////////////////////////////////////////////////////////////////////
		printf("--------------------\n%d. Vstupny terminal:\n",i);
		printf("Terminal type: %d\n",token_in.terminal->type);
		if (token_in.terminal->attribute->string != NULL){
			printf("Terminal name: %s\n",token_in.terminal->attribute->string);
		}else{printf("Terminal name: Nema :(\n");}
		/////////////////////////////////////////////////////////////////////////
		
		
		
		/**5. Look the operator priority in the precedence table */
		switch(precedence_table[get_index(top_terminal.terminal->type)][get_index(token_in.terminal->type)]){
			case '<':
				printf("Tlacim na zasobnik\n\n");
				
				
				PA_stack_push(&stack,token_in,1);
				break;
			case '=': 
				printf("Rovnaka priorita\n\n");
				
				PA_stack_push(&stack,token_in,1);
				break;
			case '>': 
				//Pri redukcii chceme testovat podmienku
				//Ale nechceme nacitat dalsi tokeny, pretoze predchadzajuci nebol spracovany
				//Nechcem testovat ci je dno zasobniku prazdne lebo sa tam vyskytuje neterminal
				printf("Redukujem\n\n");
				
				
				buffer_token = 1;
				break;
			case ERR: printf("Chyba\n\n");break;
		}
		i++;
		if(buffer_token){
			continue;
		}
		PA_stack_top_terminal(&stack,&top_terminal);
	}while((token_in.terminal->type != TOKEN_EOF )&& (top_terminal.terminal->type != TOKEN_EOF));
	if( top_terminal.terminal->type != TOKEN_EOF ){
		printf("Chyba! stack is not empty \n");
	}
#endif
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
		//TODO Add funcion control, check the symtable
		//case TOKEN_ID		: return 16;
		case TOKEN_COLON	: return 17;
		case TOKEN_EOF		: return 18;
	};
   return -1;	
}
