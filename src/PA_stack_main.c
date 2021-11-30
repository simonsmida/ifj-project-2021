/** Main pre skusanie stack rozhrania */

#include <stdlib.h>
#include <stdio.h>
#include "include/PA_stack.h"
#include "include/scanner.h"
#include "include/bottom_up_sa.h"


int main(){
	char* filename = "regex.txt";
	FILE *f = fopen(filename,"r");
	parser_t parser;
	analyze_bottom_up(f,&parser);
	destroy_token(parser.token);
#if 0
	PA_item_t item;
	item.terminal = get_next_token(f);
	//item.item_type = 1;
	
	PA_stack stack;
	PA_stack_init(&stack);
	
	printf("Terminal type: %d\n",item.terminal->type);
	if (item.terminal->attribute->string != NULL){
		printf("Terminal name: %s\n",item.terminal->attribute->string);
	}else{printf("Terminal name: Nema :(\n");}

	PA_stack_push(&stack,item,1);
	
	PA_item_destroy(item);
	
	printf("Vypisujem zo zasobnika\n");
	PA_stack_top(&stack, &item);
	PA_stack_destroy(&stack);
	printf("Terminal type: %d\n",item.terminal->type);
	if (item.terminal->attribute->string != NULL){
		printf("Terminal name: %s\n",item.terminal->attribute->string);
	}else{printf("Terminal name: Nema :(\n");}
	
	PA_item_destroy(item);
#endif
#if 0	
		PA_stack zasobnicek;
		PA_stack_init(&zasobnicek);
		PA_item_t polozka;
		token_t* new_token;
		if ( f == NULL){
			fprintf(stderr,"Error while opening the file %s\n",filename);
		}

		//Push nonterminal -> the only difference is in the item.type	
		for(int i=0; i<3;i++){
			new_token = get_next_token(f);
			polozka.terminal = new_token;
			printf("\nZiskany terminal\n");
			printf("Token type: %d\n", polozka.terminal->type);
			printf("Token name: %s\n",polozka.terminal->attribute->string);
			if(i == 2){
				PA_stack_push(&zasobnicek, polozka,1);
			}else
				PA_stack_push(&zasobnicek, polozka,0);
		}
		
		int fail = PA_stack_top_terminal(&zasobnicek,&polozka);
		if(fail){
			printf("Ziaden terminal na zasovbniku\n");
		}
		printf("-------------------------------\n");
		printf("Terminal closest to the top of the stack\n");
		printf("Token type: %d\n", polozka.terminal->type);
		printf("Token name: %s\n",polozka.terminal->attribute->string);
		printf("-------------------------------\n");
		
		for(int i=0; i<3;i++){
			printf("\nPopujem\n");
			PA_stack_top(&zasobnicek,&polozka);
			printf("Ziskany terminal\n");
			printf("Token type: %d\n", polozka.terminal->type);
			printf("Token name: %s\n",polozka.terminal->attribute->string);
			PA_stack_pop(&zasobnicek);
		}
#endif
	fclose(f);
	return 0;
}
