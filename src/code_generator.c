
// TODO code_gen.h move to the include file
#include "include/code_generator.h"
#include "include/parser.h"
#include<stdarg.h>

#if TESTING
void CODE(const char *fmt, ...) {
   	va_list args;
   	va_start(args, fmt);
	FILE *out = fopen("output.txt","wa");
	if(out == NULL){
		printf("drblo\n");
	}
   	fprintf(out,fmt, args);
   	va_end(args);
   	fprintf(out,"\n");
	fclose(out);
}

void generate_built_in_write( token_t *token, char *function_id, int depth, int *array_depth){
	
	if ( token->type == TOKEN_ID ){
		// We have to make sure to not start printing WRITE if the token string is "write"
		if (!strcmp(token->attribute->string, "write")){
			// do nothing
			return;
		}
		else {
			CODE("WRITE LF@%s$%s$%d$%d", token->attribute->string, function_id, depth, array_depth[depth]);
		}
	}
	else if (token->type == TOKEN_STR_LIT){
		CODE("WRITE string@%s", token->attribute->string);
	}
	else if (token->type == TOKEN_INT_LIT){
		CODE("WRITE int@%d", token->attribute->integer);
	}
	else if (token->type == TOKEN_NUM_LIT){
		CODE("WRITE float@%a", token->attribute->number);
	}
}

void generate_head(){
	CODE(".IFJcode21");
	CODE("DEFVAR GF@tmp1");
	CODE("DEFVAR GF@tmp2");
	CODE("DEFVAR GF@tmp3");
	CODE("DEFVAR GF@tmp4");
	CODE("DEFVAR GF@tmp5");
	CODE("\nCALL $MAIN");
	CODE("EXIT int@0");

	return;
}

void generate_built_in_reads(){
	CODE("LABEL READS");
	CODE("READ GF@tmp1 string");
	CODE("RETURN\n");
}

void generate_built_in_readi(){
	CODE("LABEL READI");
	CODE("READ GF@tmp1 int");
	CODE("RETURN\n");
}

void generate_built_in_readn(){
	CODE("LABEL READN");
	CODE("READ GF@tmp1 float");
	CODE("RETURN\n");
}

void generate_built_in_functions(){
	check_nil_builtin();
	generate_built_in_readi();
	generate_built_in_reads();
	generate_built_in_readn();
	generate_built_in_substr();
	generate_built_in_ord();
	generate_built_in_chr();
	generate_if_body();
}

void generate_main(){
	CODE("LABEL $MAIN");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME");
}

void generate_end(){
	CODE("LABEL $END");
	CODE("CLEARS");
	CODE("POPFRAME");
	CODE("EXIT int@0\n");
}
//TODO: check_nil do buitin fukncii

void check_nil_builtin(){

	CODE("LABEL $checknil");
	

	CODE("POPS GF@tmp4");

	CODE("TYPE GF@tmp5 GF@tmp4");
	CODE("EQ GF@tmp5 GF@tmp5 string@nil");

	CODE("JUMPIFEQ $nnil GF@tmp5 bool@false");

	CODE("EXIT int@8");

	CODE("LABEL $nnil");
	CODE("PUSHS GF@tmp4");
	CODE("RETURN");
}

void generate_built_in_substr(){
	// substr ( string, int_start, int_stop )
	CODE("LABEL SUBSTR");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME");
	CODE("DEFVAR LF@tmp_char");
	CODE("DEFVAR LF@ret_string");
	CODE("MOVE LF@ret_string string@");
	CODE("DEFVAR LF@str_len");
	CODE("DEFVAR LF@bool_var");
	CODE("MOVE LF@bool_var bool@true");

	CODE("CALL $checknil"); 
	CODE("POPS GF@tmp3");		// This is the stop byte

	CODE("CALL $checknil"); 	
	CODE("POPS GF@tmp2");		// This is start byte

	CODE("CALL $checnil"); 		
	CODE("POPS GF@tmp1"); 		// This is the string

	CODE("SUB GF@tmp2 GF@tmp2 int@1");
	CODE("SUB GF@tmp3 GF@tmp3 int@1");
	CODE("STRLEN LF@str_len GF@tmp1");
	// Check if j > strlen
	CODE("CLEARS");

	CODE("GT LF@bool_var LF@str_len GF@tmp3");
	CODE("JUMPIFEQ RETURN_EMPTY LF@bool_var bool@false\n");

	CODE("GT LF@bool_var GF@tmp2 GF@tmp3");

	CODE("JUMPIFEQ RETURN_EMPTY LF@bool_var bool@true\n");
	CODE("JUMP REPEAT_OK\n");



	CODE("LABEL REPEAT_OK");
	CODE("GETCHAR LF@tmp_char GF@tmp1 GF@tmp2");
	CODE("CONCAT LF@ret_string LF@ret_string LF@tmp_char");
	CODE("ADD GF@tmp2 GF@tmp2 int@1");

	CODE("GT LF@bool_var GF@tmp2 GF@tmp3");

	CODE("JUMPIFEQ END_OK LF@bool_var bool@true");
	CODE("JUMP REPEAT_OK\n");

	CODE("LABEL END_OK");
	CODE("CLEARS");
	CODE("MOVE GF@tmp1 LF@ret_string");
	CODE("POPFRAME");
	CODE("RETURN\n"); // idk

	CODE("LABEL RETURN_EMPTY");
	CODE("CLEARS");
	CODE("POPFRAME");
	CODE("MOVE GF@tmp1 string@");
	CODE("RETURN\n"); // idk

}

