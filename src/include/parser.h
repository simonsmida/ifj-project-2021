#ifndef PARSER_H
#define PARSER_H

#include "symtable.h"
#include "scanner.h"

typedef struct parser {
    symtable_t *global_symtable;
    symtable_item_t *curr_item;
    symtable_item_t *curr_func;
    symtable_item_t *curr_rhs;
    // list of tokens?
    // semantic analysis
    token_t *token;
    FILE *src;
    int curr_arg_count;
    int block_temp_id;
    int curr_block_id;
    int curr_block_depth;
    bool inside_func_def;
    bool inside_func_dec;
} parser_t;


/**
 * @brief Initialize parser structure
 */
parser_t *parser_init(FILE *src);

/**
 * @brief Start parsing source file
 * @param src input source file to be parsed
 */
int parser_parse(FILE *src);

/**
 * @brief Deallocate parser structure
 * @param parser structure
 */
void parser_destroy(parser_t *parser);


#define TOKEN_T parser->token->type
#define SYMTAB_G parser->global_symtable

// TODO: beware of null!
#define SYMTAB_L parser->curr_func->function->local_symtable
#define CURR_F_KEY parser->curr_func->key

#define CURR_FUNC parser->curr_func->function
#define FUNC_ITEM parser->curr_item->function
#define C_VAR_ITEM parser->curr_item->const_var

#define STRING_TOKEN_T token_type_to_str(TOKEN_T)
#define STRING_KW_T kw_type_to_str(parser->token->attribute->keyword_type)

#define TOKEN_REPR parser->token->attribute->string
#define TOKEN_KW_T parser->token->attribute->keyword_type 

#define IS_DTYPE(_keyword) ((_keyword) == KEYWORD_NIL    || \
                            (_keyword) == KEYWORD_NUMBER || \
                            (_keyword) == KEYWORD_STRING || \
                            (_keyword) == KEYWORD_INTEGER)

#define IS_LITERAL(_ttype) ((_ttype) == TOKEN_NUM_LIT || \
                            (_ttype) == TOKEN_INT_LIT || \
                            (_ttype) == TOKEN_STR_LIT)

#define IS_NIL(_ttype) (((_ttype) == TOKEN_KEYWORD) && (TOKEN_KW_T == KEYWORD_NIL))

#define GET_TOKEN() do {                                                   \
    destroy_token(parser->token);                                          \
    if (parser == NULL) return ERR_INTERNAL;                               \
    parser->token = get_next_token(parser->src);                           \
    if (parser->token == NULL) {                                           \
        error_message("Fatal", ERR_INTERNAL, "INTERNAL INTERPRET ERROR!"); \
        return ERR_INTERNAL;                                               \
    }                                                                      \
} while(0)

// TODO: error message for specific nonterminal rule function
#define CHECK_RESULT_VALUE(_result, _value) do {                                  \
    if ((_result) != (_value)) {                                                  \
        if (TOKEN_T == TOKEN_ERROR) {                                             \
            error_message("Scanner", (_result), "unknown token '%s'", TOKEN_REPR);\
        } else {                                                                  \
            error_message("Parser", (_result), "unexpected token '%s' (%s)",      \
                                                TOKEN_REPR, STRING_TOKEN_T);      \
        }                                                                         \
        return (_result);                                                         \
    }                                                                             \
} while(0)

#define CHECK_RESULT_VALUE_SILENT(_result, _value) do { \
    if ((_result) != (_value)) {                        \
        return (_result);                               \
    }                                                   \
} while(0)

#define CHECK_TOKEN_ERROR() do {                                             \
    if (TOKEN_T == TOKEN_ERROR) {                                            \
        error_message("Scanner", ERR_LEX, "unknown token '%s'", TOKEN_REPR); \
        return ERR_LEX; /* scanner handles error message */                  \
    }                                                                        \
} while(0)

#define CHECK_TOKEN_TYPE(_type) do {                                    \
    if (TOKEN_T != (_type)) {                                           \
        error_message("Parser", ERR_SYNTAX, "expected: '%s', is: '%s'", \
                             token_type_to_str(_type), STRING_TOKEN_T); \
        return ERR_SYNTAX;                                              \
    }                                                                   \
} while(0)

#define CHECK_KEYWORD(_type) do {                                               \
    if (TOKEN_KW_T != (_type)) {                                                \
        error_message("Parser", ERR_SYNTAX, "expected keyword: '%s', is: '%s'", \
                                           kw_type_to_str(_type), STRING_KW_T); \
        return ERR_SYNTAX;                                                      \
    }                                                                           \
} while(0)


#define PARSER_EAT() do { \
    GET_TOKEN();          \
    CHECK_TOKEN_ERROR();  \
} while(0)


#define HANDLE_SYMTABLE_FUNC_DEC() do {                                                         \
    /* Create symtable item for current ID if not present in symtable */                        \
    if (!(parser->curr_item = symtable_search(SYMTAB_G, TOKEN_REPR))) {                         \
        /* Current function ID not found in global symtab */                                    \
        /* Insert function ID into the newly created item of global symtable */                 \
        if (!(parser->curr_item = symtable_create_and_insert_function(SYMTAB_G, TOKEN_REPR))) { \
            return ERR_INTERNAL;                                                                \
        }                                                                                       \
    } /* if item not found */                                                                   \
} while(0)

#define HANDLE_SYMTABLE_FUNC_DEF() do {                                                         \
    /* Create symtable item for current ID if not present in symtable */                        \
    if (!(parser->curr_func = symtable_search(SYMTAB_G, TOKEN_REPR))) {                         \
        /* Current function ID not found in global symtab */                                    \
        /* Insert function ID into the newly created item of global symtable */                 \
        if (!(parser->curr_func = symtable_create_and_insert_function(SYMTAB_G, TOKEN_REPR))) { \
            return ERR_INTERNAL;                                                                \
        }                                                                                       \
    } /* if item not found */                                                                   \
} while(0)

/**
 * @brief Check if given function id is built in function
 */
bool is_built_in(char *func_id);

/**
 * @brief Define every specified built in function of ifj21
 */
int define_every_builtin_function(parser_t *parser);

/**
 * @brief Create an item in the global symtable and insert corresponding function
 */
int create_builtin_function(parser_t *parser, char *func_id);

/* Built-in function of ifj21 */
int define_ord(parser_t *parser, char *func_id);
int define_chr(parser_t *parser, char *func_id);
int define_reads(parser_t *parser, char *func_id);
int define_readi(parser_t *parser, char *func_id);
int define_readn(parser_t *parser, char *func_id);
int define_write(parser_t *parser, char *func_id);
int define_substr(parser_t *parser, char *func_id);
int define_tointeger(parser_t *parser, char *func_id);

#endif // PARSER_H
