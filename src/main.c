#include <stdio.h>
#include "include/parser.h"
#include "include/error.h"
#include <unistd.h> // sleep()

#define PRINT_ERROR \
    error_message("Scanner", ERR_LEX, " - unknown token '%s'", token->attribute->string);

int main(int argc, char *argv[])
{
    /* This code demonstrates the scanners work */
#if 1
    FILE *f;
        // if ((f = fopen(argv[1], "r")) == NULL) {
        //     fprintf(stderr, "Failed to open file '%s'\n", argv[1]);
        //     return ERR_INTERNAL;
        // }
        int i = 0;
        while(i<37){
            printf("\n");
            switch (i)
            {
            case 0 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.1.ifj21", "r");
                     printf("3.1\n");
                break;
            case 1 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.2.ifj21", "r");
                     printf("3.2\n");
                break;
            case 2 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.3.ifj21", "r");
                     printf("3.3\n");
                break;
            case 3 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.4.ifj21", "r");
                     printf("3.4\n");
                break;
            case 4 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.5.ifj21", "r");
                     printf("3.5\n");
                break;
            case 5 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.6.ifj21", "r");
                     printf("3.6\n");
                break;
            case 6 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.7.ifj21", "r");
                     printf("3.7\n");
                break;
            case 7 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.8.ifj21", "r");
                     printf("3.8\n");
                break;
            case 8 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.1.ifj21", "r");
                     printf("5.1\n");
                break;
            case 9 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.2.ifj21", "r");
                    printf("5.2\n");
                break;
            case 10 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.3.ifj21", "r");
            printf("5.3\n");
                break;
            case 11 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.4.ifj21", "r");
            printf("5.4\n");
                break;
            case 12 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.5.ifj21", "r");
            printf("5.5\n");
                break;
            case 13 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.6.ifj21", "r");
            printf("5.6\n");
                break;
            case 14 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.7.ifj21", "r");
            printf("5.7\n");
                break;
            case 15 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.8.ifj21", "r");
            printf("5.8\n");
                break;
            case 16 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.9.ifj21", "r");
            printf("5.9\n");
                break;
            case 17 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.10.ifj21", "r");
            printf("5.10\n");
                break;
            case 18 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_6.1.ifj21", "r");
                break;
            // case 19 : //f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_7.1.ifj21", "r"); //seg
            //     printf("\nsegfault\n");.1
            //     break;
            case 19 : f = fopen("tests/test_data/CodeStructureSnippets/bad/var_dec_syn_err.ifj21", "r");
                break;
            case 20 : f = fopen("tests/test_data/CodeStructureSnippets/good/func_dec.ifj21", "r");
                break;
            // case 21 : f = fopen("tests/test_data/CodeStructureSnippets/good/multiassign.ifj21", "r");    //seg
            //     break;
            case 21 : f = fopen("tests/test_data/CodeStructureSnippets/good/var_def.ifj21", "r");
                break;
            case 22 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.1.ifj21", "r");
                      printf("4.1\n");
                break;
            case 23 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.2.ifj21", "r");
            printf("4.2\n");
                break;
            case 24 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.3.ifj21", "r");
            printf("4.3\n");
                break;
            case 25 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.4.ifj21", "r");
            printf("4.4\n");
                break;
            case 26 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.5.ifj21", "r");
            printf("4.5\n");
                break;
            case 27 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.6.ifj21", "r");
            printf("4.6\n");
                break;
            case 28 : f = fopen("tests/test_data/CodeStructureSnippets/good/visibility.ifj21", "r");
                break;
            case 29 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.9.ifj21", "r");
                break;
            case 30 : f = fopen("tests/test_data/CodeStructureSnippets/good/filip1.ifj21", "r");
                break;
            case 31 : f = fopen("tests/test_data/CodeStructureSnippets/good/snipWrite.ifj21", "r");
                break;
            case 32 : f = fopen("tests/test_data/CodeStructureSnippets/good/expr_good1.ifj21", "r");
            printf("expr1good\n");
                break;
            case 33 : f = fopen("tests/test_data/CodeStructureSnippets/good/expr_good2.ifj21", "r");
            printf("expr2good\n");
                break;
            case 34 : f = fopen("tests/test_data/CodeStructureSnippets/good/expr_good3.ifj21", "r");
            printf("expr3good\n");
                break;
            case 35 : f = fopen("tests/test_data/CodeStructureSnippets/bad/expr_bad1.ifj21", "r");
            printf("epxr1bad\n");
                break;
            case 36 : f = fopen("tests/test_data/CodeStructureSnippets/bad/expr_bad2.ifj21", "r");
            printf("epxr2bad\n");
                break;
            default:
                break;
            }
            printf("----------------------------------\n");
            // static int three = 1;
            // static int four = 1;
            // static int five = 1;
            int a = parser_parse(f);
            // if(a == 3){
            //     three ++;
            //     printf("\n3.%d\n",three);
            // }
            // if(a == 5){
            //     five ++;
            //     printf("\n5.%d\n",five);
            // }
            // if(a == 4){
            //     four ++;
            //     printf("\n5.%d\n",four);
            // }
            //printf("%d\n",a);
            if(a == 0){
                printf("subor %d je EXIT_OK\n",i+1);
            }
            
            //fclose(f);
            i++;
           // printf("----------------------------------\n");
        }
#endif
#if 0
    FILE *f = stdin;
    if (argc > 1) {
        if ((f = fopen(argv[1], "r")) == NULL) {
            fprintf(stderr, "Failed to open file '%s'\n", argv[1]);
            return ERR_INTERNAL;
        }
    }
    return parser_parse(f);
#endif
}