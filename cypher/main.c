#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

#define LINESIZE 256

int main(int argc, char const *argv[]) {
    int pipe1[2], pipe2[2];
    pid_t pid;
    char line[LINESIZE];
    int bytes_read;

    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        fprintf(stderr, "Error creating pipes!\n");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        fprintf(stderr, "Error creating child process!\n");
        exit(EXIT_FAILURE);

    } else if (pid > 0) {
        /* parent */
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
        /* child */
        close(pipe1[WRITE_END]);
        close(pipe2[READ_END]);

        do {
            bytes_read = read(pipe1[READ_END], line, LINESIZE);

            write(pipe2[WRITE_END], line, bytes_read);
        } while (bytes_read > 0);

        close(pipe1[READ_END]);
        close(pipe2[WRITE_END]);

        /* exit gracefully */
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
