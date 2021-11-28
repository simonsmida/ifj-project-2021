#include  "../src/include/symtable.h"
#include  "../src/include/error.h"
#include "unity.h"

void setUp(void)
{
    // symtable_t *table;
    // symtable_init(CAPACITY);
}

void tearDown(void)
{
}

void test_symtable_init(void)
{
    symtable_t* table = NULL;
    table = symtable_init(CAPACITY);

    TEST_ASSERT_TRUE(table->size == 0);
    TEST_ASSERT_TRUE(table->items_size == CAPACITY);
    TEST_ASSERT_TRUE(table->items[1] == NULL);
    TEST_ASSERT_TRUE(table->items[9] == NULL);
    TEST_ASSERT_TRUE(table->items[0] == NULL);
    TEST_ASSERT_TRUE(table->items[8] == NULL);
    TEST_ASSERT_TRUE(table->items[3] == NULL);
    symtable_destroy(table);
}
void test_symtable_insert(void) 
{
    symtable_t* table;
    table = symtable_init(CAPACITY);
    symtable_item_t *help;
    TEST_ASSERT_EQUAL_INT32(0,table->size);
    //otestovanie Insert
    char *key = "jedna";
    help = symtable_insert(table,key); //pridam jednotku
    TEST_ASSERT_TRUE(!strcmp(help->key,"jedna"));
    TEST_ASSERT_TRUE(help->next == NULL);
    TEST_ASSERT_EQUAL_INT32(1,table->size);

    char *key2 = "dva";
    help = symtable_insert(table,key2); //pridam dvojku
    TEST_ASSERT_TRUE(!strcmp(help->key,"dva"));
    TEST_ASSERT_TRUE(table->size == 2);

    char *key3 = "tri";
    help = symtable_insert(table,key3); //pridam dvojku
    TEST_ASSERT_TRUE(!strcmp(help->key,"tri"));
    TEST_ASSERT_FALSE(table->size == 2);
    TEST_ASSERT_TRUE(table->size == 3);

    //HLADANIE
    symtable_item_t* search = NULL;
    search = symtable_search(table,key);
    TEST_ASSERT_NOT_NULL(search);
    search = symtable_search(table,key2);
    TEST_ASSERT_NOT_NULL(search);
    search = symtable_search(table,key3);
    TEST_ASSERT_NOT_NULL(search);

    //Pustime prvok s rovnakym key
    help = symtable_insert(table,key); //pridam jednotku druhy raz
    search = symtable_search(table,key);
    TEST_ASSERT_TRUE(help == search);
    symtable_destroy(table);
}

void test_symtable_delete(void)
{
    symtable_t* table = NULL;
    symtable_init(CAPACITY); //inicializacia tabulky
    char *key = "jedna";
    symtable_insert(table,key); //pridam jednotku
    TEST_ASSERT_TRUE(table->size == 1);
    //symtab_clear(table);
    TEST_ASSERT_TRUE(table->size == 0);
    TEST_ASSERT_TRUE(table->items[1] == NULL);
    TEST_ASSERT_TRUE(table->items[3] == NULL);

    char *key2 = "dva";
    char *key3 = "tri";
    symtable_insert(table,key); //znova pridam jednotku
    symtable_insert(table,key2); //pridam dvojku
    symtable_insert(table,key3); 
    TEST_ASSERT_TRUE(table->size == 3);
    //symtable_clear(table);
    TEST_ASSERT_TRUE(table->items[1] == NULL);
    TEST_ASSERT_TRUE(table->items[3] == NULL);
    TEST_ASSERT_TRUE(table->size == 0);

    symtable_destroy(table);
}

int main(void) 
{
    printf("\n");
    UNITY_BEGIN();

    RUN_TEST(test_symtable_init);
    RUN_TEST(test_symtable_insert);
    //RUN_TEST(test_symtable_delete);

    return UNITY_END();
}