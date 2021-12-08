#include  "../src/include/code_generator.h"
#include "unity/src/unity.h"
#include  "../src/include/scanner.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test1(void)
{
    FILE *sourceCode;
    
    sourceCode = fopen("program.code", "r");

    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipWrite.txt", "r");
    token = get_next_token(srcfile);
    // token->type = TOKEN_INT_LIT;
    // token->attribute->integer = 5;
    
    while(token->type != TOKEN_EOF){
        //print_token(token);
        //generate_built_in_write(0,token);
        destroy_token(token);
        token = get_next_token(srcfile);
    }
    print_token(token);
    destroy_token(token);

    fclose(srcfile);
    

    //TEST_ASSERT_EQUAL_INT32(NO_ERROR, generator_begin());

    // Token token;
    // token.type = TOKEN_INTEGER;
    // token.attribute.integer = 22;

    // gen_pushs(token, true);

    // token.type = TOKEN_DECIMAL;
    // token.attribute.decimal = 22.0;

    // gen_pushs(token, true);

    // gen_eqs();

    // gen_instruct("POPS GF@$temp");
    // gen_instruct("WRITE GF@$temp");

    // gen_instruct("CREATEFRAME");
    // gen_instruct("DEFVAR TF@%1");
    // gen_instruct("MOVE TF@%1 string@auto");
    // gen_instruct("DEFVAR TF@%2");
    // gen_instruct("MOVE TF@%2 int@2");
    // gen_instruct("DEFVAR TF@%3");
    // gen_instruct("MOVE TF@%3 int@3");

    // gen_instruct("CALL $substr");

    // //gen_instruct("TYPE TF@%retval TF@%retval");
    // gen_instruct("WRITE TF@%retval");

    // Token token;
    // token.type = TOKEN_IDENTIFIER;
    // str_init(&token.attribute.string);
    // str_copy(&token.attribute.string, "dlzka");

    // Token token2;
    // token2.type = TOKEN_INTEGER;
    // token2.attribute.integer = 15;

    // Token token3;
    // token3.type = TOKEN_DECIMAL;
    // token3.attribute.decimal = 20.35;

    // gen_instruct("DEFVAR GF@dlzka");
    // gen_instruct("MOVE GF@dlzka string@cau");

    // Token token;
    // str_init(&token.attribute.string);
    // str_copy(&token.attribute.string, "Cau\nko");
    // token.type = TOKEN_STRING;

    //gen_print(true, token);

    //generate_code(sourceCode);
}

int main(void) 
{
    UNITY_BEGIN();
    
    //RUN_TEST(test1);

    return UNITY_END();
}