void generate_built_in_ord(){
	// ord ( string, int ) : int
	// Vrati ordinalnu hodnotu ASCII znaku stringu na pozicii int
	//  TODO Dat sem straze
	CODE("LABEL ORD");
	CODE("CALL $checknil");
	CODE("POPS GF@tmp2");
	CODE("CALL $checknil"); 	// Toto bude ten int
	CODE("POPS GF@tmp1"); 	// Toto bude string
	CODE("SUB GF@tmp2 GF@tmp2 int@1");
	CODE("STRI2INT GF@tmp3 GF@tmp1 GF@tmp2");
	CODE("RETURN\n");
}

void generate_built_in_chr(){
	// chr ( int ) : string
	CODE("LABEL CHR");
	CODE("CALL $checknil");
	CODE("POPS GF@tmp2");
	CODE("INT2CHAR GF@tmp1 GF@tmp2");
	CODE("RETURN\n");
}

void generate_function_end(){
	CODE("POPFRAME");
	CODE("RETURN\n");
}

void generate_function_label(const char *func_name){
	CODE("LABEL %s", func_name);
	CODE("PUSHFRAME ");
}

void generate_var_declaration(char *var_name, char *function_id, int *array_depth,  int depth){
	//printf("DEFVAR LF@%s$%s$%d$%d\n", var_name, function_id, depth, array_depth[depth]);

	return;
	
}

void generate_function_call(const char *func_name){
	CODE("CALL %s", func_name);
}

// void generate_assign_value_to_var(const char *var_name, data_type_t data_type, data_type_value_t value ){

// 	CODE("MOVE LF@%s ", var_name);
	
// 	switch( data_type ){
// 		case DTYPE_INT:
// 			CODE("int@%d", value.integer);
// 			break;
// 		case DTYPE_NUMBER:
// 			CODE("float@%f", value.number);
// 			break;
// 		case DTYPE_STRING:
// 			CODE("string@%s", value.string);
// 			break;
// 		case DTYPE_NIL:
// 			CODE("nil@nil");
// 			break;
// 		default:
// 			break;
// 	}

// 	return;
// }

void generate_assign_var_to_var(const char *var_to_be_assigned_to, const char *var_to_be_assigned){
	CODE("MOVE LF@%s LF@%s ", var_to_be_assigned_to, var_to_be_assigned);
	return;
}

void generate_return_from_function(){
	CODE("POPFRAME");
	CODE("RETURN\n");

	return;
}

void generate_main_label(){
	CODE("LABEL $MAIN");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME \n");
}

void generate_function_param( char *param_id, data_type_t data_type ){

	CODE("DEFVAR LF@%s", param_id);
	
	CODE("");
}

void generate_createframe(){
	//printf("CREATEFRAME\n");

	return;
}

void generate_pass_param_to_function(token_t *token,  char *function_name, int depth, int *array_depth, int param_index){
	
	// printf("DEFVAR TF@%c%d\n", '%', param_index);
	// printf("MOVE TF@%c%d ", '%', param_index);
	// if (token != NULL){
	// 	switch ( token->type ){
	// 		case TOKEN_INT_LIT:

	// 			printf("int@%d\n", token->attribute->integer);
	// 			break;
	// 		case TOKEN_NUM_LIT  :
	// 			printf("number@%f\n", token->attribute->number);
	// 			break;
	// 		case TOKEN_STR_LIT:
	// 			printf("string@%s\n", token->attribute->string);
	// 			break;
	// 		case TOKEN_ID:
	// 			printf("LF@%s\n", token->attribute->string);
	// 			break;
		
	// 		default:
	// 			break;
	// 	}
	// }

	// return;
}



