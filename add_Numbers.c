#include <stdio.h>
#include <omp.h>

#define N 1000

int main() {
    long long serial_sum = 0;
    long long parallel_sum = 0;
    double serial_start, serial_end;
    double parallel_start, parallel_end;

    //  SERIAL version
    serial_start = omp_get_wtime();

    for (int i = 1; i <= N; i++) {
        serial_sum += i;
    }

    serial_end = omp_get_wtime();

    //  PARALLEL version
    parallel_start = omp_get_wtime();

#pragma omp parallel for reduction(+:parallel_sum)
    for (int i = 1; i <= N; i++) {
        parallel_sum += i;
    }

    parallel_end = omp_get_wtime();

    printf("===========================================\n");
    printf("  Sum from 1 to %d\n", N);
    printf("===========================================\n");
    printf("  Serial   sum    : %lld\n", serial_sum);
    printf("  Serial   time   : %.6f ms\n", (serial_end - serial_start) * 1000.0);
    printf("-------------------------------------------\n");
    printf("  Parallel sum    : %lld\n", parallel_sum);
    printf("  Parallel time   : %.6f ms\n", (parallel_end - parallel_start) * 1000.0);
    printf("-------------------------------------------\n");
    printf("  Threads used    : %d\n", omp_get_max_threads());
    printf("===========================================\n");

    return 0;
}
