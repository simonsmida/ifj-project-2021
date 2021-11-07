#ifndef SYMTABLE_H
#define SYMTABLE_H

#define CAPACITY 13001 // random prime

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

typedef struct symtable_item {
    char *key;
    symbol_data_t data;
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
unsigned symtable_hash_index(char *key);

/**
 * @brief Hash table constructor
 */
symtable_t *symtable_init(size_t n);

/**
 * @brief Hash table destructor
 */
void symtable_destroy(symtable_t *s);

symtable_item_t *symtable_insert(symtable_t *s, const char *key);
symtable_item_t *symtable_search(symtable_t *s, const char *key);

#endif
