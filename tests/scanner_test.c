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
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
}

void TEST_EOF(void)
{
    /*testfile

    */
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipEOF.txt", "r");
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
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
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ASSIGN);
    //TEST_ASSERT_EQUAL_STRING(NULL,token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING_MESSAGE("vysl",token->attribute->string,"dojeb");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_MUL,token->type);
    //TEST_ASSERT_EQUAL_STRING_MESSAGE("*",token->attribute,"Dojebalo sa to");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"a");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"vysl");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ASSIGN);
    //TEST_ASSERT_EQUAL_STRING(token->attribute->string,"=");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"vysl");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_DIV);
    //TEST_ASSERT_EQUAL_STRING(token->attribute->string,"/");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"b");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
}

void TEST_blockCom_stringLit(void)
{
    /* test file
        --[[ --[[ ]lol jebaited ]jebaited again ]bait] : TOKEN_INT_LIT "i like pp" else while my mom>your mom<mom pl==siema // if you read~= dumb ]]
        -- --[[ ]lol jebaited ]jebaited again ]bait] : TOKEN_INT_LIT "i like pp" else while my mom>your mom<mom pl==siema // if you read~= dumb ]]
        "i like comments" 200 end 200end
    */
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/good/snipComment.txt", "r");
    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_STR_LIT,token->type);
    TEST_ASSERT_EQUAL_STRING("i\\032like\\032comments",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(200,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_INT32(KEYWORD_END,token->attribute->keyword_type);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("d",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
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
    TEST_ASSERT_EQUAL_STRING("=",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EQ);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"==");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EQ);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"==");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EQ);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"==");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
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
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("jozko99",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_jozko99_",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_99jozko99_",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_",token->attribute->string);
    destroy_token(token);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("a",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
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
    destroy_token(token);
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("jozko",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(9,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("jozko_",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(9,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("_kola",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
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
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(0,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_INT_LIT);
    TEST_ASSERT_EQUAL_INT32(7,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(7.008,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10E10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e-10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e+10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10e10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10e-10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10e+10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10.10E-10,token->attribute->number);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_NUM_LIT);
    TEST_ASSERT_EQUAL_FLOAT(10e+10,token->attribute->number);
    destroy_token(token);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_MINUS);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_EOF,token->type);
    destroy_token(token);

    fclose(srcfile);
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
    TEST_ASSERT_EQUAL_INT32(TOKEN_ERROR,token->type);
    destroy_token(token);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_ID,token->type);
    TEST_ASSERT_EQUAL_STRING("e10",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_ID,token->type);
    TEST_ASSERT_EQUAL_STRING("E10",token->attribute->string);
    destroy_token(token);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    destroy_token(token);    

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);
    destroy_token(token);
    
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    destroy_token(token);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_COMMA,token->type);
    //TEST_ASSERT_EQUAL_STRING(",",token->attribute);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    destroy_token(token);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_NUM_LIT,token->type);
    TEST_ASSERT_EQUAL_FLOAT(0e10,token->attribute->number);
    destroy_token(token);


    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    destroy_token(token);

    token = get_next_token(srcfile);                    
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_LIT,token->type);
    TEST_ASSERT_EQUAL_INT32(10,token->attribute->integer);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);
    
    printf("----------------------------------------------\n");
    printf("\n");
    
    fclose(srcfile);
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
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"else");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_ELSE);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"end");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_END);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"function");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_FUNCTION);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"global");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_GLOBAL);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"if");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_IF);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"integer");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_INTEGER);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"local");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_LOCAL);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"nil");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_NIL);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"number");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_NUMBER);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"require");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_REQUIRE);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"return");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_RETURN);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"string");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_STRING);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"then");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_THEN);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_KEYWORD);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"while");
    TEST_ASSERT_EQUAL_INT32(token->attribute->keyword_type, KEYWORD_WHILE);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);    
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
    destroy_token(token);

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"whilethen");
    destroy_token(token);

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"integers");
    destroy_token(token);

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"localhost");
    destroy_token(token);

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"barbecuenilbeef");
    destroy_token(token);

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"n");
    destroy_token(token);

    token = get_next_token(srcfile); 
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING(token->attribute->string,"il");
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
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
    destroy_token(token);
    //TEST_ASSERT_EQUAL_STRING("*",token->attribute->string);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_DIV,token->type);
    //TEST_ASSERT_EQUAL_STRING("/",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_MINUS,token->type);
    //TEST_ASSERT_EQUAL_STRING("-",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_PLUS,token->type);
    //TEST_ASSERT_EQUAL_STRING("+",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_STRLEN,token->type);
    //TEST_ASSERT_EQUAL_STRING("#",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_INT_DIV,token->type);
    //TEST_ASSERT_EQUAL_STRING("//",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_CONCAT,token->type);
    //TEST_ASSERT_EQUAL_STRING("..",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_EQ,token->type);
    //TEST_ASSERT_EQUAL_STRING("==",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_LT,token->type);
    //TEST_ASSERT_EQUAL_STRING("<",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_LE,token->type);
    //TEST_ASSERT_EQUAL_STRING("<=",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_GT,token->type);
    //TEST_ASSERT_EQUAL_STRING(">",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_GE,token->type);
    //TEST_ASSERT_EQUAL_STRING(">=",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT_EQUAL_INT32(TOKEN_NOT_EQ,token->type);
    //TEST_ASSERT_EQUAL_STRING("~=",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
}

void TEST_Error(void){
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeSnippets/bad/snipError.txt", "r");
    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING(";",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING("&",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING("%",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING("{",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING("}",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING("^",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING("@",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("mom",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING(".",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ID);
    TEST_ASSERT_EQUAL_STRING("om",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_ERROR);
    TEST_ASSERT_EQUAL_STRING("~",token->attribute->string);
    destroy_token(token);

    token = get_next_token(srcfile);
    TEST_ASSERT(token->type == TOKEN_EOF);
    destroy_token(token);

    fclose(srcfile);
}

void TEST_checkWholeExampleFactIter(void){
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeExamples/fact_iter.ifj21", "r");
    token = get_next_token(srcfile);

    while(token->type != TOKEN_EOF){
        
        print_token(token);
        destroy_token(token);
        token = get_next_token(srcfile);

    }
    print_token(token);
    destroy_token(token);

    fclose(srcfile);
}

void TEST_checkWholeExampleFactRec(void){
    token_t *token;
    FILE *srcfile = fopen("tests/test_data/codeExamples/fact_recur.ifj21", "r");
    token = get_next_token(srcfile);

    while(token->type != TOKEN_EOF){
        print_token(token);
        destroy_token(token);
        token = get_next_token(srcfile);
    }
    print_token(token);
    destroy_token(token);

    fclose(srcfile);
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
    RUN_TEST(TEST_Error);
    printf("\n");
    printf("TEST_checkFactIter MESSAGES:\n");
    printf("----------------------------------------------\n");
    RUN_TEST(TEST_checkWholeExampleFactIter);
    printf("----------------------------------------------\n");
    printf("\n");

    printf("\n");
    printf("TEST_checkFactRec MESSAGES:\n");
    printf("----------------------------------------------\n");
    RUN_TEST(TEST_checkWholeExampleFactRec);
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


