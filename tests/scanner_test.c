#include "./unity/src/unity.h"
#include  "../src/include/scanner.h"
#include "../src/include/buffer.h"
#include "../src/include/error.h"


void setUp(void)
{
}

void tearDown(void)
{
}

void TEST_blockCom(void)
{
    /* test file looks like this
       --[[ lol i like pp poopoo ]]
       dd
    */
    token_t *token;
    //STRING_T s;
    //STRING_T *buffer = buffer
    const char *filename = "tests/test_data/codeSnippets/good/snipBlockCom.txt";
    FILE *srcfile = fopen(filename, "r");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("dd",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_EOF(void)
{
    /*testfile

    */
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipEOF.txt", "r");
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_op_id_comment(void)
{
    /* testfile
        vysl = vysl * a -- dva prikazy vysl = vysl * a
        vysl = vysl / b
    */
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipLineCom.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("vysl",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ASSIGN);
    //TEST_ASSERT_EQUAL_STRING(NULL,token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("vysl",token->attribute->string,"dojeb");

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_MUL,token->type);
    //TEST_ASSERT_EQUAL_STRING_MESSAGE("*",token->attribute,"Dojebalo sa to");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"a");

    token = get_next_token(srcfile);

    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"vysl");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ASSIGN);
    //TEST_ASSERT_EQUAL_STRING(token->attribute->string,"=");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"vysl");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_DIV);
    //TEST_ASSERT_EQUAL_STRING(token->attribute->string,"/");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"b");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_blockCom_stringLit(void)
{
    /* test file
        --[[ --[[ ]lol jebaited ]jebaited again ]bait] : TOKEN_INT_LIT "i like pp" else while my mom>your mom<mom pl==siema // if you read~= dumb ]]
        -- --[[ ]lol jebaited ]jebaited again ]bait] : TOKEN_INT_LIT "i like pp" else while my mom>your mom<mom pl==siema // if you read~= dumb ]]
        "i like comments"
    */
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipComment.txt", "r");
    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_STR_LIT,token->type);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"\"i like comments\"");
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_assignOrEqual(void)
{
    /* test file:
      = ==== ==
    */

    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipCompare.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ASSIGN);
    //TEST_ASSERT_EQUAL_STRING("=",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EQ);
    //TEST_ASSERT_EQUAL_STRING(token->attribute->string,"==");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EQ);
    //TEST_ASSERT_EQUAL_STRING(token->attribute->string,"==");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EQ);
    //TEST_ASSERT_EQUAL_STRING(token->attribute->string,"==");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_goodId(void)
{
    /* test file

    jozko jozko99 _jozko99_ _99jozko99_ _ a

    */

    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipGoodID.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("jozko",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("jozko99",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_jozko99_",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_99jozko99_",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_",token->attribute->string);

     token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("a",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_badId(void)
{
    /* test file

        9jozko 9jozko_ 9_kola

    */

    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/bad/snipBadID.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(9,token->attribute->integer);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("jozko",token->attribute->string);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(9,token->attribute->integer);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("jozko_",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(9,token->attribute->integer);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_kola",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_goodNums(void)
{
    /* test file

     0 0000 0007 0007.008 10e10 10E10 10e-10 10e+10 10.10e10 10.10e-10 10.10e+10 10.10E-10 10e+10-

    */
    
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipGoodNums.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(0,token->attribute->integer);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(0,token->attribute->integer);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(7,token->attribute->integer);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(7.008,token->attribute->number);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10E10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e-10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e+10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10e10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10e-10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10e+10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10E-10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e+10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_MINUS);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_EOF,token->type);
}

void TEST_badNums(void)
{
    /* test file

    7. e10 E10 10.e10 .E10 10e 10.10E 10,.10e10 10.10ee10

    */
    printf("\n");
    printf("TEST_badNums MESSAGES:\n");
    printf("----------------------------------------------\n");
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/bad/snipBadNums.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_ID,token->type);
    TEST_ASSERT_EQUAL_STRING("e10",token->attribute->string);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_ID,token->type);
    TEST_ASSERT_EQUAL_STRING("E10",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    
    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    
    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_COMMA,token->type);
    //TEST_ASSERT_EQUAL_STRING(",",token->attribute);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    
    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_NUM_LIT,token->type);
    TEST_ASSERT_EQUAL_FLOAT(0e10,token->attribute->number);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    
    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);

    printf("----------------------------------------------\n");
    printf("\n");
    
}