void generate_pass_param_to_operation(token_t *token, char *function_name, int depth, int *array_depth){
	if (token != NULL){
		switch ( token->type ){
			case TOKEN_INT_LIT:
				CODE("PUSHS int@%d", token->attribute->integer);
				break;
			case TOKEN_NUM_LIT  :
				CODE("PUSHS number@%f", token->attribute->number);
				break;
			case TOKEN_STR_LIT:
				CODE("PUSHS string@%s", token->attribute->string);
				break;
			case TOKEN_ID:
				CODE("PUSHS LF@%s$%s$%d$%d", token->attribute->string, function_name, depth, array_depth[depth]);
				break;
			default:
				break;
		}
	}
}

void generate_return_params(token_t *token, int param_index){

}



void check_nil_op(){

	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("POPS GF@tmp2");

	CODE("TYPE GF@tmp3 GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp3 string@nil");

	CODE("TYPE GF@tmp4 GF@tmp2");
	CODE("EQ GF@tmp4 GF@tmp4 string@nil");

	CODE("OR GF@tmp3 GF@tmp3 GF@tmp4");

	CODE("JUMPIFEQ $nnil$%d GF@tmp3 bool@false", i);

	CODE("EXIT int@8");

	CODE("LABEL $nnil$%d", i);			//POZOR MUSI BYT UNIKATNE
	CODE("PUSHS GF@tmp2");
	CODE("PUSHS GF@tmp1");
	i++;	
}

void generate_strlen(){
	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("TYPE GF@tmp3 GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp3 string@nil");
	CODE("JUMPIFEQ $nnilStrlen$%d GF@tmp3 bool@false", i);
	CODE("EXIT int@8");
	CODE("LABEL $nnilStrlen$%d", i);
	CODE("STRLEN GF@tmp2 GF@tmp1");
	CODE("PUSHS GF@tmp2");
	i++;
}

void generate_div(){
	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp1 float@0x0.0p+0");
	CODE("JUMPIFEQ $notZeroDiv$%d GF@tmp3 bool@false", i); //POZOR!!! NAVESTI NOTZERO MUSI BYT UNIKATNE
	CODE("EXIT int@9");
	CODE("LABEL $notZeroDiv$%d", i);
	CODE("PUSHS GF@tmp1");
	CODE("DIVS");
	i++;
}

void generate_idiv(){
	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp1 int@0");
	CODE("JUMPIFEQ $notZeroIdiv$%d GF@tmp3 bool@false", i); //POZOR!!! NAVESTI NOTZERO MUSI BYT UNIKATNE
	CODE("EXIT int@9");
	CODE("LABEL $notZeroIdiv$%d", i);
	CODE("PUSHS GF@tmp1");
	CODE("IDIVS");
	i++;
}

void generate_stack_operation(token_t *token){
	if((token->type != TOKEN_STRLEN) && (token->type != TOKEN_EQ) && (token->type != TOKEN_NOT_EQ))
		check_nil_op();

	if (token != NULL){
		switch(token->type){
			case TOKEN_PLUS:
				CODE("ADDS");
				break;
			case TOKEN_MINUS:
				CODE("SUBS");
				break;
			case TOKEN_MUL:
				CODE("MULS");
				break;
			case TOKEN_DIV:
				generate_div();
				break;
			case TOKEN_INT_DIV:
				generate_idiv();
				break;
			case TOKEN_LT:
				CODE("LTS");
				break;
			case TOKEN_LE:
				CODE("POPS GF@tmp1");
				CODE("POPS GF@tmp2");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("EQS");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("LTS");
				CODE("ORS");
				break;
			case TOKEN_EQ:
				CODE("EQS");
				break;
			case TOKEN_GE:
				CODE("POPS GF@tmp1");
				CODE("POPS GF@tmp2");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("EQS");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("GTS");
				CODE("ORS");
				break;
			case TOKEN_GT:
				CODE("GTS");
				break;
			case TOKEN_NOT_EQ:
				CODE("EQS");
				CODE("NOTS");
				break;
			case TOKEN_CONCAT:
				CODE("POPS GF@tmp1");
				CODE("POPS GF@tmp2");
				CODE("CONCAT GF@tmp3 GF@tmp2 GF@tmp1");
				CODE("PUSHS GF@tmp3");
				break;
			case TOKEN_STRLEN:
				generate_strlen();
				break;
			default:
				break;
		}
	}
}

