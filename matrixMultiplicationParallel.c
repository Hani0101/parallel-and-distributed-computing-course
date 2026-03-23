#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define N 1200  // Matrix size (N x N)

// Matrices declared globally to avoid large stack allocation
double A[N][N];
double B[N][N];
double C[N][N];

void fill_random(double mat[N][N]) {
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            mat[i][j] = (double)rand() / RAND_MAX * 10.0; //times 10 to give us results between 0 and 10
}

void matrix_multiply_parallel(void) {
#pragma omp parallel for collapse(2) schedule(static) //collapse merges the two for loops into one pool of iterations (e.g. 40,000 iteration in the case of 200x200)
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < N; k++) // not added to collapse since it will create a race condition, we keep it serialized instead
                C[i][j] += A[i][k] * B[k][j];
        }
}

int main(void) {
    srand(42);  // Same seed as serial for comparable results

    fill_random(A);
    fill_random(B);

    int num_threads = omp_get_max_threads();
    printf("=== Parallel Matrix Multiplication (OpenMP) ===\n");
    printf("Matrix size  : %d x %d\n", N, N);
    printf("Threads used : %d\n\n", num_threads);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    matrix_multiply_parallel();

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec)
                   + (end.tv_nsec - start.tv_nsec) / 1e9; // meaning?

    printf("C[0][0] = %.6f  (sanity check)\n", C[0][0]);
    printf("Execution time: %.6f seconds\n", elapsed);

    return 0;
}