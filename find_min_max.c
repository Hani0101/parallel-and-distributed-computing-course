#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <omp.h>

#define N 1000000000
#define NUM_THREADS 4

static void fill_array(int *arr, int n)
{
    srand(42);
    for (int i = 0; i < n; i++)
        arr[i] = rand();
}

static void serial_minmax(const int *arr, int n, int *out_min, int *out_max)
{
    int mn = arr[0], mx = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] < mn) mn = arr[i];
        if (arr[i] > mx) mx = arr[i];
    }
    *out_min = mn;
    *out_max = mx;
}

static void parallel_minmax(const int *arr, int n, int *out_min, int *out_max)
{
    int mn = arr[0], mx = arr[0];

    #pragma omp parallel for num_threads(NUM_THREADS) reduction(min:mn) reduction(max:mx)
    for (int i = 1; i < n; i++) {
        if (arr[i] < mn) mn = arr[i];
        if (arr[i] > mx) mx = arr[i];
    }
    *out_min = mn;
    *out_max = mx;
}

static void sections_independent_minmax(const int *arr, int n,
                                        int *out_min, int *out_max)
{
    int mn = INT_MAX, mx = INT_MIN;
    int mn_A, mx_A, mn_B, mx_B;

    #pragma omp parallel sections num_threads(2)
    {
        #pragma omp section
        {
            int local_mn = arr[0], local_mx = arr[0];
            for (int i = 1; i < n; i++) {
                if (arr[i] < local_mn) local_mn = arr[i];
                if (arr[i] > local_mx) local_mx = arr[i];
            }
            mn_A = local_mn;
            mx_A = local_mx;
        }

        #pragma omp section
        {
            int local_mn = arr[0], local_mx = arr[0];
            for (int i = 1; i < n; i++) {
                if (arr[i] < local_mn) local_mn = arr[i];
                if (arr[i] > local_mx) local_mx = arr[i];
            }
            mn_B = local_mn;
            mx_B = local_mx;
        }
    }

    mn = (mn_A < mn_B) ? mn_A : mn_B;
    mx = (mx_A > mx_B) ? mx_A : mx_B;

    *out_min = mn;
    *out_max = mx;
}

static void sections_grouped_minmax(const int *arr, int n,
                                    int *out_min, int *out_max)
{
    int mn = INT_MAX, mx = INT_MIN;

    #pragma omp parallel sections num_threads(2)
    {
        #pragma omp section
        {
            int local_mn = arr[0];
            for (int i = 1; i < n; i++)
                if (arr[i] < local_mn) local_mn = arr[i];
            mn = local_mn;
        // }
        //
        // #pragma omp section
        // {
            int local_mx = arr[0];
            for (int i = 1; i < n; i++)
                if (arr[i] > local_mx) local_mx = arr[i];
            mx = local_mx;
        }
    }

    *out_min = mn;
    *out_max = mx;
}

int main(void)
{
    int *arr = (int *)malloc((size_t)N * sizeof(int));
    if (!arr) { fprintf(stderr, "malloc failed\n"); return 1; }

    printf("Filling array with %d random elements...\n\n", N);
    fill_array(arr, N);

    int mn, mx;
    double t_start, t_end;


    printf("%-30s %10s %10s %12s\n",
           "Method", "MIN", "MAX", "Time (s)");
    printf("%-30s %10s %10s %12s\n",
           "------", "---", "---", "--------");

    t_start = omp_get_wtime();
    serial_minmax(arr, N, &mn, &mx);
    t_end = omp_get_wtime();
    printf("%-30s %10d %10d %12.6f\n",
           "1. Serial", mn, mx, t_end - t_start);

    t_start = omp_get_wtime();
    parallel_minmax(arr, N, &mn, &mx);
    t_end = omp_get_wtime();
    printf("%-30s %10d %10d %12.6f\n",
           "2. Parallel (reduction)", mn, mx, t_end - t_start);

    t_start = omp_get_wtime();
    sections_independent_minmax(arr, N, &mn, &mx);
    t_end = omp_get_wtime();
    printf("%-30s %10d %10d %12.6f\n",
           "3. Sections (independent)", mn, mx, t_end - t_start);

    t_start = omp_get_wtime();
    sections_grouped_minmax(arr, N, &mn, &mx);
    t_end = omp_get_wtime();
    printf("%-30s %10d %10d %12.6f\n",
           "4. Sections (grouped)", mn, mx, t_end - t_start);

    printf("\nDone.\n");
    free(arr);
    return 0;
}