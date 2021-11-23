/** Main pre skusanie stack rozhrania */

#include <stdlib.h>
#include <stdio.h>
#include "include/PA_stack.h"
#include "include/scanner.h"

int main(){
	PA_stack stack;
	PA_item_t polozka;
	PA_stack_init(&stack);
	skuska(&stack);
}
