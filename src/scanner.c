/**
 * @file  scanner.c
 * @brief Scanner implementation
 * 
 * @authors  Krištof Šiška - xsiska16
 *           Šimon Šmída   - xsmida03
 * @date  1.10.2021
 *
 * Compiled : gcc version 9.3.0
 */

#include "include/scanner.h"
#include "include/buffer.h"


// All the keywords used in IFJ21
char *keywords[] =  { "require", "do", "if", "else", "end", "function",
                      "global", "local", "nil", "return", "then", "while" };

char *variable_type[] = {"string", "integer", "nil", "number", "double"};

char first_operators[] = {'#', '*', '/', '+', '-', '.', '<', '>', '~'};

char second_operators[] = {'/', '=', '.'};


/**
 * @brief Scans the input file - the main function of the scanner
 */
token_t *get_next_token(FILE *file) 
{
    string_t *buffer = init_buffer();
  
    if (buffer == NULL) {
        printf("EROOR\n");
        exit(1);
    }
    int state = 0;
    int c;
    char escape_seq_bufer[5]; // used if escape sequence is in \ddd form
    while ( (c = fgetc(file)) != EOF ) {
        
        switch(state) 
        {
            
            case DEFAULT_STATE:
                // Ignoring all the whitespaces if we are in default state
                if ( isspace(c)) {
                    break;
                }
                if (c == '-') {
                    append_character(buffer, c);
                    state = START_COMMENT_OR_MINUS;
                }
                    // Found number
                else if (isdigit(c) && c != '0') { 
                    append_character(buffer, c);
                    state = NUMBER_SEQUENCE;
                }
                    // found ID
                else if (isalpha(c) || c == '_') {
                    append_character(buffer, c);
                    state = ID_OR_KEYWORD;
                }
                    // Found String literal
                else if (c == '"') {
                    append_character(buffer, c);
                    state = STRING_LITERAL;
                }
                    // Found separator
                else if ( c == ',' ) {
                    append_character(buffer, c);
                    return generate_token(buffer, SEPARATOR);
                }
                    // Found equals '='
                else if ( c == '=' ) {
                    state = ASSIGN_OR_EQUALS;
                    append_character(buffer , c);
                }
                else if ( c == '(' ) {
                    append_character(buffer, c);
                    return generate_token(buffer,  L_PAREN);
                }
                else if ( c == ')' ) {
                    append_character(buffer, c);
                    return generate_token(buffer, R_PAREN);
                }
                else if ( c == ':') {
                    append_character(buffer, c);
                    return generate_token(buffer,  COLON);
                }
                    // Found an operator
                else if ( is_operator(c) ) {
					append_character(buffer, c);
                    state = OPERATOR;
                }
                break;

            case START_COMMENT_OR_MINUS:
                if (c == '-') {
                    state = ON_COMMENT;
                    buffer->string[0] = '\0';
                    buffer->current_index = 0;
                }
                else {
                    return generate_token(buffer,  OPERATOR);
                    ungetc(c, file);
                    state = DEFAULT_STATE;
                }
                break;
        
            case ON_COMMENT:
                if (c == '[' ) {
                    state = CHECK_COMMENT_BLOCK;
                }
                else {
                    state = INSIDE_LINE_COMMENT;
                }
                break;

            case CHECK_COMMENT_BLOCK:
                if (c == '[' ) {
                    state = INSIDE_BLOCK_COMMENT;
                }
                else {
                    state = INSIDE_LINE_COMMENT;
                }
                break;
        
            case INSIDE_LINE_COMMENT:
                if (c == '\n') {
                    state = DEFAULT_STATE;
                }
                break;
            
            case INSIDE_BLOCK_COMMENT:
                if (c == ']' ) {
                    state = CHECK_END_BLOCK_COMMENT;
                }
                break; 
        
            case CHECK_END_BLOCK_COMMENT:
                if (c == ']' ) {
                    state = DEFAULT_STATE;
                }
                else {
                    state = INSIDE_BLOCK_COMMENT;
                }
                break;
            
            case STRING_LITERAL:
                if (c == '"') {
                    append_character(buffer, c);
                    return generate_token(buffer , STRING_LITERAL);
                }
                else if ( c == '\\' ) {
                    state = ESCAPE_SEQUENCE;
                }
                else {
                    append_character(buffer, c);
                }
                break;

            case NUMBER_SEQUENCE:
                if (c == '.') {
                    append_character(buffer, c);
                    state = DOUBLE_DOT_SEQUENCE;
                }        
                else if (c == 'e' || c == 'E') {
                    append_character(buffer, c);
                    state = DOUBLE_E_SEQUENCE;
                }
                else if ( (c >= '0') && (c <= '9' ) ) {
                    append_character(buffer, c);
                }
                
                else {
                    ungetc(c, file);
                    return generate_token(buffer, integer);
                }
                break;
        
            case DOUBLE_DOT_SEQUENCE:
                if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
					state = DOUBLE_DOT_SEQUENCE_VALID;
                }
				else {
					fprintf(stderr, "Error, there has to be a digit after '.' in number sequence\n " );
					exit(-1);
				}
                break;

			case DOUBLE_DOT_SEQUENCE_VALID:
				if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
					state = DOUBLE_DOT_SEQUENCE_VALID;
                }
				else if (c == 'e' || c == 'E') {
					append_character(buffer, c);
					state = DOUBLE_E_SEQUENCE;
                }
				else if (isspace(c)) {
                    return generate_token(buffer,  state);
				 }
				else {
                    // No space between number and other token
                    ungetc(c ,file);
                    return generate_token(buffer, state);
                }

				break;

            case DOUBLE_E_SEQUENCE:
                if (c == '+' || c == '-') {
                    append_character(buffer, c);
                    state = DOUBLE_E_PLUS_MINUS_SEQUENCE;
                }
                else if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
                }
                else {
					// TODO : Exit program
					fprintf(stderr, "After 'e' in number sequence, there has to be digit or '+' or '-' ");
					exit(-1);
				}
                break;

			case DOUBLE_E_SEQUENCE_VALID:
				if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
                }
                else if (isspace(c)) {
                    return generate_token(buffer, state);
                }
                else { 
                    ungetc(c ,file);
                    return generate_token(buffer, state);
                }
				break;

            case DOUBLE_E_PLUS_MINUS_SEQUENCE:
                if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
					state = DOUBLE_E_PLUS_MINUS_SEQUENCE_VALID;
                }
                else {
					// TODO : Exit program
					fprintf(stderr, "After 'e-' 'e+' in number sequence, there has to be digit\n");
					exit(-1);
				}
                break;

			case DOUBLE_E_PLUS_MINUS_SEQUENCE_VALID:
				if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
                }
                else if (isspace(c)) {
                    return generate_token(buffer, state);
                }
                else {
                    // No space between number and other token
                    ungetc(c, file);
                    return generate_token(buffer, state);
                }
                break;

            case ID_OR_KEYWORD:
                if (isalpha(c) || c == '_' || isdigit(c)) {
                    append_character(buffer, c);
                }
                else {
                    ungetc(c, file);
                    return generate_token(buffer,  state);
                }
                break; 

            case OPERATOR:
                if ( buffer->string[buffer->current_index - 1] == '/' && c == '/' )  {
                    append_character(buffer, c);
                    return generate_token(buffer,  state);
                }

				if ( buffer->string[buffer->current_index - 1] == '*' ){
					ungetc(c, file);
					return generate_token(buffer, state);
				}

				if ( buffer->string[buffer->current_index - 1] == '/' ){
					ungetc(c, file);
					return generate_token(buffer, state);
				}

                if ( buffer->string[buffer->current_index - 1] == '.' && c == '.' ) {
                    append_character(buffer, c);
                    return generate_token(buffer,  state);
                }
				else {
					fprintf(stderr, "there is no token that can start with dot and its not '..' \n");
					exit(-1);
				}

                if (buffer->string[buffer->current_index - 1] == '=' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer,  state);
                }
                else if (buffer->string[buffer->current_index - 1] == '<' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer, state);
                }
                else if (buffer->string[buffer->current_index - 1] == '>' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer, state);
                }
                else if (buffer->string[buffer->current_index - 1] == '~' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer, state);
                }
                else if (buffer->string[buffer->current_index] == '~' && c != '=') {
                    printf("Lexical error, ~ has to be followed by =\n");
                    exit(1); // TODO : Think of a way to end the program without using exit              
                    // Lexical analysis error - incorrect operator use (if using ~, = has to follow immediately)
                }
                else if (is_operator(c) || c == '=') {
                    printf("Lexical error, %c cannot be followed by %c\n", buffer->string[buffer->current_index], c);
                    exit(1);
                }
                else {
                    return generate_token(buffer, state);
                }
                break;

            case ESCAPE_SEQUENCE:
                if (c == '\"' || c == '\\' ) {
                    append_character(buffer, c);
                    state = STRING_LITERAL;
                }
                else if ( c == 'n' ) {
                    append_character(buffer, '\n');
                    state = STRING_LITERAL;
                }
                else if ( c == 't' ) {
                    append_character(buffer, '\t' );
                    state = STRING_LITERAL;
                }
                else if(c >= '0' && c <= '9' ) {
                    escape_seq_bufer[0] = c;
                    state = ESCAPE_1;
                }
                else {
                    append_character(buffer, c);
                    state = STRING_LITERAL;
                }
                break;

            
            case ESCAPE_1:
                if (c >= '0' && c <= '9') {
                    escape_seq_bufer[1] = c;
                    state = ESCAPE_2;
                }
                else {
                    fprintf(stderr, "Invalid escape sequence in string literal");
                    break;
                }
            case ESCAPE_2:
                if (c >= '0' && c <= '9') {
                    escape_seq_bufer[2] = c;
                    escape_seq_bufer[3] = '\0';
                    int character = strtold(escape_seq_bufer,NULL);
                    if (character > 255) {
                        fprintf(stderr, "Escape character value cannot be higher than 255\n");
                        exit(1);
                        // TODO : replace exit
                    }
                    if (isprint(character)) {
                        append_character(buffer, character);
                    }

                    state = STRING_LITERAL;
                    break;
                }


            case ASSIGN_OR_EQUALS:
                if ( c == '=' ) {
                    append_character(buffer, c);
                    return generate_token(buffer, state);
                }
                else {
                    ungetc(c, file);
                    return generate_token(buffer, state);
                }

            default:
                break;

        } // switch

    } // while
    if (c == EOF) {
        if (buffer->current_index > 0 ) {
			if (state == DOUBLE_E_SEQUENCE || state == DOUBLE_DOT_SEQUENCE || state == DOUBLE_E_PLUS_MINUS_SEQUENCE){
				fprintf(stderr, "There has to be non-empty digit after '.' or 'e' or 'e+/-'\n");
				exit(-1);
			}
            return generate_token(buffer, state);
        }
        else {
            destroy_buffer(buffer);
            return NULL; // Found no more tokens
        }
    }

    return NULL;
}


