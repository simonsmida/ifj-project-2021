/**
 * @file symtable.c
 * 
 * @brief function definitions used in symtab
 * @author Krištof Šiška - xsiska16
 *         Šimon Šmída   - xsmida03
 */

<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h> // uint32_t
<<<<<<< HEAD
#include <string.h>
#include "include/symtable.h"
#include "include/error.h"
=======

>>>>>>> main
=======
#include "include/symtable.h"
#include "include/error.h"
>>>>>>> main

/**
 * @brief Hashing function - calculate index for given key
 */
size_t symtable_hash_function(const char *key)
{
    // TODO: this is Pepe's magic, we can replace it
    uint32_t h = 0;
    const unsigned char *p;

    for (p=(const unsigned char*)key; *p != '\0'; p++) {
        h = 65599*h + *p;
    }
  
    return h;
}

/**
 * @brief Calculate hash table index using hash function
 */
unsigned symtable_hash_index(const char *key)
{
    return symtable_hash_function(key) % CAPACITY;
}


/**
 * @brief Hash table constructor
 */
symtable_t *symtable_init(size_t n)
{
    symtable_t *s = calloc(1, sizeof(symtable_t) + n*sizeof(symtable_item_t));
    if (s == NULL) {
        error_message("symtable", ERR_INTERNAL,  "Calloc failure.\n"); // TODO internal compiler err? 
        return NULL; // TODO: calloc failure error message
    } 
    s->size = 0; // current number of records
    s->items_size = n; // size of an array of pointers

    for (int i = 0; i < n; i++) {
        s->items[i] = NULL;
    }

    return s;
}

// TODO: symtable_create_item()

// TODO: symtable_create_function();

// TODO: symtable_create_const_var();

/**
 * @brief Hash table destructor
 */
void symtable_destroy(symtable_t *s)
{
	symtable_item_t *next;
	symtable_item_t *tmp;
    for (int i = 0; i < s->items_size; i++){
		tmp = s->items[i];
		while (tmp != NULL){
			if (tmp->function != NULL){
				if (tmp->function->ret_types != NULL){
					free(tmp->function->ret_types);
				}
				if (tmp->function->type_params != NULL){
					free(tmp->function->type_params);
				}
				free(tmp->function);
			}

			if (tmp->const_var != NULL){
				free(tmp->const_var);
			}

			free(tmp->key);
			next = tmp->next;
			free(tmp);	
			tmp = next;		
		} // while
	} // for
    free(s);
    s = NULL;
}

symtable_item_t *symtable_insert(symtable_t *s, const char *key)
{
    int index = symtable_hash_index(key);
	symtable_item_t *item;
	symtable_item_t *previous = NULL;
	item = symtable_search(s, key);
	if (item == NULL){
		item = s->items[index];
		while (item != NULL){
			previous = item;
			item = item->next;
		}
		item = calloc(1, sizeof(symtable_item_t));
		if (item == NULL){
			error_message("Symtable", ERR_INTERNAL, "Failed calloc");
		}
		item->key = calloc(1, strlen(key) + 1);
		if (item->key == NULL){
			error_message("Symtable", ERR_INTERNAL, "Failed calloc");
		}
		strcpy( item->key, key );
		item->const_var = NULL;
		item->function = NULL;
		item->next = NULL;

		if (previous != NULL){
			previous->next = item;
		}
		else {
			s->items[index] = item;
		}
	}
	
	s->items_size++;

	return item;
}


symtable_item_t *symtable_search(symtable_t *s, const char *key) 
{
    int index = symtable_hash_index(key);
    symtable_item_t *item = s->items[index];

	while (item != NULL){
		if (!strcmp(item->key, key)) { // The item already exists
			return item;
		}
		item = item->next;
	}

    return NULL;
}

item_function_t *create_symtable_function(symtable_t *s, const char *key){
	
	symtable_item_t *item = symtable_search(s, key);
	if (item == NULL){
		fprintf(stderr, "There is no item with the key %s in symtab\n", key);
		return NULL;
	}
	item_function_t *function = NULL;

	function = calloc(1,sizeof(item_function_t));
	if (function == NULL){
		ERROR_MSG_SYMTABLE;
		return NULL;
	}
	function->num_params = 0;
	function->num_ret_types = 0;
	function->ret_types = calloc(1, sizeof(data_type_t));
	function->ret_types[0] = DTYPE_STRING;
	function->type_params = calloc(1, sizeof(data_type_t));

	if (function->ret_types == NULL || function->type_params == NULL){
		ERROR_MSG_SYMTABLE;
		return NULL;
	}

	item->function = function;
	return function;
}

const_var_t *create_symtable_const_var(bool is_var, bool is_defined, data_type_t type){
	const_var_t *param;
	param = calloc(1, sizeof(const_var_t));
	if (param == NULL){
		ERROR_MSG_SYMTABLE;
		return NULL;
	}
	param->is_var = is_var;
	param->data.type = type;
	param->data.defined = is_defined;
	return param;
}

void symtable_insert_const_var(symtable_t *s, char *key, const_var_t *const_var){
	symtable_item_t *item;
	item = symtable_search(s,key);
	if (item != NULL){
		item->const_var = const_var;
	}
	else {
		fprintf(stderr, "Cannot insert const_var. Item with key %s not found\n", key);
	}

	return;
}


void symtable_insert_new_function_param(symtable_item_t *item ,data_type_t data){
	data_type_t *function;
	function = realloc(item->function->type_params, sizeof(data_type_t) * item->function->num_params + 1);
	if (function == NULL){
		ERROR_MSG_SYMTABLE;
	}
	function[item->function->num_params] = data;
	item->function->num_params++;
	item->function->type_params = function;

	return;
}

void symtable_insert_new_function_ret_type(symtable_item_t *item ,data_type_t data){
	data_type_t *ret_types;
	ret_types = realloc(item->function->ret_types, sizeof(data_type_t) * (item->function->num_ret_types + 1));
	if (ret_types == NULL){
		ERROR_MSG_SYMTABLE;
	}
	ret_types[item->function->num_ret_types] = data;
	item->function->num_ret_types++;
	item->function->ret_types = ret_types;

	return;
}
