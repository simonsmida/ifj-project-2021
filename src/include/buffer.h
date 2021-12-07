#ifndef BUFFER_H
#define BUFFER_H

/**
 * @file buffer.h
 * 
 * @brief functions and structures used in dynamic string manipulation
 * @author Krištof Šiška - xsiska16
 *         Šimon Šmída   - xsmida03
 */

typedef struct string {
    char *string;
    unsigned current_index;
    unsigned size;
} string_t;


/**
 * @brief Initialize a buffer - dynamic string manipulation
 */
string_t *init_buffer(void);

/**
 * @brief Deallocate the buffer
 * @param buffer pointer to the buffer to be deallocated
 */
void destroy_buffer(string_t *buffer);

/**
 * Help procedure to append a character into the buffer
 * 
 * @param int c A character to be written into buffer
 * @param string A buffer to be written into
 * @param string_index A pointer to the int which represents the current index in string
 *
 */
void append_character(string_t *buffer, int c);


void append_string(string_t *buffer, char *string_to_be_appended);

#define BASIC_ALLOC_SIZE 20

#endif
