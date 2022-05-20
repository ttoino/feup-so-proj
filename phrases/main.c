#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CMD_USAGE "Usage: %s [-l] file\n"

void print_phrases(const char *file_path, bool list) {
    FILE *fp = fopen(file_path, "r");

    if (fp == NULL) {
        fprintf(stderr, "Can't open file!\n");
        exit(EXIT_FAILURE);
    }

    int phrase_count = 0, phrase_length = 0;
    char *phrase = NULL;
    char current_char;
    bool punct = false;

    while ((current_char = fgetc(fp)) != EOF) {
        if (ferror(fp)) {
            fprintf(stderr, "Error while reading file!\n");
            exit(EXIT_FAILURE);
        }

        // We're on a new phrase, print last one
        if (punct && !strchr(".!?", current_char)) {
            // ignore leading whitespace
            int offset = 0;
            while (*phrase == ' ') {
                phrase++;
                offset++;
            }

            ++phrase_count;

            if (list)
                printf("[%d] %.*s\n", phrase_count, phrase_length - offset,
                       phrase);

            free(phrase - offset);
            phrase = NULL;
            phrase_length = 0;
        }

        phrase = reallocarray(phrase, ++phrase_length, sizeof(char));

        if (phrase == NULL) {
            fprintf(stderr, "Error while trying to allocate more memory!\n");
            exit(EXIT_FAILURE);
        }

        if (current_char == '\n')
            current_char = ' ';

        phrase[phrase_length - 1] = current_char;

        punct = strchr(".!?", current_char);
    }

    // the file might be empty or end with a sentence terminator
    if (phrase != NULL) {
        // Ignore trailing whitespace
        while (phrase[phrase_length - 1] == ' ')
            --phrase_length;

        // ignore leading whitespace
        int offset = 0;
        while (*phrase == ' ') {
            phrase++;
            offset++;
        }

        if (*phrase) {
            ++phrase_count;

            if (list)
                printf("[%d] %.*s\n", phrase_count, phrase_length - offset,
                       phrase);
        }
        free(phrase - offset);
    }

    if (!list)
        printf("%d\n", phrase_count);

    fclose(fp);
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, CMD_USAGE, argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    bool list = false;

    int opt;

    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch(opt) {
            case 'l':
                list = true;
                break;
            default:
                fprintf(stderr, CMD_USAGE, argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    for (int i = optind; i < argc; i++)
        filename = argv[i];

    print_phrases(filename, list);

    return EXIT_SUCCESS;
}
