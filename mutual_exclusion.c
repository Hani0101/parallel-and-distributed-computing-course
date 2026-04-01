// #include <stdio.h>
// #include <omp.h>
//
// int main() {
//     int counter = 0;
//
// #pragma omp parallel for num_threads(4)
//     for (int i = 0; i < 100000; i++) {
//         counter++;  // RACE CONDITION — multiple threads read/write simultaneously
//     }
//
//     printf("Expected: 100000 | Got: %d\n", counter);
//     return 0;
// }

// #include <stdio.h>
// #include <omp.h>
//
// int main() {
//     int counter = 0;
//
// #pragma omp parallel for num_threads(4)
//     for (int i = 0; i < 100000; i++) {
// #pragma omp atomic
//         counter++;  // Single indivisible hardware instruction — no thread can interrupt it
//     }
//
//     printf("Expected: 100000 | Got: %d\n", counter);  // Always 100000
//     return 0;
// }


#include <stdio.h>
#include <omp.h>

int main() {
    int counter = 0;

#pragma omp parallel for num_threads(4)
    for (int i = 0; i < 100000; i++) {
#pragma omp critical
        {
            counter++;  // Only one thread allowed inside at a time
        }
    }

    printf("Expected: 100000 | Got: %d\n", counter);  // Always 100000
    return 0;
}