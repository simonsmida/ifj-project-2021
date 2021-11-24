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

char *variable_type[] = {"string", "integer", "nil", "number"};

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
	int error = 0;
    int state = DEFAULT_STATE;
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
                else if (isdigit(c)) { 
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
                    return generate_token(buffer, SEPARATOR, error);
                }
                    // Found equals '='
                else if ( c == '=' ) {
                    state = ASSIGN_OR_EQUALS;
                    append_character(buffer , c);
                }
                else if ( c == '(' ) {
                    append_character(buffer, c);
                    return generate_token(buffer,  L_PAREN, error);
                }
                else if ( c == ')' ) {
                    append_character(buffer, c);
                    return generate_token(buffer, R_PAREN, error);
                }
                else if ( c == ':') {
                    append_character(buffer, c);
                    return generate_token(buffer,  COLON, error);
                }
                    // Found an operator
                else if ( is_operator(c) ) {
					append_character(buffer, c);
                    state = OPERATOR;
                }
				else {
					// Found an illegal character for example ';'
					return generate_token(buffer, STATE_ERROR, error);
				}
                break;

            case START_COMMENT_OR_MINUS:
                if (c == '-') {
                    state = ON_COMMENT;
                    buffer->string[0] = '\0';
                    buffer->current_index = 0;
                }
                else {
					ungetc(c, file);
                    return generate_token(buffer,  OPERATOR, error);
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
                    return generate_token(buffer , state, error);
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
                    return generate_token(buffer, state , error);
                }
                break;
        
            case DOUBLE_DOT_SEQUENCE:
                if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
					state = DOUBLE_DOT_SEQUENCE_VALID;
                }
				else {
					fprintf(stderr, "Error, there has to be a digit after '.' in number sequence\n " );
					error = -1;
					return generate_token(buffer, state, error);
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
                    return generate_token(buffer,  state, error);
				 }
				else {
                    // No space between number and other token
                    ungetc(c ,file);
                    return generate_token(buffer, state, error);
                }
				break;

            case DOUBLE_E_SEQUENCE:
                if (c == '+' || c == '-') {
                    append_character(buffer, c);
                    state = DOUBLE_E_PLUS_MINUS_SEQUENCE;
                }
                else if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
					state = DOUBLE_E_SEQUENCE_VALID;
                }
                else {
					
					fprintf(stderr, "After 'e' in number sequence, there has to be digit or '+' or '-' ");
					error = -1;
					return generate_token(buffer, state, error);
				}
                break;

			case DOUBLE_E_SEQUENCE_VALID:
				if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
                }
                else if (isspace(c)) {
                    return generate_token(buffer, state, error);
                }
                else { 
                    ungetc(c ,file);
                    return generate_token(buffer, state, error);
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
					error = -1;
					return generate_token(buffer, state, error);
				}
                break;

			case DOUBLE_E_PLUS_MINUS_SEQUENCE_VALID:
				if (c >= '0' && c <= '9' ) {
                    append_character(buffer, c);
                }
                else if (isspace(c)) {
                    return generate_token(buffer, state, error);
                }
                else {
                    // No space between number and other token
                    ungetc(c, file);
                    return generate_token(buffer, state, error);
                }
                break;

            case ID_OR_KEYWORD:
                if (isalpha(c) || c == '_' || isdigit(c)) {
                    append_character(buffer, c);
                }
                else {
                    ungetc(c, file);
                    return generate_token(buffer,  state, error);
                }
                break; 

            case OPERATOR:
                if ( buffer->string[buffer->current_index - 1] == '/' && c == '/' )  {
                    append_character(buffer, c);
                    return generate_token(buffer,  state, error);
                }

				if ( buffer->string[buffer->current_index - 1] == '*' ){
					ungetc(c, file);
					return generate_token(buffer, state, error);
				}

				if ( buffer->string[buffer->current_index - 1] == '/' ){
					ungetc(c, file);
					return generate_token(buffer, state, error);
				}

				if ( buffer->string[buffer->current_index - 1] == '-' ){
					ungetc(c, file);
					return generate_token(buffer, state, error);
				}

				if ( buffer->string[buffer->current_index - 1] == '+' ){
					ungetc(c, file);
					return generate_token(buffer, state, error);
				}

                if ( buffer->string[buffer->current_index - 1] == '.' && c == '.' ) {
                    append_character(buffer, c);
                    return generate_token(buffer,  state, error);
                }
				else if ((buffer->string[buffer->current_index] - 1) == '.' && c != '.') {
					fprintf(stderr, "there is no token that can start with dot and its not '..' \n");
					error = -1;
					return generate_token(buffer, state, error);
				}

                if (buffer->string[buffer->current_index - 1] == '=' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer,  state, error);
                }
                else if (buffer->string[buffer->current_index - 1] == '<' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer, state, error);
                }
                else if (buffer->string[buffer->current_index - 1] == '>' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer, state, error);
                }
                else if (buffer->string[buffer->current_index - 1] == '~' && c == '=') {
                    append_character(buffer, c);
                    return generate_token(buffer, state, error);
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
                    return generate_token(buffer, state, error);
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
					error = -1;
					return generate_token(buffer, state, error);
                   
                }
				break;
            case ESCAPE_2:
                if (c >= '0' && c <= '9') {
                    escape_seq_bufer[2] = c;
                    escape_seq_bufer[3] = '\0';
                    int character = strtold(escape_seq_bufer,NULL);
                    if (character > 255) {
                        fprintf(stderr, "Escape character value cannot be higher than 255\n");
                        error = -1;
						return generate_token(buffer, state, error);
                    }
                    if (isprint(character)) {
                        append_character(buffer, character);
                    }

                    state = STRING_LITERAL;
                }
				break;


            case ASSIGN_OR_EQUALS:
                if ( c == '=' ) {
                    append_character(buffer, c);
                    return generate_token(buffer, state, error);
                }
                else {
                    ungetc(c, file);
                    return generate_token(buffer, state, error);
                }

            default:
                break;

        } // switch

    } // while
    if (c == EOF) {
        if (buffer->current_index > 0 ) {
			ungetc(c, file);
			if (state == DOUBLE_E_SEQUENCE || state == DOUBLE_DOT_SEQUENCE || state == DOUBLE_E_PLUS_MINUS_SEQUENCE){
				fprintf(stderr, "There has to be non-empty digit after '.' or 'e' or 'e+/-'\n");
				error = -1;
				return generate_token(buffer, state, error);
			}
            return generate_token(buffer, state, error);
        }
        else {
            return generate_token(buffer, STATE_EOF, error);
        }
    }

    return NULL;
}

