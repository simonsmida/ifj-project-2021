#include <stdio.h>
#include "include/parser.h"
#include "include/error.h"

int main(int argc, char *argv[])
{
    FILE *f = stdin;

    /*
    char c;
    while ((c=getchar()) != 'q' && (!feof(f))) {
        printf("Calling scanner...\n");
        token_t *token = get_next_token(f);
        if (token->type == TOKEN_EOF) break;
        if (token->type == TOKEN_ERROR) {
            fprintf(stderr, "Error while scanning\n");
            return -1;
        }
        print_token(token);
    }
    printf("Source file read successfully");
    */

    return parser_parse(f);
}
