#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint32_t
#include "include/symtable.h"

/**
 * @brief Hashing function - calculate index for given key
 */
size_t symtable_hash_function(const char *key)
{
    // TODO: this is Pepe's magic, we can replace it
    uint32_t h = 0;
    const unsigned char *p;

    for (p=(const unsigned char*)str; *p != '\0'; p++) {
        h = 65599*h + *p;
    }
  
    return h;
}

/**
 * @brief Calculate hash table index using hash function
 */
unsigned symtable_hash_index(char *key)
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
        error_message("symtable", "Calloc failure.\n"); // TODO internal compiler err? 
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

/**
 * @brief Hash table destructor
 */
void symtable_destroy(symtable_t *s)
{
    // TODO
    free(s);
    s = NULL;
}

symtable_item_t *symtable_insert(symtable_t *s, const char *key)
{
    // TODO
}


symtable_item_t *symtable_search(symtable_t *s, const char *key) 
{
    int index = symtable_hash_index(key);
    symtable_item_t *item = s->items[index];

    if (item != NULL) { // item exists
        if (!strcmp(item->key, key)) {
            return item;
        }
        item = item->next;
    }
    return NULL;
}