void generate_type_conversion(int op){//konverzia bude vzdy len z int na number???

	//a + b ... a is the first operand, b is the second operand
	//converting a 
	check_nil_op();
	if(op == 1){													
		CODE("POPS GF@tmp2");	// 	 stack 
		CODE("POPS GF@tmp1");	//   +----------+
		CODE("PUSHS GF@tmp1");	//   |    b     | 
		CODE("INT2FLOATS");		//   +----------+
		CODE("PUSHS GF@tmp2");	//   |    a     |
								//   +----------+ 
	}
	else{						//converting b
		CODE("INT2FLOATS");
	}	
}

void generate_label_if(char *function_name, int *pole_zanoreni, int depth){ //zavola sa ked narazis na "if" ----> "if"(expr)
	CODE("CALL $if");
	CODE("JUMPIFEQ %s$else$%d$%d GF@tmp1 bool@true", function_name, depth, pole_zanoreni[depth]);
}

void generate_label_else(char *function_name, int *pole_zanoreni, int depth){
	CODE("JUMP %s$if_end$%d$%d",function_name, depth, pole_zanoreni[depth]);
	CODE("LABEL %s$else$%d$%d\n", function_name, depth, pole_zanoreni[depth]);

}

void generate_label_if_end(char *function_name, int *pole_zanoreni, int depth){
	CODE("LABEL %s$if_end$%d$%d\n", function_name, depth, pole_zanoreni[depth]);

	return;
}

void generate_if_body(){//nazov labelov sa bude odvijat od nazvu funkcie a hlbky
	CODE("LABEL $if");
	CODE("MOVE GF@tmp1 bool@false");
	CODE("POPS GF@tmp2");
	CODE("PUSHS GF@tmp2");
	CODE("EQ GF@tmp1 GF@tmp1 GF@tmp2");
	CODE("RETURN");
}

void generate_pop_stack_to_var(char *var_id, char *function_id, int *array_depth, int depth){
	CODE("POPS LF@%s$%s$%d$%d", var_id, function_id, depth, array_depth[depth]);

	return;
}

void generate_start_of_program(){
	CODE("LABEL $MAIN");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME\n");
}


void generate_var_declaration_function(char *var_name, char *function_id, int depth, int *array_depth, int num_param){
	// printf("DEFVAR LF@%s$%s$%d$%d\n", var_name, function_id, depth, array_depth[depth]);
	// printf("MOVE LF@%s$%s$%d$%d LF@%c%d\n", var_name, function_id, depth, array_depth[depth], '%',  num_param);

	return;
}



void generate_while_repeat_label(char *func_id, int depth, int *array_depth){
	// printf("JUMP %s$define_vars$%d$%d\n",  func_id, depth, array_depth[depth]);
	// printf("LABEL %s$while$%d$%d\n", func_id, depth, array_depth[depth]);

	return;
}

void generate_while_end_label(char *func_id, int depth, int *array_depth, string_t *buffer){
	// printf("JUMP %s$while$%d$%d\n",  func_id, depth, array_depth[depth]);
	// printf("LABEL %s$define_vars$%d$%d\n", func_id, depth, array_depth[depth]);
	// printf("%s", buffer->string);
	// printf("JUMP %s$while$%d$%d\n",  func_id, depth, array_depth[depth]);
	// printf("LABEL %s$while_end$%d$%d\n", func_id, depth, array_depth[depth] );
}


// Use this after then if inside_while == true 
void generate_jump_while_end(char *func_id, int depth, int *array_depth){
	// printf("CALL $if\n");
	// printf("JUMPIFEQ %s$while_end$%d$%d GF@tmp1 bool@false\n",func_id, depth, array_depth[depth] );

	return;
}

void generate_main_scope(){
	// printf("RETURN\n");
	// printf("LABEL $MAIN\n");
	// printf("CREATEFRAME\n");
	// printf("PUSHFRAME\n");

	return;

}
#endif

#if NORMAL
void CODE(const char *fmt, ...) {
   	va_list args;
   	va_start(args, fmt);
	//FILE *out = fopen("output.txt","w");
   	vprintf(fmt, args);
   	va_end(args);
   	printf("\n");
	//fclose(out);
}

