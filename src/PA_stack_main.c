/** Main pre skusanie stack rozhrania */

#include <stdlib.h>
#include <stdio.h>
#include "include/PA_stack.h"
#include "include/scanner.h"

int main(){
	PA_stack zasobnicek;
	PA_stack_init(&zasobnicek);
	PA_item_t polozka;
	token_t* new_token;
	char* filename = "regex.txt";
	FILE *f = fopen(filename,"r");
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
		if(i == 1){
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
	
#if 1	
	for(int i=0; i<3;i++){
		printf("\nPopujem\n");
		PA_stack_top(&zasobnicek,&polozka);
		printf("Ziskany terminal\n");
		printf("Token type: %d\n", polozka.terminal->type);
		printf("Token name: %s\n",polozka.terminal->attribute->string);
		PA_stack_pop(&zasobnicek);
	}
#endif 
#if 0
	for(int i=0;i<3;i++){
		PA_stack_pop(&zasobnicek);
	}
#endif 
	fclose(f);
}
