#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "error.h"
#include "parser.h"

bool is_term_type_valid(int term_type, int expected_type);
int dtype_token(parser_t *parser);
int dtype_keyword(int keyword_type);

int check_arg_count(parser_t *parser);
int check_arg_type_literal(parser_t *parser);
int check_arg_type_id(parser_t *parser);
int check_arg_previous_def(parser_t *parser);
int check_function_redeclaration(parser_t *parser);
int check_function_call(parser_t *parser);
int check_function_call_arg_count(parser_t *parser);
int check_arg_def(parser_t *parser);
int check_function_redefinition(parser_t *parser);
int check_param_redeclaration(parser_t *parser);
int check_param_mismatch(parser_t *parser, int param_index);
int check_param_count_mismatch(parser_t *parser);
int check_no_return_values(parser_t *parser);
int check_return_value_count(parser_t *parser, int ret_type_index);
int check_return_values(parser_t *parser, int ret_type_index);
int check_variable_redeclaration(parser_t *parser);
int check_undeclared_var_or_func(parser_t *parser, symtable_item_t *i);
int check_undefined_arg(parser_t *parser);
int check_invalid_variable_name(parser_t *parser);
int check_declared_function_defined(parser_t *parser);
int check_ret_val_count(parser_t *parser);

#endif