void generate_built_in_write( token_t *token, char *function_id, int depth, int *array_depth){
	
	if ( token->type == TOKEN_ID ){
		// We have to make sure to not start printing WRITE if the token string is "write"
		if (!strcmp(token->attribute->string, "write")){
			// do nothing
			return;
		}
		else {
			CODE("WRITE LF@%s$%s$%d$%d", token->attribute->string, function_id, depth, array_depth[depth]);
		}
	}
	else if (token->type == TOKEN_STR_LIT){
		CODE("WRITE string@%s", token->attribute->string);
	}
	else if (token->type == TOKEN_INT_LIT){
		CODE("WRITE int@%d", token->attribute->integer);
	}
	else if (token->type == TOKEN_NUM_LIT){
		CODE("WRITE float@%a", token->attribute->number);
	}
}

void generate_head(){
	CODE(".IFJcode21");
	CODE("DEFVAR GF@tmp1");
	CODE("DEFVAR GF@tmp2");
	CODE("DEFVAR GF@tmp3");
	CODE("DEFVAR GF@tmp4");
	CODE("DEFVAR GF@tmp5");
	CODE("\nCALL $MAIN");
	CODE("EXIT int@0");

	return;
}

void generate_built_in_reads(){
	CODE("LABEL reads");
	CODE("READ GF@tmp1 string");
	CODE("PUSHS GF@tmp1");
	CODE("RETURN\n");
}

void generate_built_in_readi(){
	CODE("LABEL readi");
	CODE("READ GF@tmp1 int");
	CODE("PUSHS GF@tmp1");
	CODE("RETURN\n");
}

void generate_built_in_readn(){
	CODE("LABEL readn");
	CODE("READ GF@tmp1 float");
	CODE("PUSHS GF@tmp1");
	CODE("RETURN\n");
}

void generate_built_in_functions(){
	check_nil_builtin();
	generate_built_in_readi();
	generate_built_in_reads();
	generate_built_in_readn();
	generate_built_in_substr();
	generate_built_in_ord();
	generate_built_in_chr();
	generate_if_body();
}

void generate_main(){
	CODE("LABEL $MAIN");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME");
}

void generate_end(){
	CODE("LABEL $END");
	CODE("CLEARS");
	CODE("POPFRAME");
	CODE("EXIT int@0\n");
}
//TODO: check_nil do buitin fukncii

void check_nil_builtin(){

	CODE("LABEL $checknil");
	

	CODE("POPS GF@tmp4");

	CODE("TYPE GF@tmp5 GF@tmp4");
	CODE("EQ GF@tmp5 GF@tmp5 string@nil");

	CODE("JUMPIFEQ $nnil GF@tmp5 bool@false");

	CODE("EXIT int@8");

	CODE("LABEL $nnil");
	CODE("PUSHS GF@tmp4");
	CODE("RETURN");
}

void generate_built_in_substr(){
	// substr ( string, int_start, int_stop )
	CODE("LABEL SUBSTR");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME");
	CODE("DEFVAR LF@tmp_char");
	CODE("DEFVAR LF@ret_string");
	CODE("MOVE LF@ret_string string@");
	CODE("DEFVAR LF@str_len");
	CODE("DEFVAR LF@bool_var");
	CODE("MOVE LF@bool_var bool@true");

	CODE("CALL $checknil"); 
	CODE("POPS GF@tmp3");		// This is the stop byte

	CODE("CALL $checknil"); 	
	CODE("POPS GF@tmp2");		// This is start byte

	CODE("CALL $checnil"); 		
	CODE("POPS GF@tmp1"); 		// This is the string

	CODE("SUB GF@tmp2 GF@tmp2 int@1");
	CODE("SUB GF@tmp3 GF@tmp3 int@1");
	CODE("STRLEN LF@str_len GF@tmp1");
	// Check if j > strlen
	CODE("CLEARS");

	CODE("GT LF@bool_var LF@str_len GF@tmp3");
	CODE("JUMPIFEQ RETURN_EMPTY LF@bool_var bool@false\n");

	CODE("GT LF@bool_var GF@tmp2 GF@tmp3");

	CODE("JUMPIFEQ RETURN_EMPTY LF@bool_var bool@true\n");
	CODE("JUMP REPEAT_OK\n");



	CODE("LABEL REPEAT_OK");
	CODE("GETCHAR LF@tmp_char GF@tmp1 GF@tmp2");
	CODE("CONCAT LF@ret_string LF@ret_string LF@tmp_char");
	CODE("ADD GF@tmp2 GF@tmp2 int@1");

	CODE("GT LF@bool_var GF@tmp2 GF@tmp3");

	CODE("JUMPIFEQ END_OK LF@bool_var bool@true");
	CODE("JUMP REPEAT_OK\n");

	CODE("LABEL END_OK");
	CODE("CLEARS");
	CODE("MOVE GF@tmp1 LF@ret_string");
	CODE("POPFRAME");
	CODE("RETURN\n"); // idk

	CODE("LABEL RETURN_EMPTY");
	CODE("CLEARS");
	CODE("POPFRAME");
	CODE("MOVE GF@tmp1 string@");
	CODE("RETURN\n"); // idk

}

