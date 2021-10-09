#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int read_input();
void generate_token(char *string, unsigned *string_index, char *type);

/** 
 * Used to get terminating null to the end of the string
 * and then printing the content of string to stdout
 * 
 * @param string Buffer with content to be written
 * @param String_index Pointer to the position in string to write terminating null
 */
void print_string();


void append_character();
bool is_operator();
bool is_variable_type(char *string);
bool is_keyword();