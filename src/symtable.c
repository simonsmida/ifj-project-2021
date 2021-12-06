/**
 * @file symtable.c
 * 
 * @brief function definitions used in symtab
 * @author Krištof Šiška - xsiska16
 *         Šimon Šmída   - xsmida03
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h> // uint32_t
#include "include/symtable.h"
#include "include/error.h"

#include "include/scanner.h"


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
        //error_message("symtable", ERR_INTERNAL,  "Calloc failure.\n"); // TODO internal compiler err? 
        return NULL; // TODO: calloc failure error message
    } 
    s->size = 0; // current number of records
    s->items_size = n; // size of an array of pointers

    for (size_t i = 0; i < n; i++) {
        s->items[i] = NULL;
    }

    return s;
}

/**
 * @brief Hash table destructor
 */
void symtable_destroy(symtable_t *s)
{
	symtable_item_t *next;
	symtable_item_t *tmp;
    for (size_t i = 0; i < s->items_size; i++){
		tmp = s->items[i];
		while (tmp != NULL){
			if (tmp->function != NULL){
				if (tmp->function->ret_types != NULL){
					free(tmp->function->ret_types);
				}
				if (tmp->function->type_params != NULL){
					free(tmp->function->type_params);
				}
				if (tmp->function->local_symtable != NULL){
					symtable_destroy(tmp->function->local_symtable);
				}
				free(tmp->function);
			} // function

            if (tmp->const_var != NULL){
                free(tmp->const_var);
            } // const_var

			if (tmp->key != NULL){
				free(tmp->key);
			}

			next = tmp->next;
			free(tmp);	
			tmp = next;		
		} // while
	} // for
    free(s);
    s = NULL;
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
    // key not found
    return NULL;
}

// TODO: reset parser->curr_block_id
bool would_be_var_redeclared(symtable_t *s, const char *key, int block_id) 
{
    int index = symtable_hash_index(key);
    symtable_item_t *item = s->items[index];

	while (item != NULL){
		if (!strcmp(item->key, key)) { // variable ID found
            // If variable names and block ids are the same, redeclaration
			if (item->const_var->block_id == block_id) {
                return item->const_var->declared;
            }
		}
		item = item->next;
	}
    return false;
}

symtable_item_t *most_recent_var(symtable_t *s, const char *key, int block_id, int block_depth, bool must_be_defined) 
{
    symtable_item_t *item = symtable_search(s, key);

    int i = 0;
    int current_depth;
    symtable_item_t *closest_item = NULL;
    int closest_depth_above;

	while (item != NULL) {
		if (!strcmp(item->key, key)) { // variable ID found
            const_var_t *var = item->const_var;
            current_depth = var->block_depth; 
            // Return variable with the same ID which is the closest from above
            if (i == 0) { // First match
			    if (current_depth <= block_depth) {
                    if (must_be_defined && (var != NULL && var->defined)) {
                        closest_depth_above = current_depth;
                        closest_item = item;
                    } else if (!must_be_defined && (var != NULL && var->declared)) {
                        closest_depth_above = current_depth;
                        closest_item = item;
                    }
                }
            } else { // Not first match
			    if ((current_depth <= block_depth) && (current_depth > closest_depth_above)) {
                    if (must_be_defined && (var != NULL && var->defined)) {
                        closest_depth_above = current_depth;
                        closest_item = item;
                    } else if (!must_be_defined && (var != NULL && var->declared)) {
                        closest_depth_above = current_depth;
                        closest_item = item;
                    }
                } 
            }
            i++;
		}
		item = item->next;
	} // while
    
    // Handle special case - variable has same depth, but different block id -> its not visible
    if ((closest_item->const_var->block_depth == block_depth) && 
        (closest_item->const_var->block_id != block_id)) {
        return NULL;
    }
    return closest_item;    
}

