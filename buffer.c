#include <stdio.h>
#include <stdlib.h>
#include "buffer.h"

STRING_T *init_buffer(){
    STRING_T *buffer;
    buffer = malloc(sizeof(STRING_T));
    if (buffer == NULL){
        // error
    }
    buffer->string = malloc(BASIC_ALLOC_SIZE);
    if (buffer->string == NULL){
        // error
    }
    buffer->current_index = 0;
    buffer->size = BASIC_ALLOC_SIZE;
}

void append_character(STRING_T *buffer, int c){
    /* if (buffer->current_index >= buffer->size - 1){
        buffer->string = realloc(buffer->string, buffer->size * 2);
        if (buffer->string == NULL){
            fprintf(stderr, "Intern reallo problem in lexer\n");
            exit(1);
        }
        buffer->size = buffer->size * 2;
    } */
    buffer->string[buffer->current_index] = c;
    buffer->current_index = buffer->current_index + 1;
    return;
}

void destroy_buffer(STRING_T *buffer){
    free(buffer->string);
    free(buffer);
    
    return;
}