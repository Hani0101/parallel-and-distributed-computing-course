#include <stdio.h>
#include <omp.h>

int main()
{
#pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int total = omp_get_num_threads();
        printf("Hello from thread %d of %d\n", thread_id, total);
    }
    return 0;
}

// //wa7ad serial wa7ad parallel
// // 1-loop for add numbers from 1 to 1000 (print execution time)
// // 2-sorting
//
//
// #include <stdio.h>
// #include <omp.h>
//
// #define N 1000
//
// int main() {
//     long long serial_sum = 0;
//     long long parallel_sum = 0;
//     double serial_start, serial_end;
//     double parallel_start, parallel_end;
//
//     // ─────────────────────────────────────────
//     //  SERIAL version
//     // ─────────────────────────────────────────
//     serial_start = omp_get_wtime();
//
//     for (int i = 1; i <= N; i++) {
//         serial_sum += i;
//     }
//
//     serial_end = omp_get_wtime();
//
//     // ─────────────────────────────────────────
//     //  PARALLEL version
//     // ─────────────────────────────────────────
//     parallel_start = omp_get_wtime();
//
// #pragma omp parallel for reduction(+:parallel_sum)
//     for (int i = 1; i <= N; i++) {
//         parallel_sum += i;
//     }
//
//     parallel_end = omp_get_wtime();
//
//     // ─────────────────────────────────────────
//     //  Results
//     // ─────────────────────────────────────────
//     printf("===========================================\n");
//     // printf("  Sum from 1 to %d\n", N);
//     printf("===========================================\n");
//     printf("  Serial   sum    : %lld\n", serial_sum);
//     printf("  Serial   time   : %.9f seconds\n", serial_end - serial_start);
//     printf("-------------------------------------------\n");
//     printf("  Parallel sum    : %lld\n", parallel_sum);
//     printf("  Parallel time   : %.9f seconds\n", parallel_end - parallel_start);
//     printf("-------------------------------------------\n");
//     printf("  Threads used    : %d\n", omp_get_max_threads());
//     printf("===========================================\n");
//
//     return 0;
// }
