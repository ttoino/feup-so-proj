#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

#define LINESIZE 256

void error() { exit(EXIT_FAILURE); }

int main(int argc, char const *argv[]) {
    int pipe1[2], pipe2[2];
    pid_t pid;
    char line[LINESIZE];
    size_t bytes_read;

    signal(SIGUSR1, error);

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        fprintf(stderr, "Error creating pipes!\n");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Error creating child process!\n");
        exit(EXIT_FAILURE);

    } else if (pid > 0) {
        // Parent process
        close(pipe1[READ_END]);
        close(pipe2[WRITE_END]);

        do {
            bytes_read = read(STDIN_FILENO, line, LINESIZE);

            write(pipe1[WRITE_END], line, bytes_read);
        } while (bytes_read > 0);

        close(pipe1[WRITE_END]);

        do {
            bytes_read = read(pipe2[READ_END], line, LINESIZE);

            write(STDOUT_FILENO, line, bytes_read);
        } while (bytes_read > 0);

        close(pipe2[READ_END]);

        /* wait for child and exit */
        if (waitpid(pid, NULL, 0) < 0)
            fprintf(stderr, "Cannot wait for child: %s\n", strerror(errno));

        exit(EXIT_SUCCESS);

    } else {
        // Child process
        close(pipe1[WRITE_END]);
        close(pipe2[READ_END]);

        // Load cypher
        FILE *cypherfile = fopen("cypher.txt", "r");

        if (cypherfile == NULL) {
            fprintf(stderr, "Couldn't open cypher.txt!\n");
            kill(getppid(), SIGUSR1);
            exit(EXIT_FAILURE);
        }

        size_t ncyphers = 0, buffer_size = 0;
        char **keys = calloc(ncyphers, sizeof(char *));
        char **values = calloc(ncyphers, sizeof(char *));
        char *buffer = NULL;

        while (getline(&buffer, &buffer_size, cypherfile) > 0) {
            ++ncyphers;
            keys = reallocarray(keys, ncyphers, sizeof(char *));
            values = reallocarray(values, ncyphers, sizeof(char *));

            keys[ncyphers - 1] = buffer;

            while (*buffer != ' ') {
                ++buffer;

                if (*buffer == '\0') {
                    fprintf(stderr, "Malformed cypher.txt!\n");
                    kill(getppid(), SIGUSR1);
                    exit(EXIT_FAILURE);
                }
            }

            *buffer++ = '\0';
            values[ncyphers - 1] = buffer;

            while (*buffer != ' ' && *buffer != '\n') {
                ++buffer;

                if (*buffer == '\0') {
                    fprintf(stderr, "Malformed cypher.txt!\n");
                    kill(getppid(), SIGUSR1);
                    exit(EXIT_FAILURE);
                }
            }

            *buffer = '\0';

            buffer = NULL;
        }

        size_t size = 0;
        char *str = calloc(size, sizeof(char));

        do {
            bytes_read = read(pipe1[READ_END], line, LINESIZE);
            size += bytes_read;

            str = reallocarray(str, size, sizeof(char));
            memcpy(str + size - bytes_read, line, bytes_read);
        } while (bytes_read > 0);

        close(pipe1[READ_END]);

        for (size_t i = 0; i < size; ++i) {
        nextchar:
            for (size_t j = 0; j < ncyphers; ++j) {
                size_t n = strlen(keys[j]);
                size_t m = strlen(keys[j]);

                if (i + n > size)
                    continue;

                if (strncmp(str + i, keys[j], n) == 0) {
                    write(pipe2[WRITE_END], values[j], m);
                    i += n;
                    goto nextchar;
                }

                if (i + m > size)
                    continue;

                if (strncmp(str + i, values[j], m) == 0) {
                    write(pipe2[WRITE_END], keys[j], n);
                    i += m;
                    goto nextchar;
                }
            }

            write(pipe2[WRITE_END], str + i, 1);
        }

        for (size_t j = 0; j < ncyphers; ++j) {
            free(keys[j]);
        }

        free(str);
        free(keys);
        free(values);
        close(pipe2[WRITE_END]);

        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