/**
 * @brief Function for generating token, used by lexer
 * 
 * @param buffer Buffer with string value of current token
 * @param type State of FSM while calling this function
 * 
 * @return Pointer to a token if successful, otherwise NULL
 */
token_t *generate_token(string_t *buffer,  int type, int error) 
{
	

    token_t *token = (token_t *) malloc(sizeof(token_t));
    if (token == NULL) {
        fprintf(stderr,"Intern malloc problem");
        return NULL;
    }
    	

    token->attribute = calloc(1, sizeof(token_attribute_t));

	if (token->attribute == NULL){
		fprintf(stderr,"Intern malloc problem");
        return NULL;
	}
	// Initializing attributes
	token->attribute->integer = 0;
	token->attribute->number = 0.0f;
	token->attribute->string = NULL;
    buffer->string[buffer->current_index] = '\0';
	
    switch (type) 
    {
        case ID_OR_KEYWORD:
			token->attribute->string = malloc(strlen(buffer->string));
			if (token->attribute->string == NULL){
				fprintf(stderr, "Intern malloc problem\n");
				return NULL;
			}
			memcpy(token->attribute->string,buffer->string, strlen(buffer->string));

            if (is_keyword(buffer->string) || is_variable_type(buffer->string)) {
                token->type = TOKEN_KEYWORD;
				token->attribute->keyword_type = determine_keyword(buffer->string);
                break;
            }
            else {
                token->type = TOKEN_ID;
				break;
            }
			break;
        case OPERATOR:
			 // + - *
			switch (buffer->string[buffer->current_index - 1]){
				case '-':
					token->type = TOKEN_MINUS;
					break;
				case '+':
					token->type = TOKEN_PLUS;
			 		break;
				case '*':
					token->type = TOKEN_MUL;
					break;
				case '#':
					token->type = TOKEN_STRLEN;
					break;
			 } // switch

			if (!strcmp((buffer->string), "~=")){
				token->type = TOKEN_NOT_EQ;
				break;
			}
			if (!strcmp((buffer->string), "<" )){
				token->type = TOKEN_LT;
				break;
			}
			if (!strcmp((buffer->string), "<=" )){
				token->type = TOKEN_LE;
				break;
			}
			if (!strcmp((buffer->string), ">=" )){
				token->type = TOKEN_GE;
				break;
			}
			if (!strcmp((buffer->string), ">" )){
				token->type = TOKEN_GT;
				break;
			}
			if (!strcmp((buffer->string), ".." )){
				token->type = TOKEN_CONCAT;
				break;
			}
			if (!strcmp((buffer->string), "/" )){
				token->type = TOKEN_DIV;
				break;
			}
			if (!strcmp((buffer->string), "//" )){
				token->type = TOKEN_INT_DIV;
				break;
			}
            break;
        
        case STRING_LITERAL:
			token->attribute->string = malloc(strlen(buffer->string));
			if (token->attribute->string == NULL){
				fprintf(stderr,"Intern malloc problem");
        		return NULL;
			}
			memcpy(token->attribute->string,buffer->string,  strlen(buffer->string));
            token->type = TOKEN_STR_LIT;
            break;

        case ASSIGN_OR_EQUALS:
            if (!strcmp(buffer->string, "==")) {
                token->type = TOKEN_EQ;  
            }
            else {
                token->type = TOKEN_ASSIGN;
            }
            break;

        case COLON:
            token->type = TOKEN_COLON;
            break;
        case SEPARATOR:
            token->type = TOKEN_COMMA;
            break;

       
        case L_PAREN:
            token->type = TOKEN_L_PAR;
            break;
        
        case R_PAREN:
            token->type = TOKEN_R_PAR;
            break;

		case NUMBER_SEQUENCE:
			token->type = TOKEN_INT_LIT;
			int num = (int) strtol(buffer->string, NULL, 10);
			token->attribute->integer = num;
		
            break;

		case DOUBLE_DOT_SEQUENCE_VALID:
		case DOUBLE_E_PLUS_MINUS_SEQUENCE_VALID:
		case DOUBLE_E_SEQUENCE_VALID:
			token->type = TOKEN_NUM_LIT;
			double number = strtod(buffer->string, NULL);
			token->attribute->number = number;
			
			break;

		case STATE_EOF:
			token->type = TOKEN_EOF;
			break;

		case START_COMMENT_OR_MINUS:
			if (!strcmp(buffer->string, "-")){
				token->type = TOKEN_MINUS;
			}
			break;

		case STATE_ERROR:
			token->type =  TOKEN_ERROR;
			break;
		
    } // switch
    
    destroy_buffer(buffer);
    return token;
}


