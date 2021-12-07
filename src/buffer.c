/**
 * @file buffer.c
 *  
 * @brief Function implementation for dynamic string manipulation
 * @author Krištof Šiška - xsiska16
 *         Šimon Šmída   - xsmida03
 */

#include <stdio.h>
#include <stdlib.h>
#include "include/buffer.h"


/**
 * @brief Initialize buffer - dynamic string manipulation
 */
string_t *init_buffer(void)
{
    string_t *buffer = NULL;
    buffer = (string_t *) malloc(sizeof(string_t));
    if (buffer == NULL) {
        // error
    }
    buffer->string = NULL;
    // Using calloc to avoid getting errors from valgrind
    buffer->string = calloc(BASIC_ALLOC_SIZE, sizeof(char));
    if (buffer->string == NULL) {
        // error
    }
    buffer->current_index = 0;
    buffer->size = BASIC_ALLOC_SIZE;
    return buffer;
}


/**
 * Help procedure to append a character into the buffer
 */ 
void append_character(string_t *buffer, int c)
{
    if (buffer->current_index >= buffer->size - 1) {
		char *temp = realloc(buffer->string, buffer->size * 2);
        if (temp == NULL) {
            fprintf(stderr, "Intern realloc problem in lexer\n");
            exit(1);
        }
        buffer->size = buffer->size * 2;
		buffer->string = temp;
    } 
    buffer->string[buffer->current_index] = c;
    buffer->current_index = buffer->current_index + 1;
}

void append_string(string_t *buffer, char *string_to_be_appended){
	for (int i = 0; i != '\0'; i++){
		append_character(buffer, string_to_be_appended[i]);
	}

	return;
}


/**
 * @brief Deallocate the buffer
 */
void destroy_buffer(string_t *buffer)
{
    free(buffer->string);
    free(buffer);
}
