#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "symtable.h"

void generate_built_in_write( token_t *token, char *function_id, int depth);

void generate_head();

void generate_built_in_reads();

void generate_built_in_readi();

void generate_built_in_readn();

void generate_built_in_functions();

void generate_end();

void generate_built_in_substr();

void generate_built_in_ord();

void generate_built_in_chr();

void generate_var_declaration(char *var_name, char *function_id, int *array_depth,  int depth);

void generate_function_label(const char *func_name);

void generate_function_call(const char *func_name);

void generate_var_declaration_function(char *var_name, char *function_id, int depth, int *array_depth, int num_param);

void generate_assign_value_to_var(const char *var_name, data_type_t data_type, data_type_value_t value );

void generate_assign_var_to_var(const char *var_to_be_assigned_to, const char *var_to_be_assigned);

void generate_function_param(); 

void generate_pass_param(token_t *token, int param_index);

void generate_pass_param_to_operation(token_t *token, char *function_name, int depth, int *array_depth);

void generate_pass_param_to_function(token_t *token, int param_index);

void generate_function_end ();

void generate_start_of_program();

void generate_createframe();

void generate_pop_stack_to_var(char *var_id, char *function_id, int *array_depth, int depth);

void check_nil_builtin();

void generate_label_if(char * function_name, int *pole_zanoreni, int depth);

void generate_label_else(char *function_name, int *pole_zanoreni, int depth);

void generate_label_if_end(char *function_name, int *pole_zanoreni, int depth);

void generate_if_body();

void generate_while_repeat_label(char *func_id, int depth, int *array_depth);

void generate_jump_while_end(char *func_id, int depth, int *array_depth);

void generate_while_end_label(char *func_id, int depth, int *array_depth, string_t *buffer);