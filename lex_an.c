/**
 * @file  lex_an.c
 * @authors  Krištof Šiška - xsiska16
 * @date  1.10.2021
 *
 * Compiled : gcc version 9.3.0
 */

#include "lex_an.h"
#include "buffer.h"
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
// All the keywords used in IFJ21
char *keywords[] =  { "do", "else", "end", "function",
                    "global", "if", "local", "nil",
                    "require", "return", "then", "while" };

char *variable_type[] = {   "string", "integer", "nil", "number",
                            "double" 
                        };

char first_operators[] = { '#', '*', '/', '+', '-', '.', '<', '>', '~'};

char second_operators[] = { '/', '=', '.' };



TOKEN_T *read_input(FILE *file){
    
    STRING_T *buffer = NULL;

    buffer = init_buffer();

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
                    append_character(buffer, c);;    
                    state = NUMBER_SEQUENCE;
                }
                    // found ID
                else if (isalpha(c) || c == '_'){
                    append_character(buffer, c);;
                    state = ID_OR_KEYWORD;
                }
                    // Found String literal
                else if (c == '"'){
                    append_character(buffer, c);
                    state = STRING_LITERAL;
                }
                    // Found separator
                else if ( c == ',' ){
                    append_character(buffer, c);
                    return generate_token(buffer, "separator");
                }
                    // Found equals '='
                else if ( c == '=' ){
                    append_character(buffer , c);
                    return generate_token(buffer,  "equals");
                }
                else if ( c == '(' ){
                    append_character(buffer, c);
                    return generate_token(buffer,  "L_PAREN");
                }
                else if ( c == ')' ){
                    append_character(buffer, c);
                    return generate_token(buffer, "R_PAREN");
                }
                else if ( c == ':'){
                    append_character(buffer, c);
                    return generate_token(buffer,  "colon");
                }
                    // Found an operator
                else if ( is_operator(c) ){
                    state = OPERATOR;
                    append_character(buffer, c);;
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
                    return generate_token(buffer,  "operator");
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
                   append_character(buffer, c);;
                    return generate_token(buffer , "STRING_LITERAL");
                    state = DEFAULT_STATE;
                }
                else if ( c == '\\' ){
                    state = BACKSLASH_SEQUENCE;
                }
                else {
                    append_character(buffer, c);;
                }
                break;

            case NUMBER_SEQUENCE:
                if (c == '.'){
                    append_character(buffer, c);
                    state = DOUBLE_DOT_SEQUENCE;
                }        
                else if (c == 'e' || c == 'E'){
                    append_character(buffer, c);
                    state = DOUBLE_E_SEQUENCE;
                }
                else if ( (c >= '0') && (c <= '9' ) ){
                    append_character(buffer, c);;
                }
                else if (isspace(c)){
                     return generate_token(buffer,  "INT");
                    state = DEFAULT_STATE;
                }
                else {
                    printf("Lexical error, NUM_SEQ %c\n", c);
                    // Warning - lexical error
                }
                break;
        
            case DOUBLE_DOT_SEQUENCE:
                if (c == 'e' || c == 'E'){
                    append_character(buffer, c);
                    state = DOUBLE_E_SEQUENCE;
                }
                else if (c >= '0' && c <= '9' ){
                    append_character(buffer, c);
                }
                else if (isspace(c)){
                     return generate_token(buffer,  "DOUBLE");
                    state = DEFAULT_STATE;
                }
                else {
                    printf("Lexical error, DOUBLE_DOT_SEQ\n");
                    // Warning - lexical error
                }
                break;

            case DOUBLE_E_SEQUENCE:
                if (c == '+' || c == '-'){
                    append_character(buffer, c);;
                    state = DOUBLE_E_PLUS_MINUS_SEQUENCE;
                }
                else if (c >= '0' && c <= '9' ){
                    append_character(buffer, c);
                }
                else if (isspace(c)){
                    return generate_token(buffer, "DOUBLE");
                    state = DEFAULT_STATE;
                }
                else {
                    printf("Lexical error, DOUBLE_E_SEQ\n");
                    // Warning - lexical error
                }
                break;

            case ID_OR_KEYWORD:
                if (isalpha(c) || c == '_'){
                    append_character(buffer, c);;
                }
                else {
                    return generate_token(buffer,  NULL);
                    state = DEFAULT_STATE;
                }
                break; 

            case OPERATOR:
                if (buffer->string[buffer->current_index] == '/' && c == '/'){
                    append_character(buffer, c);
                    append_character(buffer, '\0');
                    return generate_token(buffer,  "operator");
                    state = DEFAULT_STATE;
                }
                else if (buffer->string[buffer->current_index] == '.' && c == '.'){
                    append_character(buffer, c);
                    append_character(buffer, '\0');
                    return generate_token(buffer,  "operator");
                    state = DEFAULT_STATE;
                }
                else if (buffer->string[buffer->current_index] == '=' && c == '='){
                    append_character(buffer, c);
                    append_character(buffer,  '\0');
                    return generate_token(buffer,  "operator");
                    state = DEFAULT_STATE;
                }
                else if (buffer->string[buffer->current_index] == '<' && c == '='){
                    append_character(buffer, c);
                    append_character(buffer,  '\0');
                    return generate_token(buffer, "operator");
                    state = DEFAULT_STATE;
                }
                else if (buffer->string[buffer->current_index] == '>' && c == '='){
                    append_character(buffer, c);
                    append_character(buffer,  '\0');
                    return generate_token(buffer, "operator");
                    state = DEFAULT_STATE;
                }
                else if (buffer->string[buffer->current_index] == '~' && c == '='){
                    append_character(buffer, c);
                    append_character( buffer, '\0');
                    return generate_token(buffer, "operator");
                    state = DEFAULT_STATE;
                }
                else if (buffer->string[buffer->current_index] == '~' && c != '='){
                    printf("Lexical error, ~ has to be followed by =\n");
                    exit(1); // TODO : Think of a way to end the program without using exit              
                    // Lexical analysis error - incorrect operator use (if using ~, = has to follow immediately)
                }
                else if (is_operator(c) || c == '='){
                    printf("Lexical error, %c cannot be followed by %c\n", buffer->string[buffer->current_index], c);
                    exit(1);
                }
                else {
                    append_character(buffer, '\0');
                    return generate_token(buffer, "operator");
                }
                state = DEFAULT_STATE;
                break;

            case BACKSLASH_SEQUENCE:
                if (c == '\"' || c == '\\' ){
                    append_character(buffer, c);;
                    state = STRING_LITERAL;
                }
                else if ( c == 'n' ){
                    append_character(buffer, '\n');
                    state = STRING_LITERAL;
                }
                else if ( c == 't' ){
                    append_character(buffer, '\t' );
                    state = STRING_LITERAL;
                }
                else if(c >= '0' && c <= '9' ){
                    append_character( buffer, c );
                    if (escape_seq_index == 3){
                        escape_seq_bufer[escape_seq_index] = '\0';
                        int num = strtold(escape_seq_bufer, NULL);
                        append_character(buffer, num );
                        state = STRING_LITERAL;
                        escape_seq_index = 0;
                    }
                }
                else {
                    append_character(buffer, c);
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



bool is_operator(int c){
    for (int i = 0; i < NUM_OF_OPERATORS; i++){
        if (c == first_operators[i]){
            return true;
        }
    }

    return false;
}

TOKEN_T *generate_token(STRING_T *buffer,  char *type){
    // If type is Null, we are signalizing, that we couldnt
    // determine type of token during tokenization
    TOKEN_T *token = malloc (sizeof(TOKEN_T));
    if (token == NULL){
        fprintf(stderr,"Intern malloc problem");
        exit(1);
    }
    
    token->value = malloc(buffer->size);
    
    if (token->value == NULL){
        fprintf(stderr,"Intern malloc problem");
        exit(1);
    }
  
    buffer->string[buffer->current_index] = '\0';

    strcpy(token->value, buffer->string);
    
    if (type == NULL){
        buffer->string[buffer->current_index] = '\0';
        if (is_keyword(buffer->string)){
            printf("%s - %s", buffer->string, "keyword");
             destroy_buffer(buffer);
             return token;
        }
        else if (is_variable_type(buffer->string)){
            printf("%s - %s", buffer->string,"variable_type");
             destroy_buffer(buffer);
            return token;
        }
        else {
            printf("%s - %s", buffer->string, "ID");
            token->TYPE = identifier;
            destroy_buffer(buffer);
            return token;
        }
    }
    else if (!strcmp(type, "operator")){
        printf("%s - %s", buffer->string, "OPERATOR");
        token->TYPE = operator;
        destroy_buffer(buffer);
        return token;
    }
    else if (!strcmp(type, "STRING_LITERAL")){
        buffer->string[buffer->current_index] = '\0';
        printf("%s - %s", buffer->string, "STRING_LITERAL");
        token->TYPE = str_literal;
        destroy_buffer(buffer);
        return token;
    }

    else {
        printf("%s - %s", buffer->string, type);
    }
    printf("\n");
    // Null the string index
    destroy_buffer(buffer);
    
    return token;
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