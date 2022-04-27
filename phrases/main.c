#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_phrases(const char *file_path, bool list) {
    FILE *fp = fopen(file_path, "r");

    if (fp == NULL) {
        fprintf(stderr, "Can't open file!\n");
        exit(EXIT_FAILURE);
    }

    int phrase_count = 0, phrase_length = 0;
    char *phrase = NULL;
    char current_char;

    while ((current_char = fgetc(fp)) != EOF) {
        if (ferror(fp)) {
            fprintf(stderr, "Error while reading file!\n");
            exit(EXIT_FAILURE);
        }

        if (strchr(".!?", current_char)) {
            if (phrase == NULL)
                continue; // discard multiple delimiters in sequence

            phrase[phrase_length] = '\0';

            int offset = 0;
            while (*phrase == ' ') {
                phrase++;
                offset++;
            } // ignore leading whitespace

            ++phrase_count;

            if (list)
                printf("[%d] %s%c\n", phrase_count, phrase, current_char);

            free(phrase - offset);
            phrase = NULL;
            phrase_length = 0;

        } else {
            phrase =
                realloc(phrase, sizeof(char) * ++phrase_length +
                                    1); // + 1 to account for the eventual \0

            if (phrase == NULL) {
                fprintf(stderr,
                        "Error while trying to allocate more memory!\n");
                exit(EXIT_FAILURE);
            }

            if (current_char == '\n')
                current_char = ' ';

            phrase[phrase_length - 1] = current_char;
        }
    }

    // the file might be empty or end with a sentence terminator
    if (phrase == NULL) {
        if (!list)
            printf("%d\n", phrase_count);
        return;
    }

    // we need to do this once more because the last sentence would not be
    // printed
    phrase[phrase_length] = '\0';

    // Ignore trailing whitespace
    while (phrase[phrase_length - 1] == ' ')
        phrase[--phrase_length] = '\0';

    // ignore leading whitespace
    int offset = 0;
    while (*phrase == ' ') {
        phrase++;
        offset++;
    }

    ++phrase_count;

    if (list)
        printf("[%d] %s\n", phrase_count, phrase);
    else
        printf("%d\n", phrase_count);

    free(phrase - offset);
    fclose(fp);
}

int main(int argc, char const *argv[]) {
    if (argc != 2 && argc != 3) {
        fprintf(stderr, "Usage: %s [-l] file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];
    bool list = false;

    if ((list = argc == 3)) {
        if (strcmp(argv[1], "-l") != 0) {
            fprintf(stderr, "Usage: %s [-l] file\n", argv[0]);
            exit(EXIT_FAILURE);
        }

        filename = argv[2];
    }

    print_phrases(filename, list);

    return EXIT_SUCCESS;
}