void generate_built_in_ord(){
	// ord ( string, int ) : int
	// Vrati ordinalnu hodnotu ASCII znaku stringu na pozicii int
	//  TODO Dat sem straze
	CODE("LABEL ORD");
	CODE("CALL $checknil");
	CODE("POPS GF@tmp2");
	CODE("CALL $checknil"); 	// Toto bude ten int
	CODE("POPS GF@tmp1"); 	// Toto bude string
	CODE("SUB GF@tmp2 GF@tmp2 int@1");
	CODE("STRI2INT GF@tmp3 GF@tmp1 GF@tmp2");
	CODE("RETURN\n");
}

void generate_built_in_chr(){
	// chr ( int ) : string
	CODE("LABEL CHR");
	CODE("CALL $checknil");
	CODE("POPS GF@tmp2");
	CODE("INT2CHAR GF@tmp1 GF@tmp2");
	CODE("RETURN\n");
}

void generate_function_end(){
	CODE("POPFRAME");
	CODE("RETURN\n");
}

void generate_function_label(const char *func_name){
	CODE("LABEL %s", func_name);
	CODE("PUSHFRAME ");
}

void generate_var_declaration(char *var_name, char *function_id, int *array_depth,  int depth){
	printf("DEFVAR LF@%s$%s$%d$%d\n", var_name, function_id, depth, array_depth[depth]);
	printf("MOVE LF@%s$%s$%d$%d nil@nil\n", var_name, function_id, depth, array_depth[depth]);

	return;
	
}

void generate_function_call(const char *func_name){
	CODE("CALL %s", func_name);
}

// void generate_assign_value_to_var(const char *var_name, data_type_t data_type, data_type_value_t value ){

// 	CODE("MOVE LF@%s ", var_name);
	
// 	switch( data_type ){
// 		case DTYPE_INT:
// 			CODE("int@%d", value.integer);
// 			break;
// 		case DTYPE_NUMBER:
// 			CODE("float@%f", value.number);
// 			break;
// 		case DTYPE_STRING:
// 			CODE("string@%s", value.string);
// 			break;
// 		case DTYPE_NIL:
// 			CODE("nil@nil");
// 			break;
// 		default:
// 			break;
// 	}

// 	return;
// }

void generate_assign_var_to_var(const char *var_to_be_assigned_to, const char *var_to_be_assigned){
	CODE("MOVE LF@%s LF@%s ", var_to_be_assigned_to, var_to_be_assigned);
	return;
}

void generate_return_from_function(){
	CODE("POPFRAME");
	CODE("RETURN\n");

	return;
}

void generate_main_label(){
	CODE("LABEL $MAIN");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME \n");
}

void generate_function_param( char *param_id, data_type_t data_type ){

	CODE("DEFVAR LF@%s", param_id);
	
	CODE("");
}

void generate_createframe(){
	printf("CREATEFRAME\n");

	return;
}

void generate_pass_param_to_function(token_t *token,  char *function_name, int depth, int *array_depth, int param_index){
	
	printf("DEFVAR TF@%c%d\n", '%', param_index);
	printf("MOVE TF@%c%d ", '%', param_index);
	if (token != NULL){
		switch ( token->type ){
			case TOKEN_INT_LIT:

				printf("int@%d\n", token->attribute->integer);
				break;
			case TOKEN_NUM_LIT  :
				printf("float@%a\n", token->attribute->number);
				break;
			case TOKEN_STR_LIT:
				printf("string@%s\n", token->attribute->string);
				break;
			case TOKEN_ID:
				printf("LF@%s$%s$%d$%d\n", token->attribute->string, function_name, depth, array_depth[depth]);

				break;
		
			default:
				break;
		}
	}

	return;
}



