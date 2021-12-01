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
#define SYMSTACK_SIZE 50

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
    bool defined; // whether variable had been defined or not
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

typedef struct symtable_stack {
	symtable_t *symstack[SYMSTACK_SIZE];
	int top_index;
}symtable_stack_t;


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
 * 
 * @param n Size of hasthable
 */
symtable_t *symtable_init(size_t n);

/**
 * @brief Hash table destructor
 * 
 * @param s Pointer to the hashtable to be destroyed
 */
void symtable_destroy(symtable_t *s);

/**
 * @brief Create a structure for functions in hashtable and insert it into symtab item
 * This function automatically inserts item_function into the hashtable item
 * with the given key. No need to call for other functions to insert it.
 * Returned item_function_t * can be ignored
 * 
 * @param s Pointer to the hashtable
 * @param key key to the hashtable
 */
item_function_t *symtable_create_and_insert_function(symtable_t *s, const char *key);

/**
 * @brief Create a structure for const/vars in hashtable
 * The function doesnt insert it into the hashtable, this has to be done
 * with the function symtable_insert_const_var
 * 
 * @param is_var indicates whether symbtable const_var is const or var
 * @param is_defined indicates whether const/var is defined in given "ramec"
 * @param type Type of variable
 * 
 * @return Pointer to the created const_var_t if successfull, otherwise NULL
 */ 
const_var_t *symtable_create_const_var(bool is_var, bool is_defined, data_type_t type);


/**
 * @brief Updates the param list of item_function. Inserts the new found param
 * into the list
 * 
 * @param s Pointer to the hashtable
 * @param data The type of the new found function parameter
 * @param key Hash to the table
 */
void symtable_insert_new_function_param(symtable_t *s ,data_type_t data, const char *key);


/**
 * @brief Updates the return type list of item_function. Inserts the new found return type
 * into the list
 * 
 * @param s Pointer to the hashtable
 * @param data The type of the new found function return type
 * @param key Hash to the table
 */
void symtable_insert_new_function_ret_type(symtable_t *s ,data_type_t data, const char *key);


/**
 * @brief Insert the given const_var variable into the item given by key.
 * The const_var variable has to be previously initialized by the function
 * symtable_create_const_var;
 * 
 * @param s Pointer to the hashtable
 * @param key key to the hashtable
 * @param const_var Pointer to be set into the given item->const_var
 */
void symtable_insert_const_var(symtable_t *s, char *key, const_var_t *const_var);

/**
 * @brief Creates an initialized item with the given key and inserts it
 * into the hashtable. Item->key is set to the given key, and pointers
 * to item_function or item_const_var are set to NULL. We will manipulate with them
 * after we determine whether a given ID is function or a variable / constant
 * 
 * @param s Pointer to the hashtable structure
 * @param key Hashtable key
 */ 
symtable_item_t *symtable_insert(symtable_t *s, const char *key);


/**
 * @brief Searches the hashtable, and determines whether an item with the given
 * key exists in the table
 * 
 * @param s pointer to the hashtable structure
 * @param key hashtable key
 * 
 * @return Pointer to the item with item->key key if it exits, otherwise NULL 
 */ 
symtable_item_t *symtable_search(symtable_t *s, const char *key);

symtable_t *symtable_stack_top(symtable_stack_t *stack);
void symtable_stack_pop(symtable_stack_t *stack);
void symtable_stack_push(symtable_stack_t *stack, symtable_t *s);
void symtable_stack_init(symtable_stack_t *stack);
void symtable_stack_destroy(symtable_stack_t *stack);
bool stack_empty(symtable_stack_t *stack);


// TODO Funkcia definovana aj deklarovana?

#endif
