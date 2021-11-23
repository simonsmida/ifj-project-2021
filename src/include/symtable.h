/**
 * @file symtable.h
 * 
 * @brief functions and structures used symtab
 * @author Krištof Šiška - xsiska16
 *         Šimon Šmída   - xsmida03
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#define CAPACITY 13001 // random prime

#define ERROR_MSG_SYMTABLE \
		error_message("Symtable", ERR_INTERNAL ,"Failed calloc\n")
	


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint32_t
#include <string.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum data_type {
    DTYPE_UNKNOWN = 0,
    DTYPE_INT,
    DTYPE_NUMBER,
    DTYPE_STRING,
    DTYPE_NIL,
} data_type_t;

typedef struct symbol_data {
    data_type_t type;
    bool define; // whether variable had been defined or not
} symbol_data_t;

typedef struct item_function {
	int num_params;
	int num_ret_types;
	data_type_t *type_params;
	data_type_t *ret_types;
} item_function_t;

typedef struct item_const_var {
	symbol_data_t data;
	bool is_var;
} const_var_t;

typedef struct symtable_item {
    char *key;
    const_var_t *const_var;
	item_function_t *function;
    struct symtable_item *next;
} symtable_item_t;

typedef struct symtable {
    unsigned int size; // total number of items in htab
    unsigned int items_size; // number of items in htab list
    symtable_item_t *items[]; // linked list of items
} symtable_t;


/**
 * @brief Hashing function - calculate hash for given key
 * 
 * From: http://www.cse.yorku.ca/~oz/hash.html
 * Variant: sdbm 
 * 
 * @param key hash table item key
 * @return hash for the given key
 */
size_t symtable_hash_function(const char *key);

/**
 * @breif Calculates index into the hash table using generated hash
 * @param key hash table item key
 */
unsigned symtable_hash_index(const char *key);

/**
 * @brief Hash table constructor
 */
symtable_t *symtable_init(size_t n);

/**
 * @brief Hash table destructor
 */
void symtable_destroy(symtable_t *s);

/**
 * @brief Create a structure for functions in hashtable and insert it into symtab item
 */
item_function_t *create_symtable_function();

/**
 * @brief Create a structure for const/vars in hashtable and insert it into symtab item
 * 
 * @param is_var indicates whether symbtable const_var is const or var
 * @param is_defined indicates whether const/var is defined in given "ramec"
 * @param type Type of variable
 */ 
const_var_t *create_symtable_const_var(bool is_var, bool is_defined, data_type_t type);

void symtable_insert_new_function_param(symtable_item_t *item ,data_type_t data);

void symtable_insert_new_function_ret_type(symtable_item_t *item ,data_type_t data);

void symtable_insert_const_var(symtable_t *s, char *key, const_var_t *const_var);


symtable_item_t *symtable_insert(symtable_t *s, const char *key);
symtable_item_t *symtable_search(symtable_t *s, const char *key);

#endif