void generate_pass_param_to_operation(token_t *token, char *function_name, int depth, int *array_depth){
	if (token != NULL){
		switch ( token->type ){
			case TOKEN_INT_LIT:
				CODE("PUSHS int@%d", token->attribute->integer);
				break;
			case TOKEN_NUM_LIT  :
				CODE("PUSHS float@%a", token->attribute->number);
				break;
			case TOKEN_STR_LIT:
				CODE("PUSHS string@%s", token->attribute->string);
				break;
			case TOKEN_ID:
				CODE("PUSHS LF@%s$%s$%d$%d", token->attribute->string, function_name, depth, array_depth[depth]);
				break;
			default:
				break;
		}
	}
}

void generate_return_params(token_t *token, int param_index){

}



void check_nil_op(){

	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("POPS GF@tmp2");

	CODE("TYPE GF@tmp3 GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp3 string@nil");

	CODE("TYPE GF@tmp4 GF@tmp2");
	CODE("EQ GF@tmp4 GF@tmp4 string@nil");

	CODE("OR GF@tmp3 GF@tmp3 GF@tmp4");

	CODE("JUMPIFEQ $nnil$%d GF@tmp3 bool@false", i);

	CODE("EXIT int@8");

	CODE("LABEL $nnil$%d", i);			//POZOR MUSI BYT UNIKATNE
	CODE("PUSHS GF@tmp2");
	CODE("PUSHS GF@tmp1");
	i++;	
}

void generate_strlen(){
	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("TYPE GF@tmp3 GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp3 string@nil");
	CODE("JUMPIFEQ $nnilStrlen$%d GF@tmp3 bool@false", i);
	CODE("EXIT int@8");
	CODE("LABEL $nnilStrlen$%d", i);
	CODE("STRLEN GF@tmp2 GF@tmp1");
	CODE("PUSHS GF@tmp2");
	i++;
}

void generate_div(){
	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp1 float@0x0.0p+0");
	CODE("JUMPIFEQ $notZeroDiv$%d GF@tmp3 bool@false", i); //POZOR!!! NAVESTI NOTZERO MUSI BYT UNIKATNE
	CODE("EXIT int@9");
	CODE("LABEL $notZeroDiv$%d", i);
	CODE("PUSHS GF@tmp1");
	CODE("DIVS");
	i++;
}

void generate_idiv(){
	static int i = 0;
	CODE("POPS GF@tmp1");
	CODE("EQ GF@tmp3 GF@tmp1 int@0");
	CODE("JUMPIFEQ $notZeroIdiv$%d GF@tmp3 bool@false", i); //POZOR!!! NAVESTI NOTZERO MUSI BYT UNIKATNE
	CODE("EXIT int@9");
	CODE("LABEL $notZeroIdiv$%d", i);
	CODE("PUSHS GF@tmp1");
	CODE("IDIVS");
	i++;
}

void generate_stack_operation(token_t *token){

	if (token != NULL){
		switch(token->type){
			case TOKEN_PLUS:
				check_nil_op();
				CODE("ADDS");
				break;
			case TOKEN_MINUS:
				check_nil_op();
				CODE("SUBS");
				break;
			case TOKEN_MUL:
				check_nil_op();
				CODE("MULS");
				break;
			case TOKEN_DIV:
				check_nil_op();
				generate_div();
				break;
			case TOKEN_INT_DIV:
				check_nil_op();
				generate_idiv();
				break;
			case TOKEN_LT:
				check_nil_op();
				CODE("LTS");
				break;
			case TOKEN_LE:
				check_nil_op();
				CODE("POPS GF@tmp1");
				CODE("POPS GF@tmp2");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("EQS");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("LTS");
				CODE("ORS");
				break;
			case TOKEN_EQ:
				CODE("EQS");
				break;
			case TOKEN_GE:
				check_nil_op();
				CODE("POPS GF@tmp1");
				CODE("POPS GF@tmp2");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("EQS");
				CODE("PUSHS GF@tmp2");
				CODE("PUSHS GF@tmp1");
				CODE("GTS");
				CODE("ORS");
				break;
			case TOKEN_GT:
				check_nil_op();
				CODE("GTS");
				break;
			case TOKEN_NOT_EQ:
				CODE("EQS");
				CODE("NOTS");
				break;
			case TOKEN_CONCAT:
				check_nil_op();
				CODE("POPS GF@tmp1");
				CODE("POPS GF@tmp2");
				CODE("CONCAT GF@tmp3 GF@tmp2 GF@tmp1");
				CODE("PUSHS GF@tmp3");
				break;
			case TOKEN_STRLEN:
				generate_strlen();
				break;
			default:
				break;
		}
	}
}