/**
 * @brief Determines wether a given character is operator
 * 
 * @param c Character to be determined whether is operator
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
 * 
 * @param string String from which to be determined whether it is a keyword
 * 
 * @return True if string is keyword, otherwise false
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
 * 
 * @param string String from which to be determined whether it is a variable type
 * 
 * @return True if string is var type, otherwise false
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
        case TOKEN_ERROR:
            return "TOKEN_ERROR";
        case TOKEN_PLUS:
            return "plus";
        case TOKEN_MINUS:
            return "minus";
        case TOKEN_MUL:
            return "mul";
        case TOKEN_DIV:
            return "div";
        case TOKEN_INT_DIV:
            return "int div";
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_COLON:
            return "colon";
        case TOKEN_ASSIGN: 
            return "assign";
        case TOKEN_L_PAR: 
            return "l_paren";
        case TOKEN_R_PAR: 
            return "r_paren";
        case TOKEN_KEYWORD: 
            return "keyword";
        case TOKEN_COMMA: 
            return "separator";
        case TOKEN_ID: 
            return "identifier";
        case TOKEN_NUM_LIT: 
            return "number_literal";
        case TOKEN_INT_LIT: 
            return "int_literal";
        case TOKEN_STR_LIT: 
            return "str_literal";
        case TOKEN_EQ: 
        case TOKEN_NOT_EQ:
        case TOKEN_CONCAT:
        case TOKEN_STRLEN:
        case TOKEN_GT:
        case TOKEN_GE:
        case TOKEN_LT:
        case TOKEN_LE: 
            return "operator";
  }
  return "unrecognized token type";
}

// TODO: add line number?
/**
 * @brief Auxiliary function that prints string representation of current token
 */
void print_token(token_t *token)
{
    // TODO change token->attribute->string to its actual representation
	if (token->attribute->keyword_type == KEYWORD_STRING){
		printf("Next token is STRING\n");
	}
	if (token->attribute->keyword_type == KEYWORD_INTEGER){
		printf("Next token is INTEGER\n");
	}
	if (token->attribute->keyword_type == KEYWORD_NIL){
		printf("Next token is NIL\n");
	}
	
    printf("Token: [%s '%s']\n", token_type_to_str(token->type), token->attribute->string);
}


keyword_type_t determine_keyword(const char *string){
	if(!strcmp(string,"require")){
		return KEYWORD_REQUIRE;
	}
	else if(!strcmp(string,"nil")){
		return KEYWORD_NIL;
	}
	else if(!strcmp(string,"if")){
		return KEYWORD_IF;
	}
	else if(!strcmp(string,"else")){
		return KEYWORD_ELSE;
	}
	else if(!strcmp(string,"do")){
		return KEYWORD_DO;
	}
	else if(!strcmp(string,"end")){
		return KEYWORD_END;
	}
	else if(!strcmp(string,"function")){
		return KEYWORD_FUNCTION;
	}
	else if(!strcmp(string,"global")){
		return KEYWORD_GLOBAL;
	}
	else if(!strcmp(string,"local")){
		return KEYWORD_LOCAL;
	}
	else if(!strcmp(string,"return")){
		return KEYWORD_RETURN;
	}
	else if(!strcmp(string,"then")){
		return KEYWORD_THEN;
	}
	else if(!strcmp(string,"while")){
		return KEYWORD_WHILE;
	}
	else if(!strcmp(string,"string")){
		return KEYWORD_STRING;
	}
	else if(!strcmp(string,"integer")){
		return KEYWORD_INTEGER;
	}
	else if(!strcmp(string,"number")){
		return KEYWORD_NUMBER;
	}

}