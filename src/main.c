#include <stdio.h>
#include "include/scanner.h"


int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Error: Not enough arguments, please specify an input file.\n");
        return -1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        fprintf(stderr, "Error: file failed to be opened.\n");
        return -1;
    }

    char c;
    while ((c=getchar()) != 'q' && (!feof(f))) {
        printf("Calling scanner...\n");
        token_t *token = get_next_token(f);
        if (token == NULL) {
            fprintf(stderr, "Error: token could not be loaded.\n");
            return -1;
        }
        print_token(token);
    }
    printf("Source file read successfully");
}
