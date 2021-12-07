#include  "../src/include/parser.h"
#include "../src/include/bottom_up_sa.h"
#include "../src/include/error.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void write_1(void)
{
    /*
    require "ifj21"

    function main()
        local a : integer = 5
        write("vysl\n", a,"aaa\n")
    end
    main()
    */

    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/good/snipWrite.ifj21", "r");
    TEST_ASSERT_EQUAL_INT32(EXIT_OK, parser_parse(srcfile));
}

void err_sem_3(void)
{
    /*
    require "ifj21"

    function main()
        local vysl : integer = 0
        local x:integer=1local y:string="ahoj"

    end
    main()
    concat()
    */
    //ci bola funkcia aspon deklarovana
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.1.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_DEF, parser_parse(srcfile));

    //redeklarace/redefinice? premennej SIMON ZATIAL NEKONTROLUJE
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.2.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_DEF, parser_parse(srcfile));

    //nedefinovaná premenná SIMON ZATIAL NEKONTROLUJE
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.3.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_DEF, parser_parse(srcfile));

    //redefinice funkcie
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.4.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_DEF, parser_parse(srcfile));

    //redeklarace funkcie
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.5.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_DEF, parser_parse(srcfile));

    //rovnake id premennej a func
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.6.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_DEF, parser_parse(srcfile));

    //redeklaracia premennej rozne bloky, EXIT_OK
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.7.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(EXIT_OK, parser_parse(srcfile));

    //redeklaracia premennej rozne bloky, EXIT_OK
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.8.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(EXIT_OK, parser_parse(srcfile));
}

void err_sem_4(void)
{
    /*
    require "ifj21"
    function main()
        local vysl : integer = "papa"
    end
    main()
    */
    
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.1.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_ASSIGN, parser_parse(srcfile));
}

void err_sem_5(void)
{
    //zlý typ pri returne
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.1.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //nespravny pocet paramtetrov pri returne
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.2.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //nespravny pocet parametrov pri def fun v porovnani s dec 
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.3.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //nespravny typ parametrov pri def fun v porovnani s dec 
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.4.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //nespravny pocet navrat. hod. pri def fun v porovnani s dec 
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.5.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //nespravny typ navrat. hod. pri def fun v porovnani s dec 
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.6.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //nespravny typ pri volani funkcie
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.7.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //nespravny pocet par. pri volani funkcie
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.8.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //pocet ocakavanych hodnot prevysuje pocet navratovych hodnot
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.9.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));

    //typ ocakavanych hodnout nesedi s typom navratovych hodnot
    srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.10.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_PROG, parser_parse(srcfile));
    
}

void err_sem_6(void)
{
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_6.1.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_TC, parser_parse(srcfile));
}

void err_sem_7(void)
{
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_7.1.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SEMANTIC_OTHER, parser_parse(srcfile));
}

void err_syn_1(void)
{   
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/bad/var_dec_syn_err.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(ERR_SYNTAX, parser_parse(srcfile));
}

void test_fun_dec(void)
{
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/good/func_dec.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(EXIT_OK, parser_parse(srcfile));
}

void test_fun_multiassign(void)
{
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/good/multiassign.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(EXIT_OK, parser_parse(srcfile));
}

void test_fun_var_def(void)
{
    FILE *srcfile = fopen("tests/test_data/CodeStructureSnippets/good/var_def.ifj21", "r");
    TEST_ASSERT_EQUAL_INT(EXIT_OK, parser_parse(srcfile));
}

int main(void) 
{
    UNITY_BEGIN();
    
    RUN_TEST(write_1);
    //RUN_TEST(err_sem_3);
    // RUN_TEST(err_sem_4);
    // RUN_TEST(err_sem_5);
    // RUN_TEST(err_sem_6);
    // RUN_TEST(err_sem_7);
    // RUN_TEST(err_syn_1);
    // RUN_TEST(test_fun_dec);
    // RUN_TEST(test_fun_multiassign);
    // RUN_TEST(test_fun_var_def);

    return UNITY_END();
}