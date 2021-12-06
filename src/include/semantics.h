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

#endif
