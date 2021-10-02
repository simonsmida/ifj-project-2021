/**
 * @file  lex_an.c
 * @authors  Krištof Šiška - xsiska16
 * @date  1.10.2021
 *
 * Compiled : gcc version 9.3.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "lex_an.h"

#define DEFAULT_STATE 0 
#define START_COMMENT 1 // Used when previous state was default and a '-' char was found
#define ON_COMMENT 2 // Found "--" in text
#define CHECK_COMMENT_BLOCK 3 // Used when we are on ON_COMMENT state and we check for '[' char
#define INSIDE_LINE_COMMENT 4 // Found "--" in text but not a "--[[" Identifying block comment
#define INSIDE_BLOCK_COMMENT 5 // Found "--[[" in source code and we are ignoring all chars until we find "]]"
#define CHECK_END_BLOCK_COMMENT 6 // Found a ']' in block comment

#define NUM_OF_KEYWORDS 12


enum token_type { keyword, identifier, operator, separator } ;
// All the keywords used in IFJ21
char *keywords[] =  { "do", "else", "end", "function",
                    "global", "if", "local", "nil",
                    "require", "return", "then", "while" };

int read_input(FILE *file){
    int state = 0;
    int c;
    FILE *output_file;
    output_file = fopen("output.txt", "w");
    while ( (c = fgetc(file)) != EOF ){
        // Ignoring all the whitespaces
        if ( isspace(c) && c != '\n'){
            continue;
    }
        switch(state) {
            
            case DEFAULT_STATE:
                if (c == '-'){
                    state = START_COMMENT;
                } 
                else {
                    fputc(c, output_file);
                }
                break;

            case START_COMMENT:
                if (c == '-'){
                    state = ON_COMMENT;
                }
                else {
                    fputc('-', output_file);
                    fputc(c , output_file);
                }
                break;
        
            case ON_COMMENT:
                if (c == '[' ){
                    state = CHECK_COMMENT_BLOCK;
                }
                else {
                    state = INSIDE_LINE_COMMENT;
                }
                break;

            case CHECK_COMMENT_BLOCK:
                if (c == '[' ){
                    state = INSIDE_BLOCK_COMMENT;
                }
                else {
                    state = INSIDE_LINE_COMMENT;
                }
                break;
        
            case INSIDE_LINE_COMMENT:
                if (c == '\n'){
                    state = DEFAULT_STATE;
                }
                break;
            
            case INSIDE_BLOCK_COMMENT:
                if (c == ']' ){
                    state = CHECK_END_BLOCK_COMMENT;
                }
                break; 
        
            case CHECK_END_BLOCK_COMMENT:
                if (c == ']' ){
                    state = DEFAULT_STATE;
                }
                else {
                    state = INSIDE_BLOCK_COMMENT;
                }
                break;
        
        
        
        }


    }
    return 0;
}




