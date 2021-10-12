/**
 * @file  lex_an.c
 * @authors  Krištof Šiška - xsiska16
 * @date  1.10.2021
 *
 * Compiled : gcc version 9.3.0
 */


#include "lex_an.h"

#define DEFAULT_STATE 0 
#define START_COMMENT_OR_MINUS 1 // Used when previous state was default and a '-' char was found
#define ON_COMMENT 2 // Found "--" in text
#define CHECK_COMMENT_BLOCK 3 // Used when we are on ON_COMMENT state and we check for '[' char
#define INSIDE_LINE_COMMENT 4 // Found "--" in text but not a "--[[" Identifying block comment
#define INSIDE_BLOCK_COMMENT 5 // Found "--[[" in source code and we are ignoring all chars until we find "]]"
#define CHECK_END_BLOCK_COMMENT 6 // Found a ']' in block comment
#define ID_OR_KEYWORD 7 // 
#define STRING_LITERAL 8 // Found '"' in text signalizing string literal
#define OPERATOR 9 //
#define NUMBER_SEQUENCE 10 // Found a number
#define DOUBLE_DOT_SEQUENCE 11
#define DOUBLE_E_SEQUENCE 12
#define DOUBLE_E_PLUS_MINUS_SEQUENCE 13
#define BACKSLASH_SEQUENCE 14


#define SIZE_STRING 10

#define NUM_OF_KEYWORDS 12
#define NUM_OF_VAR_TYPE 5
#define NUM_OF_OPERATORS 9

bool is_operator(int c);
enum token_type { keyword, identifier, operator, separator, equals, colon } ;
// All the keywords used in IFJ21
char *keywords[] =  { "do", "else", "end", "function",
                    "global", "if", "local", "nil",
                    "require", "return", "then", "while" };

char *variable_type[] = {   "string", "integer", "nil", "number",
                            "double" 
                        };

char first_operators[] = { '#', '*', '/', '+', '-', '.', '<', '>', '~'};

char second_operators[] = { '/', '=', '.' };


