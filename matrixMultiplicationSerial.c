#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1200  // Matrix size (N x N)

// Matrices declared globally to avoid large stack allocation
double A[N][N];
double B[N][N];
double C[N][N];

void fill_random(double mat[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = (double)rand() / RAND_MAX * 10.0;
}

void matrix_multiply_serial(void) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < N; k++)
                C[i][j] += A[i][k] * B[k][j];
        }
}

int main(void) {
    srand(42);  // Fixed seed for reproducibility

    fill_random(A);
    fill_random(B);

    printf("=== Serial Matrix Multiplication ===\n");
    printf("Matrix size: %d x %d\n\n", N, N);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    matrix_multiply_serial();

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec)
                   + (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("C[0][0] = %.6f  (sanity check)\n", C[0][0]);
    printf("Execution time: %.6f seconds\n", elapsed);

    return 0;
}