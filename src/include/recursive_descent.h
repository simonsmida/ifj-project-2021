#ifndef RECURSIVE_DESCENT_H
#define RECURSIVE_DESCENT_H

#include "parser.h"
#include "scanner.h"
#include "error.h"
#include "bottom_up_sa.h"
#include "PA_stack.h"
#include "code_generator.h"
#include "semantics.h"
#include "dll_list.h"

int prog(parser_t *parser);
int prolog(parser_t *parser);
int seq(parser_t *parser);
int func_dec(parser_t *parser);
int func_def(parser_t *parser);
int func_call(parser_t *parser);
int func_head(parser_t *parser);
int param_fdef(parser_t *parser);
int param_fdef_n(parser_t *parser);
int param_fdec(parser_t *parser);
int param_fdec_n(parser_t *parser);
int ret_type_list(parser_t *parser);
int ret_type_list_n(parser_t *parser);
int expr_list(parser_t *parser);
int stat_list(parser_t *parser);
int else_nt(parser_t *parser);
int var_def(parser_t *parser, char *id_name);
int arg(parser_t *parser);
int arg_n(parser_t *parser);
int term(parser_t *parser, int num_param);
int dtype(parser_t *parser);
int stat(parser_t *parser);
int id_n(parser_t *parser);

#endif
