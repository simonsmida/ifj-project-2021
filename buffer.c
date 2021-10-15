/**
 * @file buffer.c
 *  
 * @brief Function implementation for dynamic string manipulation
 * @author Krištof Šiška - xsiska16
 */

#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

STRING_T *init_buffer(){
    STRING_T *buffer = NULL;
    buffer = (STRING_T *) malloc(sizeof(STRING_T));
    if (buffer == NULL){
        // error
    }
    buffer->string = NULL;
    // Using calloc to avoid getting errors from valgrind
    buffer->string = calloc(BASIC_ALLOC_SIZE, sizeof(char));
    if (buffer->string == NULL){
        // error
    }
    buffer->current_index = 0;
    buffer->size = BASIC_ALLOC_SIZE;
    return buffer;
}

void append_character(STRING_T *buffer, int c){
     if (buffer->current_index >= buffer->size - 1){
        buffer->string = realloc(buffer->string, buffer->size * 2);
        if (buffer->string == NULL){
            fprintf(stderr, "Intern reallo problem in lexer\n");
            exit(1);
        }
        buffer->size = buffer->size * 2;
    } 
    buffer->string[buffer->current_index] = c;
    buffer->current_index = buffer->current_index + 1;
    return;
}

void destroy_buffer(STRING_T *buffer){
    free(buffer->string);
    free(buffer);
    
    return;
}