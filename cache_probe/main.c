#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define LINE_SZ 64
#define MAX_IDX_BITS 12
#define MAX_ASSOC 20

#define touch(x) __asm__ volatile ( "" : "+r" (x) )

#ifdef ENV_BARE
#include "syscalls.h"

typedef size_t my_clock;

my_clock getClock()
{
    size_t cycles;
    asm volatile ("rdcycle %0" : "=r"(cycles));
    return cycles;
}

long diffClock(const my_clock a, const my_clock b) {
   return (a - b);
}

// For bare-metal small number of iterations enough
#define ITERS 10
// ^^^ ENV_BARE
#else // LINUX
#include <sys/time.h>

typedef struct timeval my_clock;

my_clock getClock() {
   struct timeval tp;
   struct timezone tzp;

   gettimeofday(&tp,&tzp);
   return tp;
}

long diffClock(const my_clock a, const my_clock b) {
   return (a.tv_sec - b.tv_sec) * 1e6 + (a.tv_usec - b.tv_usec);
}

// For linux we should run more iterations to compensate non deterministic environment
#define ITERS 10000
// ^^^ LINUX
#endif

long test(uintptr_t* arr, int stride_lines, int spots) {
    int stride_idx = stride_lines * LINE_SZ / sizeof(uintptr_t);
    long m = spots * stride_idx;
    for (long i = 0; i < m; i += stride_idx) {
        arr[i] = (uintptr_t) &arr[(i + m - stride_idx) % m];
    }
    volatile my_clock start_time = getClock();
    volatile uintptr_t* cur = (uintptr_t*) (*arr);
    for (int i = 0; i < ITERS; ++i) {
#define X10(x) x; x; x; x; x; x; x; x; x; x;
#define X1000(x) X10(X10(X10(x)));
        X1000(cur = (uintptr_t*) (*cur));
    }
    touch(cur);
    volatile my_clock stop_time = getClock();
    return diffClock(stop_time, start_time);
}

int main() {

    const long n = MAX_ASSOC * LINE_SZ * (1 << MAX_IDX_BITS) / sizeof(uintptr_t);
    uintptr_t* arr = aligned_alloc(1 << MAX_IDX_BITS, n * sizeof(uintptr_t));
    printf("N: %ld, arr: %p\n", n, arr);

    // Calculate spots/stride table
    long ss_table[MAX_IDX_BITS + 1][MAX_ASSOC + 1];
    for (int spots = 1; spots <= MAX_ASSOC; ++spots) {
        for (int idx_bit = 0; idx_bit <= MAX_IDX_BITS; ++idx_bit) {
            ss_table[idx_bit][spots] = test(arr, 1 << idx_bit, spots);
        }
    }

    printf("Spots/Stride table\n");
    for (int idx_bit = 0; idx_bit <= MAX_IDX_BITS; ++idx_bit) {
        printf("\t\t%d", 1 << idx_bit);
    }
    printf("\n");

    for (int spots = 1; spots <= MAX_ASSOC; ++spots) {
        printf("%d", spots);
        for (int idx_bit = 0; idx_bit <= MAX_IDX_BITS; ++idx_bit) {
            printf("\t%ld", ss_table[idx_bit][spots]);
        }
        printf("\n");
    }

    // Assume that L1 data cache is smaller than TLB data cache
    // Search valuable deviation and select run that fits into smallest array
    int reference_run = ss_table[1][2];
    float threshold = 1.1 * reference_run;
    struct var {
        int assoc;
        int sets;
    } candidate = {MAX_ASSOC, 1 << MAX_IDX_BITS};
    for (int spots = 1; spots < MAX_ASSOC; ++spots) {
        for (int idx_bit = 0; idx_bit < MAX_IDX_BITS; ++idx_bit) {
            if (ss_table[idx_bit][spots + 1] > threshold &&
                ss_table[idx_bit + 1][spots] > threshold &&
                ss_table[idx_bit][spots] > threshold) {
                int sets = 1 << idx_bit;
                /* printf("Deviation, spots: %d, sets: %d\n", spots, sets); */
                if (candidate.assoc * candidate.sets > (spots - 1) * sets) {
                    candidate.assoc = (spots - 1);
                    candidate.sets = sets;
                }
                break;
            }
        }
    }

    printf("Supposed number of sets: %d\n", candidate.sets);
    printf("Supposed associative: %d\n", candidate.assoc);
    printf("Supposed cache size: %d Kb\n", (candidate.sets * candidate.assoc * LINE_SZ) >> 10);

    return 0;
}

