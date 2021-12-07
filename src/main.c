#include <stdio.h>
#include "include/parser.h"
#include "include/error.h"
#include <unistd.h> // sleep()

#define PRINT_ERROR \
    error_message("Scanner", ERR_LEX, " - unknown token '%s'", token->attribute->string);

int main(int argc, char *argv[])
{
    /* This code demonstrates the scanners work */
#if 0
    token_t *token;
    while ((token=get_next_token(stdin)) != NULL) {
        if (token->type == TOKEN_EOF) {
            break;
        } else if (token->type == TOKEN_ERROR) {
            PRINT_ERROR; 
            return ERR_LEX;
        }
        // Continue scanning
        printf("Calling scanner...\n");
        sleep(1);
        print_token(token);
    }
    sleep(2);
    printf("\nSource file read successfully\n");
#endif
    return parser_parse(stdin);
}