token_t *generate_token(string_t *buffer,  int type) 
{
    // If type is Null, we are signalizing, that we couldnt
    // determine type of token during tokenization
    token_t *token = (token_t *) malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr,"Intern malloc problem");
        exit(1);
    }
    
    token->value = calloc(buffer->size, 1);
    
    if (token->value == NULL) {
        fprintf(stderr,"Intern malloc problem");
        exit(1);
    }
    
    buffer->string[buffer->current_index] = '\0';

    memcpy(token->value, buffer->string, buffer->current_index );
    
    switch (type) 
    {
        case ID_OR_KEYWORD:
            if (is_keyword(buffer->string) || is_variable_type(buffer->string)) {
                token->TYPE = keyword;
                break;
            }
            else {
                token->TYPE = identifier;
				break;
            }
        case OPERATOR:
            token->TYPE = operator;
            break;
        
        case STRING_LITERAL:
            token->TYPE = str_literal;
            break;

        case ASSIGN_OR_EQUALS:
            if (!strcmp(buffer->string, "==")) {
                token->TYPE = equals;  
            }
            else {
                token->TYPE = assign;
            }
            break;

        case COLON:
            token->TYPE = colon;
            break;
        case SEPARATOR:
            token->TYPE = separator;
            break;

        case NUMBER_SEQUENCE:
            token->TYPE = integer;
            break;
        case L_PAREN:
            token->TYPE = l_paren;
            break;
        
        case R_PAREN:
            token->TYPE = r_paren;
            break;

		case DOUBLE_DOT_SEQUENCE_VALID:
		case DOUBLE_E_PLUS_MINUS_SEQUENCE_VALID:
		case DOUBLE_E_SEQUENCE_VALID:
			token->TYPE = t_double;
			break;

        case DOUBLE_DOT_SEQUENCE:
            token->TYPE = t_double;
            break;
            
        case DOUBLE_E_SEQUENCE:
            token->TYPE = t_double;
            break;
    } // switch
    
    destroy_buffer(buffer);
    return token;
}


