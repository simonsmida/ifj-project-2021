
// TODO code_gen.h move to the include file
#include "code_generator.h"

void generate_built_in_write(bool is_global, token_t *token){
	
	if ( token->type == TOKEN_ID ){
		// We have to make sure to not start printing WRITE if the token string is "write"
		if (!strcmp(token->attribute->string, "write")){
			// do nothing
			return;
		}
		else if (is_global){
			printf("WRITE GF@%s\n", token->attribute->string);
		}
		else {
			printf("WRITE LF@%s\n", token->attribute->string);
		}
	}
	else if (token->type == TOKEN_STR_LIT){
		printf("WRITE string@%s\n", token->attribute->string);
	}
	else if (token->type == TOKEN_INT_LIT){
		printf("WRITE int@%d\n", token->attribute->integer);
	}
	else if (token->type == TOKEN_NUM_LIT){
		printf("WRITE num@%a\n", token->attribute->number);
	}

	return;
}

void generate_head(){
	printf(".IFJcode21\n");
	printf("DEFVAR GF@temp1\n");
	printf("DEFVAR GF@temp2\n");
	printf("DEFVAR GF@temp3\n\n");
	printf("\nJUMP $MAIN\n");
	return;
}

void generate_built_in_reads(){
	printf("LABEL READS\n");
	printf("READ GF@temp1 string\n");
	printf("RETURN\n\n");

	return;
}

void generate_built_in_readi(){
	printf("LABEL READI\n");
	printf("READ GF@temp1 int\n");
	printf("RETURN\n\n");

	return;
}

void generate_built_in_readn(){
	printf("LABEL READN\n");
	printf("READ GF@temp1 float\n");
	printf("RETURN\n\n");

	return;
}

void generate_built_in_functions(){
	generate_built_in_readi();
	generate_built_in_reads();
	generate_built_in_readn();
	generate_built_in_substr();
	generate_built_in_ord();
	generate_built_in_chr();

	return;
}

void generate_main(){
	printf("$MAIN\n");

	return;
}

void generate_end(){
	printf("LABEL $END\n");
	printf("EXIT int@0\n");

	return;
}

void generate_built_in_substr(){
	// substr ( string, int_start, int_stop   )
	printf("LABEL SUBSTR\n");
	printf("CREATEFRAME\n");
	printf("PUSHFRAME\n");
	printf("DEFVAR LF@temp_char\n");
	printf("DEFVAR LF@ret_string\n");
	printf("MOVE LF@ret_string string@\n");
	printf("DEFVAR LF@str_len\n");
	printf("DEFVAR LF@bool_var\n");
	printf("MOVE LF@bool_var bool@true\n");
	printf("POPS GF@temp3\n"); // This is the stop byte
	printf("POPS GF@temp2\n"); // This is start byte
	printf("POPS GF@temp1\n"); // This is the string
	printf("SUB GF@temp2 GF@temp2 int@1\n");
	printf("SUB GF@temp3 GF@temp3 int@1\n");
	printf("STRLEN LF@str_len GF@temp1\n");
	// Check if j > strlen
	printf("CLEARS\n");

	printf("GT LF@bool_var LF@str_len GF@temp3\n");
	printf("JUMPIFEQ RETURN_EMPTY LF@bool_var bool@false\n\n");

	printf("GT LF@bool_var GF@temp2 GF@temp3\n");

	printf("JUMPIFEQ RETURN_EMPTY LF@bool_var bool@true\n\n");
	printf("JUMP REPEAT_OK\n\n");



	printf("LABEL REPEAT_OK\n");
	printf("GETCHAR LF@temp_char GF@temp1 GF@temp2\n");
	printf("CONCAT LF@ret_string LF@ret_string LF@temp_char\n");
	printf("ADD GF@temp2 GF@temp2 int@1\n");

	printf("GT LF@bool_var GF@temp2 GF@temp3\n");

	printf("JUMPIFEQ END_OK LF@bool_var bool@true\n");
	printf("JUMP REPEAT_OK\n\n");

	printf("LABEL END_OK\n");
	printf("CLEARS\n");
	printf("MOVE GF@temp1 LF@ret_string\n");
	printf("POPFRAME\n");
	printf("RETURN\n\n"); // idk

	printf("LABEL RETURN_EMPTY\n");
	printf("CLEARS\n");
	printf("POPFRAME\n");
	printf("MOVE GF@temp1 string@\n");
	printf("RETURN\n\n"); // idk

}

void generate_built_in_ord(){
	// ord ( string, int ) : int
	// Vrati ordinalnu hodnotu ASCII znaku stringu na pozicii int
	//  TODO Dat sem straze
	printf("LABEL ORD\n");
	printf("POPS GF@temp2\n"); // Toto bude ten int
	printf("POPS GF@temp1\n"); // Toto bude string
	printf("SUB GF@temp2 GF@temp2 int@1\n");
	printf("STRI2INT GF@temp3 GF@temp1 GF@temp2\n");
	printf("RETURN \n\n");
	
	return;
}

void generate_built_in_chr(){
	// chr ( int ) : string
	printf("LABEL CHR\n");
	printf("POPS GF@temp2\n");
	printf("INT2CHAR GF@temp1 GF@temp2\n");
	printf("RETURN\n");

	return;
}


void generate_function_label(const char *func_name){
	printf("LABEL %s\n", func_name);
	printf("CREATEFRAME \n");
	printf("PUSHFRAME \n");

	return;
}

void generate_var_declaration(const char *var_name, data_type_t data_type){
	printf("DEFVAR LF@%s\n", var_name);
	printf("MOVE LF@%s ", var_name);

	switch( data_type ){
		DTYPE_INT:
			printf("int@0\n");
			break;
		DTYPE_NIL:
			printf("nil@nil\n");
			break;
		DTYPE_STRING:
			printf("string@\n");
			break;
		DTYPE_NUMBER:
			printf("float@0.0\n");
			break;
		default:
			break;
	} // switch

	return;
}

void generate_function_call(const char *func_name){
	printf("CALL %s\n", func_name);

	return;
}

void generate_assign_value_to_var(const char *var_name, ){

	return;
}