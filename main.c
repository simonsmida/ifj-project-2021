/**
 * @file  main.c
 * @authors  Krištof Šiška - xsiska16
 * @date  1.10.2021
 *
 * Compiled : gcc version 9.3.0
 */



#include <stdio.h>
#include "lex_an.h"

int main(int argc, char **argv){
    FILE *input;
    #ifdef DEBUG
        input = fopen(argv[1], "r");
        printf("Yup\n");
    #else 
        input = stdin;
        printf("nope\n");
    #endif

    read_input(input);
    return 0;
}