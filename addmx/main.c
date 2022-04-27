#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

void getMatrixSize(unsigned long *n, unsigned long *m, FILE *f,
                   char *matrix_name) {
    size_t str_size = 0;
    char *str = NULL, *s;
    errno = 0;
    getline(&str, &str_size, f);

    *n = strtoul(str, &s, 10);
    if (errno != 0 || *s++ != 'x') {
        fprintf(stderr, "%s matrix has malformed size!\n", matrix_name);
        exit(EXIT_FAILURE);
    }

    *m = strtoul(s, &s, 10);
    if (errno != 0 || *s++ != '\n') {
        fprintf(stderr, "%s matrix has malformed size!\n", matrix_name);
        exit(EXIT_FAILURE);
    }

    free(str);
}

void readMatrix(unsigned long *matrix, unsigned long n, unsigned long m,
                FILE *f, char *matrix_name) {
    size_t str_size = 0;
    char *str = NULL, *s;
    errno = 0;
    unsigned long i = 0, j = 0;

    while (getline(&str, &str_size, f) > 0) {
        if (j >= n) {
            fprintf(stderr, "%s matrix is malformed!\n", matrix_name);
            exit(EXIT_FAILURE);
        }

        s = str;

        for (i = 0; i < m; ++i) {
            matrix[i * n + j] = strtoul(s, &s, 10);

            if (errno != 0 || (*s != ' ' && *s++ != '\n')) {
                fprintf(stderr, "%s matrix is malformed!\n", matrix_name);
                exit(EXIT_FAILURE);
            }
        }

        if (*s != '\0') {
            fprintf(stderr, "%s matrix is malformed!\n", matrix_name);
            exit(EXIT_FAILURE);
        }

        ++j;
    }

    free(str);
}

int main(int argc, char *argv[]) {
    // Check number of arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s file1 file2\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Open matrices
    FILE *f1, *f2;
    if ((f1 = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Can't open the first matrix!\n");
        exit(EXIT_FAILURE);
    }
    if ((f2 = fopen(argv[2], "r")) == NULL) {
        fprintf(stderr, "Can't open the second matrix!\n");
        exit(EXIT_FAILURE);
    }

    // Get matrices' sizes
    unsigned long n, m, n2, m2;
    getMatrixSize(&n, &m, f1, "First");
    getMatrixSize(&n2, &m2, f2, "Second");

    if (n != n2 || m != m2) {
        fprintf(stderr, "Matrix sizes do not match!\n");
        exit(EXIT_FAILURE);
    }

    // Allocate memory for matrices
    unsigned long *matrix1 =
        mmap(NULL, n * m * sizeof(unsigned long), PROT_READ | PROT_WRITE,
             MAP_SHARED | MAP_ANONYMOUS, 0, 0);
    if (matrix1 == MAP_FAILED) {
        fprintf(stderr, "Error allocating shared memory!\n");
        exit(EXIT_FAILURE);
    }

    unsigned long *matrix2 = calloc(n * m, sizeof(unsigned long));
    if (matrix2 == NULL) {
        fprintf(stderr, "Error allocating memory!\n");
        exit(EXIT_FAILURE);
    }

    readMatrix(matrix1, n, m, f1, "First");
    readMatrix(matrix2, n, m, f2, "Second");

    fclose(f1);
    fclose(f2);

    // Start processes
    unsigned long i, j;
    for (i = 0; i < m; ++i) {
        pid_t pid;
        if ((pid = fork()) < 0) {
            fprintf(stderr, "Error creating child process!\n");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            for (j = 0; j < n; ++j)
                matrix1[i * n + j] += matrix2[i * n + j];
            exit(EXIT_SUCCESS);
        }
    }

    // Wait for processes
    for (i = 0; i < m; ++i) {
        if (waitpid(-1, NULL, 0) < 0) {
            fprintf(stderr, "Error waiting for child process!\n");
            exit(EXIT_FAILURE);
        }
    }

    // Print result
    printf("%lux%lu\n", n, m);

    for (j = 0; j < n; ++j) {
        for (i = 0; i < m; ++i) {
            if (i != 0)
                putchar(' ');

            printf("%lu", matrix1[i * n + j]);
        }
        putchar('\n');
    }

    // Free memory
    free(matrix2);
    if (munmap(matrix1, sizeof(matrix1)) < 0) {
        fprintf(stderr, "Error deallocating shared memory!\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
