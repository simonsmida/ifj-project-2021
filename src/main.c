#include <stdio.h>
// #include "include/scanner.h"
#include "include/parser.h"

int main(int argc, char *argv[])
{
    // if (argc < 2) {
    //     fprintf(stderr, "Error: Not enough arguments, please specify an input file.\n");
    //     return -1;
    // }

    FILE *f = stdin;
    // if (f == NULL) {
    //     fprintf(stderr, "Error: file failed to be opened.\n");
    //     return -1;
    // }

    
    // char c;
    // c = getchar();
    // //char d = c;
    // ungetc(c,f);
    // while ((c) != 'q' && (!feof(f))) {
    //     printf("Calling scanner...\n");
    //     token_t *token = get_next_token(f);
    //     if (token->type == TOKEN_EOF) break;
    //     if (token->type == TOKEN_ERROR) {
    //         fprintf(stderr, "Error while scanning\n");
    //         return -1;
    //     }
    //     print_token(token);
    //     c = getchar();
    // }
    // printf("Source file read successfully\n");
    
    return parser_parse(f);
}