int read_input(FILE *file){
    char string[40]; //TODO:  Use dynamic string not this
    int string_index = 0;
    int state = 0;
    int c;
    char escape_seq_bufer[5];
    int escape_seq_index = 0;
    while ( (c = fgetc(file)) != EOF ){
        
        
        switch(state) {
            
            case DEFAULT_STATE:
                // Ignoring all the whitespaces if we are in default state
                if ( isspace(c) && c != '\n'){
                break;
            }
                if (c == '-'){
                    state = START_COMMENT_OR_MINUS;
                }
                    // Found number
                else if (isdigit(c)){
                    append_character(c, string, &string_index);    
                    state = NUMBER_SEQUENCE;
                }
                    // found ID
                else if (isalpha(c) || c == '_'){
                    append_character(c, string, &string_index);
                    state = ID_OR_KEYWORD;
                }
                    // Found String literal
                else if (c == '"'){
                    append_character(c, string, &string_index);
                    state = STRING_LITERAL;
                }
                    // Found separator
                else if ( c == ',' ){
                    generate_token("," , &string_index, "separator");
                }
                    // Found equals '='
                else if ( c == '=' ){
                    generate_token("=", &string_index, "equals");
                }
                else if ( c == '(' ){
                    generate_token("(", &string_index, "L_PAREN");
                }
                else if ( c == ')' ){
                    generate_token(")", &string_index, "R_PAREN");
                }
                else if ( c == ':'){
                    generate_token(":", &string_index, "colon");
                }
                    // Found an operator
                else if ( is_operator(c) ){
                    state = OPERATOR;
                    append_character(c, string, &string_index);
                }
                
                break;

            case START_COMMENT_OR_MINUS:
                if (c == '-'){
                    state = ON_COMMENT;
                }
                else if (is_operator(c)) {
                    printf("Lexical error : '-' cannot be followed by %c\n", c);
                    exit(1);
                }
                else {
                    generate_token("-", &string_index, "operator");
                    ungetc(c, file);
                    state = DEFAULT_STATE;
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
            
            case STRING_LITERAL:
                if (c == '"'){
                    append_character(c, string, &string_index);
                    generate_token(string, &string_index, "STRING_LITERAL");
                    state = DEFAULT_STATE;
                }
                else if ( c == '\\' ){
                    state = BACKSLASH_SEQUENCE;
                }
                else {
                    append_character(c, string, &string_index);
                }
                break;

            case NUMBER_SEQUENCE:
                if (c == '.'){
                    append_character(c, string, &string_index);
                    state = DOUBLE_DOT_SEQUENCE;
                }        
                else if (c == 'e' || c == 'E'){
                    append_character(c, string, &string_index);
                    state = DOUBLE_E_SEQUENCE;
                }
                else if ( (c >= '0') && (c <= '9' ) ){
                    append_character(c, string, &string_index);
                }
                else if (isspace(c)){
                     generate_token(string, &string_index, "INT");
                    state = DEFAULT_STATE;
                }
                else {
                    printf("Lexical error, NUM_SEQ %c\n", c);
                    // Warning - lexical error
                }
                break;
        
            case DOUBLE_DOT_SEQUENCE:
                if (c == 'e' || c == 'E'){
                    append_character(c, string, &string_index);
                    state = DOUBLE_E_SEQUENCE;
                }
                else if (c >= '0' && c <= '9' ){
                    append_character(c, string, &string_index);
                }
                else if (isspace(c)){
                     generate_token(string, &string_index, "DOUBLE");
                    state = DEFAULT_STATE;
                }
                else {
                    printf("Lexical error, DOUBLE_DOT_SEQ\n");
                    // Warning - lexical error
                }
                break;

            case DOUBLE_E_SEQUENCE:
                if (c == '+' || c == '-'){
                    append_character(c, string, &string_index);
                    state = DOUBLE_E_PLUS_MINUS_SEQUENCE;
                }
                else if (c >= '0' && c <= '9' ){
                    append_character(c, string, &string_index);
                }
                else if (isspace(c)){
                    generate_token(string, &string_index, "DOUBLE");
                    state = DEFAULT_STATE;
                }
                else {
                    printf("Lexical error, DOUBLE_E_SEQ\n");
                    // Warning - lexical error
                }
                break;

            case ID_OR_KEYWORD:
                if (isalpha(c) || c == '_'){
                    append_character(c, string, &string_index);
                }
                else {
                    generate_token(string, &string_index, NULL);
                    state = DEFAULT_STATE;
                }
                break; 

            case OPERATOR:
                if (string[string_index - 1] == '/' && c == '/'){
                    append_character(c, string, &string_index);
                    append_character('\0', string, &string_index);
                    generate_token(string, &string_index, "operator");
                    state = DEFAULT_STATE;
                }
                else if (string[string_index - 1] == '.' && c == '.'){
                    append_character(c, string, &string_index);
                    append_character('\0', string, &string_index);
                    generate_token(string, &string_index, "operator");
                    state = DEFAULT_STATE;
                }
                else if (string[string_index - 1] == '=' && c == '='){
                    append_character(c, string, &string_index);
                    append_character('\0', string, &string_index);
                    generate_token(string, &string_index, "operator");
                    state = DEFAULT_STATE;
                }
                else if (string[string_index - 1] == '<' && c == '='){
                    append_character(c, string, &string_index);
                    append_character('\0', string, &string_index);
                    generate_token(string, &string_index, "operator");
                    state = DEFAULT_STATE;
                }
                else if (string[string_index - 1] == '>' && c == '='){
                    append_character(c, string, &string_index);
                    append_character('\0', string, &string_index);
                    generate_token(string, &string_index, "operator");
                    state = DEFAULT_STATE;
                }
                else if (string[string_index - 1] == '~' && c == '='){
                    append_character(c, string, &string_index);
                    append_character('\0', string, &string_index);
                    generate_token(string, &string_index, "operator");
                    state = DEFAULT_STATE;
                }
                else if (string[string_index - 1] == '~' && c != '='){
                    printf("Lexical error, ~ has to be followed by =\n");
                    exit(1); // TODO : Think of a way to end the program without using exit              
                    // Lexical analysis error - incorrect operator use (if using ~, = has to follow immediately)
                }
                else if (is_operator(c) || c == '='){
                    printf("Lexical error, %c cannot be followed by %c\n", string[string_index - 1], c);
                    exit(1);
                }
                else {
                    append_character('\0', string, &string_index);
                    generate_token(string, &string_index, "operator");
                }
                state = DEFAULT_STATE;
                break;

            case BACKSLASH_SEQUENCE:
                if (c == '\"' || c == '\\' ){
                    append_character(c, string, &string_index);
                    state = STRING_LITERAL;
                }
                else if ( c == 'n' ){
                    append_character('\n', string, &string_index);
                    state = STRING_LITERAL;
                }
                else if ( c == 't' ){
                    append_character('\t', string, &string_index);
                    state = STRING_LITERAL;
                }
                else if(c >= '0' && c <= '9' ){
                    append_character(c, escape_seq_bufer, &escape_seq_index);
                    if (escape_seq_index == 3){
                        escape_seq_bufer[escape_seq_index] = '\0';
                        int num = strtold(escape_seq_bufer, NULL);
                        append_character(num, string, &string_index);
                        state = STRING_LITERAL;
                        escape_seq_index = 0;
                    }
                }
                else {
                    append_character(c, string , &string_index);
                    state = STRING_LITERAL;
                }
            default:
                break;

        }

    }
    return 0;
}




void print_string(char *string, int *string_index){
    string[*string_index] = '\0';
    printf("%s\n", string);
    *string_index = 0;
}

void append_character(int c, char *string, int *string_index){
    string[*string_index] = c;
    *string_index = *string_index + 1;
    return;
}

bool is_operator(int c){
    for (int i = 0; i < NUM_OF_OPERATORS; i++){
        if (c == first_operators[i]){
            return true;
        }
    }

    return false;
}

void generate_token(char *string, int *string_index, char *type){
    // If type is Null, we are signalizing, that we couldnt
    // determine type of token during tokenization
    if (type == NULL){
        string[*string_index] = '\0';
        if (is_keyword(string)){
            printf("%s - %s", string, "keyword");
        }
        else if (is_variable_type(string)){
            printf("%s - %s", string,"variable_type");
        }
        else {
            printf("%s - %s", string, "ID");
        }
    }
    else if (!strcmp(type, "operator")){
        printf("%s - %s", string, "OPERATOR");
    }
    else if (!strcmp(type, "STRING_LITERAL")){
        string[*string_index] = '\0';
        printf("%s - %s", string, "STRING_LITERAL");
    }

    else {
        printf("%s - %s", string, type);
    }
    printf("\n");
    // Null the string index
    *string_index = 0;

    return;
}

bool is_keyword(char *string){
    for (int i = 0; i < NUM_OF_KEYWORDS; i++){
        if (!strcmp(string, keywords[i])){
            return true;
        }
    }

    return false;
}

bool is_variable_type(char *string){
    for (int i = 0; i < NUM_OF_VAR_TYPE; i++){
        if (!strcmp(string, variable_type[i])){
            return true;
        }
    }
    return false;
}