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

int analyze_bottom_up(FILE *f){
	/** 1. Create stack */
	//Static allocation
	PA_stack stack;
	/** 2. Init the stack */
	PA_stack_init(&stack);
	/** 3. Push $ sign on the stack */
	/** 3.1 Generate $ token */
	PA_item_t item,top_terminal,token_in;
	item.terminal = generate_empty_token();
	/** 3.2 Push $ on the top of the stack */
	PA_stack_push(&stack, item, 1);
	PA_stack_top_terminal(&stack,&top_terminal);
	//printf("Terminal type: %d\n\n",top_terminal.terminal->type);
#if 0	
	for(int i = 0; i < 4; i++){
		token_in.terminal = get_next_token(f);
		PA_stack_push(&stack, token_in, 1);
		//PA_stack_top_terminal(&stack,&top_terminal);
	}
	for(int i = 0; i < 5; i++){
		PA_stack_top_terminal(&stack,&top_terminal);
		printf("Terminal type: %d\n",top_terminal.terminal->type);
		if (top_terminal.terminal->attribute->string != NULL){
			printf("Terminal name: %s\n",top_terminal.terminal->attribute->string);
		}else{printf("Terminal name: Nema :(\n");}
		PA_stack_pop(&stack);
	}
#endif 

#if 1
	int i = 0;
	int buffer_token = 0;
	do{
		printf("%d. run\n",i);
		/** 4. Get terminal from the top of the stack and from the input */
		PA_stack_top_terminal(&stack,&top_terminal);
		//Stack top
		printf("%d. Vrchol zasobnika:\n----------------\n",i);
		printf("Terminal type: %d\n",top_terminal.terminal->type);
		if (top_terminal.terminal->attribute->string != NULL){
			printf("Terminal name: %s\n",top_terminal.terminal->attribute->string);
		}else{printf("Terminal name: Nema :(\n");}
		
		//Token in
		if(!buffer_token){
			token_in.terminal = get_next_token(f);
			buffer_token = 0;
		}
		printf("--------------------\n%d. Vstupny terminal:\n",i);
		printf("Terminal type: %d\n",token_in.terminal->type);
		if (token_in.terminal->attribute->string != NULL){
			printf("Terminal name: %s\n",token_in.terminal->attribute->string);
		}else{printf("Terminal name: Nema :(\n");}
		/**5. Look the operator priority in the precedence table */
		switch(precedence_table[get_index(top_terminal.terminal->type)][get_index(token_in.terminal->type)]){
			case '<': printf("Tlacim na zasobnik\n\n");PA_stack_push(&stack,token_in,1);break;
			case '=': 
				  printf("Rovnaka priorita\n\n");
				  PA_stack_push(&stack,token_in,1);
				  break;
			case '>': 
				  printf("Redukujem\n\n");
				  buffer_token = 1;
				  PA_stack_top_terminal(&stack,&top_terminal);
				  //Pozriet sa do pravidiel -> vysledok zredukovany neterminal
				  PA_stack_pop(&stack);
				  top_terminal.non_terminal.expr_type = EXPR;
			  	  PA_stack_push(&stack,top_terminal,0);
				  printf("Redukujem\n\n");
				  break;
			case ERR: printf("Chyba\n\n");break;
		}
		i++;
		if(buffer_token){
			continue;
		}
		PA_stack_top_terminal(&stack,&top_terminal);
	}while((token_in.terminal->type != TOKEN_EOF) && (top_terminal.terminal->type != TOKEN_EOF));
#endif
   	if(token_in.terminal != NULL){
		destroy_token(token_in.terminal);
	}
	PA_stack_destroy(&stack);
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
