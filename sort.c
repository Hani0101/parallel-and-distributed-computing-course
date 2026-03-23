#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define SIZE 100000

// ─────────────────────────────────────────
//  Utility: fill array with random values
// ─────────────────────────────────────────
void fill_random(int *arr, int n) {
    srand(42); // fixed seed → reproducible
    for (int i = 0; i < n; i++)
        arr[i] = rand() % 1000000;
}

// ─────────────────────────────────────────
//  Utility: copy array
// ─────────────────────────────────────────
void copy_array(int *dst, const int *src, int n) {
    memcpy(dst, src, n * sizeof(int));
}

// ─────────────────────────────────────────
//  Utility: print first/last few elements
// ─────────────────────────────────────────
void print_preview(const char *label, int *arr, int n) {
    printf("%s (first 8): ", label);
    for (int i = 0; i < 8 && i < n; i++) printf("%d ", arr[i]);
    printf("... ");
    for (int i = n - 4; i < n; i++) printf("%d ", arr[i]);
    printf("\n");
}

// ─────────────────────────────────────────
//  Comparator for qsort / stdlib
// ─────────────────────────────────────────
int cmp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// ─────────────────────────────────────────
//  SERIAL: simple quicksort via stdlib
// ─────────────────────────────────────────
void serial_sort(int *arr, int n) {
    qsort(arr, n, sizeof(int), cmp);
}

// ─────────────────────────────────────────
//  PARALLEL: parallel merge sort with OpenMP
//  Each thread sorts its own chunk, then
//  chunks are merged sequentially.
// ─────────────────────────────────────────

// Merge two sorted halves in-place using a temp buffer
void merge(int *arr, int left, int mid, int right) {
    int len = right - left + 1;
    int *tmp = (int *)malloc(len * sizeof(int));

    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right)
        tmp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
    while (i <= mid)  tmp[k++] = arr[i++];
    while (j <= right) tmp[k++] = arr[j++];

    memcpy(arr + left, tmp, len * sizeof(int));
    free(tmp);
}

void parallel_sort(int *arr, int n) {
    int num_threads = omp_get_max_threads();

    // Divide array into chunks — one per thread
    int chunk = n / num_threads;

    // ── Step 1: each thread sorts its own chunk ──
    #pragma omp parallel for schedule(static)
    for (int t = 0; t < num_threads; t++) {
        int start = t * chunk;
        int end   = (t == num_threads - 1) ? n : start + chunk;
        qsort(arr + start, end - start, sizeof(int), cmp);
    }

    // ── Step 2: merge sorted chunks (sequential merge tree) ──
    int width = chunk;
    while (width < n) {
        for (int left = 0; left < n - width; left += 2 * width) {
            int mid   = left + width - 1;
            int right = left + 2 * width - 1;
            if (right >= n) right = n - 1;
            merge(arr, left, mid, right);
        }
        width *= 2;
    }
}

// ─────────────────────────────────────────
//  MAIN
// ─────────────────────────────────────────
int main() {
    int *original = (int *)malloc(SIZE * sizeof(int));
    int *serial   = (int *)malloc(SIZE * sizeof(int));
    int *parallel = (int *)malloc(SIZE * sizeof(int));

    // Build one unsorted array, copy it for both runs
    fill_random(original, SIZE);
    copy_array(serial,   original, SIZE);
    copy_array(parallel, original, SIZE);

    printf("===========================================\n");
    printf("  Array size : %d elements\n", SIZE);
    printf("  Threads    : %d\n", omp_get_max_threads());
    printf("===========================================\n");
    print_preview("Unsorted", original, SIZE);
    printf("-------------------------------------------\n");

    // ── Serial sort ──
    double t1 = omp_get_wtime();
    serial_sort(serial, SIZE);
    double t2 = omp_get_wtime();

    print_preview("Serial  ", serial, SIZE);
    printf("  Serial   time : %.9f seconds\n", t2 - t1);
    printf("-------------------------------------------\n");

    // ── Parallel sort ──
    double t3 = omp_get_wtime();
    parallel_sort(parallel, SIZE);
    double t4 = omp_get_wtime();

    print_preview("Parallel", parallel, SIZE);
    printf("  Parallel time : %.9f seconds\n", t4 - t3);
    printf("-------------------------------------------\n");
    printf("  Speedup       : %.2fx\n", (t2 - t1) / (t4 - t3));
    printf("===========================================\n");

    free(original);
    free(serial);
    free(parallel);
    return 0;
}