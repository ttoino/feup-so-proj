#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void print_usage() {
    puts("usage: phrases [-l] file");
}

void print_phrases(const char* file_path, bool list) {

    FILE* fp = fopen(file_path, "r");

    if (fp == NULL) {
        printf("error while opening file '%s'\n", file_path);
        exit(EXIT_FAILURE);
    }

    int phrase_count = 0, phrase_length = 0;
    char* phrase = NULL;
    char current_char[2]; // needs to have 2 chars because of \0

    while (fread(current_char, sizeof(char), 1, fp) > 0) {

        current_char[1] = '\0';

        if (ferror(fp)) {
            printf("error while reading file '%s'\n", file_path);
            exit(EXIT_FAILURE);
        }

        if (strstr(current_char, "\n"))
            continue;
        else if (strstr(current_char, ".") || strstr(current_char, "!") || strstr(current_char, "?") || feof(fp)) { // we might reach the end of the file and not have any punctuation

            phrase[phrase_length] = '\0';

            int offset = 0;
            while (*phrase == ' ') { phrase++; offset++; } // ignore leading whitespace

            if (list) 
                printf("[%d] %s%c\n", ++phrase_count, phrase, current_char[0]);
            else
                printf("%s%c\n", phrase, current_char[0]);
            
            free(phrase - offset);
            phrase = NULL;
            phrase_length = 0;
        } else {

            phrase = realloc(phrase, sizeof(char) * ++phrase_length);
            phrase[phrase_length - 1] = current_char[0];
        }
    }

    // we need to do this once more because the last sentence would not be printed

    phrase[phrase_length] = '\0';

    int offset = 0;
    while (*phrase == ' ') { phrase++; offset++; } // ignore leading whitespace

    if (list) 
        printf("[%d] %s%c\n", ++phrase_count, phrase, current_char[0]);
    else
        printf("%s%c\n", phrase, current_char[0]);
    
    free(phrase - offset);
}

int main(int argc, char const *argv[]) {

    switch (argc) {
        case 1: { // print usage
            print_usage();
            exit(EXIT_SUCCESS);
        }
        case 2: {
            if (argv[1][0] == '-') {
                puts("must supply a file name");  
                print_usage();              
                exit(EXIT_FAILURE);
            } else { // should be a file path, process it
                print_phrases(argv[1], false);
            }
            break;
        }
        case 3: {

            if (argv[1][0] == '-') { // first arg must be the option
                if (strcmp(argv[1], "-l")) { // option must only be '-l'
                    printf("unknown option: %s\n", argv[1]);
                    print_usage();
                    exit(EXIT_FAILURE);
                }

                /* option and file */
                print_phrases(argv[2], true);

            } else {
                printf("invalid argument: %s\n", argv[1]);     
                print_usage();           
                exit(EXIT_FAILURE);
            }

            break;
        }
        default: {
            puts("invalid number of arguments");
            print_usage();
            exit(EXIT_FAILURE);
        }
    };

    return 0;
}