symtable_item_t *symtable_insert(symtable_t *s, const char *key)
{
    if (s == NULL || key == NULL) {
        return NULL;
    }

	symtable_item_t *first_in_chain;
	symtable_item_t *new;
    int index = symtable_hash_index(key);
    
    if ((new = calloc(1, sizeof(symtable_item_t))) == NULL) {
        return NULL; // INTERNAL ERROR
    }

    if ((new->key = calloc(1, strlen(key) + 1)) == NULL) {
        free(new);
        return NULL;
    }
    
    // First item of the chain
    first_in_chain = s->items[index];

    strcpy(new->key, key);
    new->const_var = NULL;
    new->function = NULL;
    new->next = first_in_chain; // connect the chain
    
    // Insert new item at the chain start 
    s->items[index] = new;    
	s->size++;
	
    return new;
}



symtable_item_t *symtable_create_and_insert_function(symtable_t *s, const char *key)
{
	/** 1. Insert an empty item to the hashtable */
    symtable_item_t *item = symtable_insert(s,key);
	if (item == NULL){
		//ERROR_MSG_SYMTABLE;
		return NULL;
	}	
	item_function_t *function = calloc(1,sizeof(item_function_t));
	if (function == NULL){
		//ERROR_MSG_SYMTABLE;
		free(item->key);
		free(item);
		return NULL;
	}

    function->declared = false;
    function->defined = false;
	function->num_params = 0;
	function->num_ret_types = 0;
	function->ret_types = calloc(1, sizeof(data_type_t));
	function->type_params = calloc(1, sizeof(data_type_t));
	function->local_symtable = symtable_init(CAPACITY);

	if (function->ret_types == NULL){
		//ERROR_MSG_SYMTABLE;
		free(item->key);
		free(item);
		free(function);
		return NULL;
	}
	if (function->type_params == NULL){
		//ERROR_MSG_SYMTABLE;
		free(item->key);
		free(item);
		free(function);
		free(function->ret_types);
		return NULL;
	}

	if (function->local_symtable == NULL){
		//ERROR_MSG_SYMTABLE;
		free(item->key);
		free(item);
		free(function);
		free(function->ret_types);
		free(function->type_params);
		return NULL;
	}
	
	item->function = function;
	return item;
}

symtable_item_t* symtable_insert_const_var(symtable_t *s, char *key){
	symtable_item_t *item = symtable_insert(s,key);
	if (item == NULL){
		//ERROR_MSG_SYMTABLE;
		return NULL;
	}	
	
	const_var_t *insert_var;
	insert_var = calloc(1, sizeof(const_var_t));
	if (insert_var == NULL){
		//ERROR_MSG_SYMTABLE;
		free(item->key);
		free(item);
		return NULL;
	}
	item->const_var = insert_var;
	
	return item;
}


int symtable_insert_new_function_param(symtable_t *s ,data_type_t data, const char *key) 
{
    symtable_item_t *item = symtable_search(s, key);
	if ((item != NULL) && (item->function)){
		data_type_t *function;
		function = realloc(item->function->type_params, sizeof(data_type_t)*(item->function->num_params + 1) );
		if (function == NULL){
			//ERROR_MSG_SYMTABLE;
			return ERR_INTERNAL;
		}
		function[item->function->num_params] = data;
		(item->function->num_params)++;
		item->function->type_params = function;
	}
	else {
		fprintf(stderr, "item doesnt exist in symtab\n");
	}

	return EXIT_OK;
}

int symtable_insert_new_function_ret_type(symtable_t *s ,data_type_t data, const char *key)
{
    symtable_item_t *item = symtable_search(s, key);
	if ( item != NULL ){
	data_type_t *ret_types;
	ret_types = realloc(item->function->ret_types, sizeof(data_type_t) * (item->function->num_ret_types + 1));
	if (ret_types == NULL){
		//ERROR_MSG_SYMTABLE;
		return ERR_INTERNAL;
	}
	ret_types[item->function->num_ret_types] = data;
	item->function->num_ret_types++;
	item->function->ret_types = ret_types;
	}
	return EXIT_OK;
}


