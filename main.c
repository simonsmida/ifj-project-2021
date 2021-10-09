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
        printf("File is given\n");
    #else 
        input = stdin;
        printf("Input file is not given\n");
    #endif

    read_input(input);
    return 0;
}