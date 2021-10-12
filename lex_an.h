#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int read_input();
void generate_token(char *string, int *string_index, char *type);


/** 
 * Used to get terminating null to the end of the string
 * and then printing the content of string to stdout
 * 
 * @param string Buffer with content to be written
 * @param String_index Pointer to the position in string to write terminating null
 */
void print_string(char *string, int *string_index);

/**
 * Help procedure to append a character into the buffer
 * 
 * @param int c A character to be written into buffer
 * @param string A buffer to be written into
 * @param string_index A pointer to the int which represents the current index in string
 *
 */
void append_character(int c, char *string, int *string_index);

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