/**
 * @brief Determines wether a given character is operator
 */ 
bool is_operator(int c) {
    for (int i = 0; i < NUM_OF_OPERATORS; i++) {
        if (c == first_operators[i]) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Determines whether a given string is a keyword
 */ 
bool is_keyword(char *string) {
    for (int i = 0; i < NUM_OF_KEYWORDS; i++) {
        if (!strcmp(string, keywords[i])) {
            return true;
        }
    }
    return false;
}


/** 
 * @brief Determines whether a given string is a variable type
 * aka "integer" or "double" etc
 */
bool is_variable_type(char *string) {
    for (int i = 0; i < NUM_OF_VAR_TYPE; i++) {
        if (!strcmp(string, variable_type[i])) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Converts given integer type of a token to its string version
 */
const char *token_type_to_str(int type)
{
    switch (type)
    {
        case colon:
            return "colon";
        case equals: 
            return "equals";
        case assign: 
            return "assign";
        case l_paren: 
            return "l_paren";
        case r_paren: 
            return "r_paren";
        case integer: 
            return "integer";
        case keyword: 
            return "keyword";
        case t_double: 
            return "t_double";
        case operator: 
            return "operator";
        case separator: 
            return "separator";
        case identifier: 
            return "identifier";
        case str_literal: 
            return "str_literal";
  }
  return "unrecognized token type";
}

// TODO: add line number?
/**
 * @brief Auxiliary function that prints string representation of current token
 */
void print_token(token_t *token)
{
    // TODO line/token number!!
    printf("Token: [%s: '%s']\n", token_type_to_str(token->TYPE), token->value);
}