void generate_type_conversion(int op){//konverzia bude vzdy len z int na number???

	//a + b ... a is the first operand, b is the second operand
	//converting a 
 	check_nil_op();
	if(op == 1){													
		CODE("POPS GF@tmp2");	// 	 stack 
		CODE("POPS GF@tmp1");	//   +----------+
		CODE("PUSHS GF@tmp1");	//   |    b     | 
		CODE("INT2FLOATS");		//   +----------+
		CODE("PUSHS GF@tmp2");	//   |    a     |
								//   +----------+ 
	}
	else{	
							//converting b
		CODE("INT2FLOATS");
	}	
}

void generate_label_if(char *function_name, int *pole_zanoreni, int depth){ //zavola sa ked narazis na "if" ----> "if"(expr)
	CODE("CALL $if");
	CODE("JUMPIFEQ %s$else$%d$%d GF@tmp1 bool@true", function_name, depth, pole_zanoreni[depth]);
}

void generate_label_else(char *function_name, int *pole_zanoreni, int depth){
	CODE("JUMP %s$if_end$%d$%d",function_name, depth, pole_zanoreni[depth]);
	CODE("LABEL %s$else$%d$%d\n", function_name, depth, pole_zanoreni[depth]);

}

void generate_label_if_end(char *function_name, int *pole_zanoreni, int depth){
	CODE("LABEL %s$if_end$%d$%d\n", function_name, depth, pole_zanoreni[depth]);

	return;
}

void generate_if_body(){//nazov labelov sa bude odvijat od nazvu funkcie a hlbky
	CODE("LABEL $if");
	CODE("MOVE GF@tmp1 bool@false");
	CODE("POPS GF@tmp2");
	CODE("PUSHS GF@tmp2");
	CODE("EQ GF@tmp1 GF@tmp1 GF@tmp2");
	CODE("RETURN");
}

void generate_pop_stack_to_var(char *var_id, char *function_id, int *array_depth, int depth){
	CODE("POPS LF@%s$%s$%d$%d", var_id, function_id, depth, array_depth[depth]);

	return;
}

void generate_start_of_program(){
	CODE("LABEL $MAIN");
	CODE("CREATEFRAME");
	CODE("PUSHFRAME\n");
}


void generate_var_declaration_function(char *var_name, char *function_id, int depth, int *array_depth, int num_param){
	printf("DEFVAR LF@%s$%s$%d$%d\n", var_name, function_id, depth, array_depth[depth]);
	printf("MOVE LF@%s$%s$%d$%d LF@%c%d\n", var_name, function_id, depth, array_depth[depth], '%',  num_param);

	return;
}



void generate_while_repeat_label(char *func_id, int depth, int *array_depth){
	printf("JUMP %s$define_vars$%d$%d\n",  func_id, depth, array_depth[depth]);
	printf("LABEL %s$while$%d$%d\n", func_id, depth, array_depth[depth]);

	return;
}

void generate_while_end_label(char *func_id, int depth, int *array_depth, string_t *buffer){
	printf("JUMP %s$while$%d$%d\n",  func_id, depth, array_depth[depth]);
	printf("LABEL %s$define_vars$%d$%d\n", func_id, depth, array_depth[depth]);
	if (buffer->string != NULL){
		printf("%s", buffer->string);
	}
	printf("JUMP %s$while$%d$%d\n",  func_id, depth, array_depth[depth]);
	printf("LABEL %s$while_end$%d$%d\n", func_id, depth, array_depth[depth] );
}


// Use this after then if inside_while == true 
void generate_jump_while_end(char *func_id, int depth, int *array_depth){
	printf("CALL $if\n");
	printf("JUMPIFEQ %s$while_end$%d$%d GF@tmp1 bool@true\n",func_id, depth, array_depth[depth] );

	return;
}

void generate_main_scope(){
	printf("RETURN\n");
	printf("LABEL $MAIN\n");
	printf("CREATEFRAME\n");
	printf("PUSHFRAME\n");

	return;

}
#endif