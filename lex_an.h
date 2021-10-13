#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


typedef struct Token {
    char *value;
    enum {
        keyword, 
        identifier, 
        operator, 
        separator, 
        equals, 
        colon,
        str_literal
    } TYPE;
} TOKEN_T;

TOKEN_T *read_input();

TOKEN_T *generate_token();


/** 
 * Used to get terminating null to the end of the string
 * and then printing the content of string to stdout
 * 
 * @param string Buffer with content to be written
 * @param String_index Pointer to the position in string to write terminating null
 */
void print_string(char *string, int *string_index);



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