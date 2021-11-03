#ifndef BUFFER_H
#define BUFFER_H

/**
 * @file buffer.h
 * 
 * @brief functions and structures used in dynamic string manipulation
 * @author Krištof Šiška - xsiska16
 */

typedef struct String_t{
    char *string;
    unsigned current_index;
    unsigned size;
}STRING_T;

void destroy_buffer();

STRING_T *init_buffer();


/**
 * Help procedure to append a character into the buffer
 * 
 * @param int c A character to be written into buffer
 * @param string A buffer to be written into
 * @param string_index A pointer to the int which represents the current index in string
 *
 */
void append_character(STRING_T *buffer, int c);

#define BASIC_ALLOC_SIZE 20

#endif