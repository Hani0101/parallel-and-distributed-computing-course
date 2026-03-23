#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define LIMIT 10000000000  // Find all primes up to this number

// SERIAL  —  Segmented Sieve of Eratosthenes
// ---------------------------------------------------------------------------
// A plain boolean array of 1 billion bytes would need ~1 GB of RAM.
// The segmented sieve fixes this: it sieves one small segment at a time,
// reusing the same buffer, so memory stays tiny (~1 MB per segment).
//
// How it works:
//   1. Find all "small" primes up to sqrt(LIMIT) with a simple sieve.
//   2. Divide [2 .. LIMIT] into segments of size SEGMENT_SIZE.
//   3. For each segment, mark multiples of every small prime → composites.
//   4. Whatever remains unmarked is prime.
// ---------------------------------------------------------------------------

#define SEGMENT_SIZE (1 << 19)   // 512 KB segment — fits in L2 cache

long long serial_sieve(void) {
    int    sqrt_limit = (int)sqrt((double)LIMIT) + 1;
    char  *small      = calloc(sqrt_limit + 1, 1); // 0 = prime, 1 = composite
    long long count   = 0;

    // Step 1: sieve small primes up to sqrt(LIMIT)
    small[0] = small[1] = 1;
    for (int i = 2; (long long)i * i <= sqrt_limit; i++)
        if (!small[i])
            for (int j = i * i; j <= sqrt_limit; j += i)
                small[j] = 1;

    // Collect small primes into an array for fast access
    int  *sprimes  = malloc((sqrt_limit + 1) * sizeof(int));
    int   nsprimes = 0;
    for (int i = 2; i <= sqrt_limit; i++)
        if (!small[i])
            sprimes[nsprimes++] = i;
    free(small);

    // Step 2: process each segment
    char *sieve = malloc(SEGMENT_SIZE);

    for (long long low = 2; low <= LIMIT; low += SEGMENT_SIZE) {
        long long high = low + SEGMENT_SIZE - 1;
        if (high > LIMIT) high = LIMIT;
        long long seg_size = high - low + 1;

        memset(sieve, 0, seg_size); // reset: 0 = prime candidate

        // Mark composites using each small prime
        for (int pi = 0; pi < nsprimes; pi++) {
            long long p    = sprimes[pi];
            // First multiple of p that falls inside [low, high]
            long long start = ((low + p - 1) / p) * p;
            if (start == p) start += p; // skip p itself

            for (long long j = start; j <= high; j += p)
                sieve[j - low] = 1;
        }

        // Count primes in this segment
        for (long long i = 0; i < seg_size; i++)
            if (!sieve[i]) count++;
    }

    free(sieve);
    free(sprimes);
    return count;
}

// PARALLEL  —  Parallel Segmented Sieve of Eratosthenes
// ---------------------------------------------------------------------------
// Same idea as the serial version, but each thread independently sieves
// its own segment — no shared writes, no race conditions.
//
// Each thread:
//   - Gets a private segment buffer (private to avoid false sharing)
//   - Uses the shared read-only small-primes array
//   - Accumulates into a local counter, then adds to the global total once

long long parallel_sieve(void) {
    int    sqrt_limit = (int)sqrt((double)LIMIT) + 1;
    char  *small      = calloc(sqrt_limit + 1, 1);
    long long count   = 0;

    // Step 1: same small-prime sieve (serial — fast, done once)
    small[0] = small[1] = 1;
    for (int i = 2; (long long)i * i <= sqrt_limit; i++)
        if (!small[i])
            for (int j = i * i; j <= sqrt_limit; j += i)
                small[j] = 1;

    int  *sprimes  = malloc((sqrt_limit + 1) * sizeof(int));
    int   nsprimes = 0;
    for (int i = 2; i <= sqrt_limit; i++)
        if (!small[i])
            sprimes[nsprimes++] = i;
    free(small);

    // Step 2: parallel loop over segments
    // Each iteration is one segment — fully independent of all others.
    // 'reduction(+:count)' safely sums each thread's local count at the end.
    // 'schedule(dynamic)' because segments near the start finish faster
    // (fewer small primes fit inside them), so dynamic balances the load.

    #pragma omp parallel for schedule(dynamic) reduction(+:count)
    for (long long low = 2; low <= LIMIT; low += SEGMENT_SIZE) {
        long long high     = low + SEGMENT_SIZE - 1;
        if (high > LIMIT)  high = LIMIT;
        long long seg_size = high - low + 1;

        char *sieve = malloc(SEGMENT_SIZE); // private per thread
        memset(sieve, 0, seg_size);

        for (int pi = 0; pi < nsprimes; pi++) {
            long long p     = sprimes[pi];
            long long start = ((low + p - 1) / p) * p;
            if (start == p) start += p;

            for (long long j = start; j <= high; j += p)
                sieve[j - low] = 1;
        }

        long long local_count = 0;
        for (long long i = 0; i < seg_size; i++)
            if (!sieve[i]) local_count++;

        count += local_count; // reduction handles this safely
        free(sieve);
    }

    free(sprimes);
    return count;
}

int main(void) {
    printf("============================================\n");
    printf("  Prime Counting up to %d\n", LIMIT);
    printf("============================================\n\n");

    struct timespec t0, t1;
    double elapsed;
    long long result;

    // Serial
    printf("[ SERIAL ]\n");
    clock_gettime(CLOCK_MONOTONIC, &t0);
    result = serial_sieve();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    printf("  Primes found : %lld\n", result);
    printf("  Time         : %.4f seconds\n\n", elapsed);

    // Parallel
    printf("[ PARALLEL — %d threads ]\n", omp_get_max_threads());
    clock_gettime(CLOCK_MONOTONIC, &t0);
    result = parallel_sieve();
    clock_gettime(CLOCK_MONOTONIC, &t1);
    elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
    printf("  Primes found : %lld\n", result);
    printf("  Time         : %.4f seconds\n\n", elapsed);

    printf("============================================\n");
    return 0;
}