void TEST_goodKeywords(void)
{
    /* test file

       do else end function global if integer local nil number require return string then while

    */

    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipKeywordsGood.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(token->type, TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"do");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_DO);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"else");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_ELSE);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"end");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_END);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"function");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_FUNCTION);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"global");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_GLOBAL);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"if");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_IF);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"integer");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_INTEGER);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"local");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_LOCAL);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"nil");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_NIL);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"number");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_NUMBER);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"require");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_REQUIRE);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"return");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_RETURN);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"string");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_STRING);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"then");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_THEN);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"while");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_WHILE);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_badKeywords(void)
{
    /* test file

       doelse whilethen integers localhost barbecuenilbeef n il

    */

    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/bad/snipKeywordBad.txt", "r"); 
    token = get_next_token(srcfile);                
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"doelse");

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"whilethen");

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"integers");

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"localhost");

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"barbecuenilbeef");

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"n");

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"il");

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
 }

void TEST_goodOP(void)
{
    /* test file

        * / - + # // .. == < <= > >= ~= 

    */

    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipOP.txt", "r");

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_MUL,token->type);
    //TEST_ASSERT_EQUAL_STRING("*",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_DIV,token->type);
    //TEST_ASSERT_EQUAL_STRING("/",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_MINUS,token->type);
    //TEST_ASSERT_EQUAL_STRING("-",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_PLUS,token->type);
    //TEST_ASSERT_EQUAL_STRING("+",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_STRLEN,token->type);
    //TEST_ASSERT_EQUAL_STRING("#",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_DIV,token->type);
    //TEST_ASSERT_EQUAL_STRING("//",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_CONCAT,token->type);
    //TEST_ASSERT_EQUAL_STRING("..",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_EQ,token->type);
    //TEST_ASSERT_EQUAL_STRING("==",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_LT,token->type);
    //TEST_ASSERT_EQUAL_STRING("<",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_LE,token->type);
    //TEST_ASSERT_EQUAL_STRING("<=",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_GT,token->type);
    //TEST_ASSERT_EQUAL_STRING(">",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_GE,token->type);
    //TEST_ASSERT_EQUAL_STRING(">=",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_NOT_EQ,token->type);
    //TEST_ASSERT_EQUAL_STRING("~=",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
}

void TEST_checkWholeExample(void){
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeExamples/fact_iter.ifj21", "r");
    token = get_next_token(srcfile);
    print_token(token);
    while(token->type != TOKEN_EOF){
        token = get_next_token(srcfile);
        print_token(token);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(TEST_blockCom);
    RUN_TEST(TEST_EOF);
    RUN_TEST(TEST_op_id_comment);
    RUN_TEST(TEST_blockCom_stringLit);
    RUN_TEST(TEST_assignOrEqual);
    RUN_TEST(TEST_goodId);
    RUN_TEST(TEST_badId);
    RUN_TEST(TEST_goodNums);
    RUN_TEST(TEST_badNums);
    RUN_TEST(TEST_goodKeywords);
    RUN_TEST(TEST_badKeywords);
    RUN_TEST(TEST_goodOP);
    printf("\n");
    printf("TEST_checkWholeExample MESSAGES:\n");
    printf("----------------------------------------------\n");
    RUN_TEST(TEST_checkWholeExample);
    printf("----------------------------------------------\n");
    printf("\n");    

    // RUN_TEST(test6_id_or_TOKEN_KEYWORD);
    // RUN_TEST(test7_numbers);
    // RUN_TEST(test8_indent);
    // RUN_TEST(test9_function);
    // RUN_TEST(test10_docstring_escape_seq);
    // RUN_TEST(test11_error_indentation);
    // // RUN_TEST(test12_bad_double);

    return UNITY_END();
}


