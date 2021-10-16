/**
 * @file lex_an.h
 * 
 * @brief functions and structures used in lexer
 * @author Krištof Šiška - xsiska16
 */

#ifndef LEX_AN_H
#define LEX_AN_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


typedef struct Token {
    char *value;
    enum {
        integer,
        keyword, 
        identifier, 
        operator, 
        separator, 
        equals, 
        colon,
        str_literal,
        assign,
        l_paren,
        r_paren
    } TYPE;
} TOKEN_T;

TOKEN_T *get_next_token();

TOKEN_T *generate_token();



/**
 * Determines wether a given character is operator
 * 
 * @param int c
 * @return true if character is operator, otherwise false
 */
bool is_operator(int c);

/** 
 * Determines wether a given string is a variable type
 * aka "integer" or "double" etc
 * 
 * @param string A buffer to check
 * @return True if string is a variable_type, otherwise false 
 */
bool is_variable_type(char *string);

/**
 * Determines wether a given string is a keyword
 * 
 * @param string A buffer to check
 * @return True if string is a keyword, otherwise false
 */
bool is_keyword(char *string);

#endif // LEX_AN_H