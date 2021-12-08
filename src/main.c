#include <stdio.h>
#include "include/parser.h"
#include "include/error.h"
#include <unistd.h> // sleep()

#define PRINT_ERROR \
    error_message("Scanner", ERR_LEX, " - unknown token '%s'", token->attribute->string);

int main(int argc, char *argv[])
{
    /* This code is used for testing */
    

#if TESTING
    
    int passed = 0;
    FILE *f;
    int i = 0;
    int all = 45;
    int failed[50];
    int j = 0;
        while(i<45){
         
            printf("\n");
            switch (i)
            {
            case 0 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.1.ifj21", "r");
                     printf("%d. [3.1]\n",i);
                break;
            case 1 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.2.ifj21", "r");
                     printf("%d. [3.2]\n",i);
                break;
            case 2 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.3.ifj21", "r");
                     printf("%d. [3.3]\n",i);
                break;
            case 3 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.4.ifj21", "r");
                     printf("%d. [3.4]\n",i);
                break;
            case 4 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.5.ifj21", "r");
                     printf("%d. [3.5]\n",i);
                break;
            case 5 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.6.ifj21", "r");
                     printf("%d. [3.6]\n",i);
                break;
            case 6 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.7.ifj21", "r");
                     printf("%d. [3.7]\n",i);
                break;
            case 7 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.8.ifj21", "r");
                     printf("%d. [3.8]\n",i);
                break;
            case 8 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_3.9.ifj21", "r");
                     printf("%d. [3.9]\n",i);
                break;
            case 9 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.1.ifj21", "r");
                     printf("%d. [5.1]\n",i);
                break;
            case 10 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.2.ifj21", "r");
                    printf("%d. [5.2]\n",i);
                break;
            case 11 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.3.ifj21", "r");
            printf("%d. [5.3]\n",i);
                break;
            case 12 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.4.ifj21", "r");
            printf("%d. [5.4]\n",i);
                break;
            case 13 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.5.ifj21", "r");
            printf("%d. [5.5]\n",i);
                break;
            case 14 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.6.ifj21", "r");
            printf("%d. [5.6]\n",i);
                break;
            case 15 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.7.ifj21", "r");
            printf("%d. [5.7]\n",i);
                break;
            case 16 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.8.ifj21", "r");
            printf("%d. [5.8]\n",i);
                break;
            case 17 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.9.ifj21", "r");
            printf("%d. [5.9]\n",i);
                break;
            case 18 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_5.10.ifj21", "r");
            printf("%d. [5.10]\n",i);
                break;
            case 19 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_6.1.ifj21", "r");
            printf("%d. [6.1]\n",i);
                break;
            case 20 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.1.ifj21", "r");
                      printf("%d. [4.1]\n",i);
                break;
            case 21 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.2.ifj21", "r");
            printf("%d. [4.2]\n",i);
                break;
            case 22 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.3.ifj21", "r");
            printf("%d. [4.3]\n",i);
                break;
            case 23 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.4.ifj21", "r");
            printf("%d. [4.4]\n",i);
                break;
            case 24 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.5.ifj21", "r");
            printf("%d. [4.5]\n",i);
                break;
            case 25 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_4.6.ifj21", "r");
            printf("%d. [4.6]\n",i);
                break;
            case 26 : f = fopen("tests/test_data/CodeStructureSnippets/bad/var_dec_syn_err.ifj21", "r");
                printf("%d.\nShould be SYNTAX_ERROR\n",i);
                break;
            case 27 : f = fopen("tests/test_data/CodeStructureSnippets/good/func_dec.ifj21", "r");
                printf("%d.\nShould be EXIT_OK\n",i);
                break;
            case 28 : f = fopen("tests/test_data/CodeStructureSnippets/good/var_def.ifj21", "r");
                break;
            case 29 : f = fopen("tests/test_data/CodeStructureSnippets/good/visibility.ifj21", "r");
                break;            
            case 30 : f = fopen("tests/test_data/CodeStructureSnippets/good/filip1.ifj21", "r");
                break;
            case 31 : f = fopen("tests/test_data/CodeStructureSnippets/good/snipWrite.ifj21", "r");
                printf("%d. LAST of EXIT_OK\n",i);
                break;
            case 32 : f = fopen("tests/test_data/CodeStructureSnippets/good/expr_good1.ifj21", "r");
            printf("%d. [expr1good]\n",i);
                break;
            case 33 : f = fopen("tests/test_data/CodeStructureSnippets/good/expr_good2.ifj21", "r");
            printf("%d. [expr2good]\n",i);
                break;
            case 34 : f = fopen("tests/test_data/CodeStructureSnippets/good/expr_good3.ifj21", "r");
            printf("%d. [expr3good]\n",i);
                break;
            case 35 : f = fopen("tests/test_data/CodeStructureSnippets/bad/expr_bad1.ifj21", "r");
            printf("%d. [epxr1bad]\n",i);
                break;
            case 36 : f = fopen("tests/test_data/CodeStructureSnippets/bad/expr_bad2.ifj21", "r");
            printf("%d. [epxr2bad]\n",i);
                break;
            case 37 : f = fopen("tests/test_data/CodeStructureSnippets/good/example3.tl", "r");
            printf("\nKRIVKA START:\n");
            printf("%d. [example3.tl]\n",i);
                break;
            case 38 : f = fopen("tests/test_data/CodeStructureSnippets/good/fact_iter.ifj21", "r");
            printf("%d. [fact_iter.ifj21]\n", i);
                break;
            case 39 : f = fopen("tests/test_data/CodeStructureSnippets/good/fact_recur.ifj21", "r");
            printf("%d. [fact_recur.ifj21]\n",i);
                break;
            case 40 : f = fopen("tests/test_data/CodeStructureSnippets/good/fun.tl", "r");
            printf("%d. [fun.tl]\n",i);
                break;
            case 41 : f = fopen("tests/test_data/CodeStructureSnippets/good/hello.tl", "r");
            printf("%d. [hello.tl]\n",i);
                break;
            case 42 : f = fopen("tests/test_data/CodeStructureSnippets/good/substr.tl", "r");
            printf("%d. [substr.tl]\n",i);
            break;
            case 43 : f = fopen("tests/test_data/CodeStructureSnippets/good/multiassign.ifj21", "r");    //seg
            printf("\nLAST of KRIVKA:\n");
            printf("%d. [multiassign.tl]\n",i);
                break;   
            case 44 : f = fopen("tests/test_data/CodeStructureSnippets/bad/sem_err_7.1.ifj21", "r"); //seg
                 printf("%d. [7.1]\n",i);
                 break;
            default:
                break;
            }
            printf("------------------------------------\n");
            int a = parser_parse(f);
            if(a == 0){
                printf("EXIT_OK\n");
            }
            if(i>=0 && i<= 8){
                
                if(a == 3){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                }
                else{
                    if(i== 6 || i==7){
                        printf("--------------|PASSED|--------------\n");
                        passed++;
                    }
                        

                    else{
                        printf("--------------|FAILED|--------------\n");
                        failed[j] = i;
                        j++;
                    }
                        

                }
                    
            }
            if((i>=9 && i<= 18)){
                if(a == 5){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                }
                    
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;

                }
            }   
            if(i>=20 && i<= 25){
                if(a == 4){
                    printf("--------------|PASSED|--------------\n");
                    passed++;           
                }
                    
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;
                }
            }
            if(i>=32 && i<= 34){
                if(a == 0){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                }
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;
                }
            }
            if((i>=35 && i<= 36) || i == 19){
                if(a == 6 || a == 4){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                }
                    
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;
                    
                }
            }
            if(i>=27 && i<= 31){
                if(a == 0){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                    
                }
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;
                }
                    
            }
            if(i == 26){
                if(a == 2){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                }
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;
                }
                    
            }
            if(i>=37 && i<= 43){
                if(a == 0){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                    
                }
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;
                }
                    
            }
            if(i == 44){
                if(a == 7){
                    printf("--------------|PASSED|--------------\n");
                    passed++;
                    
                }
                else{
                    printf("--------------|FAILED|--------------\n");
                    failed[j++] = i;
                }
            }
            
            //fclose(f);
            i++;
           // printf("-----------------------------\n");
        }
        printf("##############################################\n");
        printf("PASSED %d/%d\n", passed,all);
        int n = 0;
        printf("FAILED: ");
        while(n != j){
            printf("%d ",failed[n]);
            n++;
        }
        printf("\n");

#endif
#if NORMAL
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