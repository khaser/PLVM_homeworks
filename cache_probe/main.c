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
#define ITERS 1000
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
    uintptr_t* arr = malloc(n * sizeof(uintptr_t));
    printf("N: %ld, arr: %p\n", n, arr);

    for (int idx_bit = 0; idx_bit <= MAX_IDX_BITS; ++idx_bit) {
        printf("\t\t%d", 1 << idx_bit);
    }
    printf("\n");

    printf("Spots/Stride table\n");
    for (int spots = 2; spots <= MAX_ASSOC; ++spots) {
        printf("%d", spots);
        for (int idx_bit = 0; idx_bit <= MAX_IDX_BITS; ++idx_bit) {
            printf("\t%ld", test(arr, 1 << idx_bit, spots));
        }
        printf("\n");
    }

    // Probing associativity
    long last = test(arr, 1 << MAX_IDX_BITS, 1);
    int assoc = 0;
    for (; assoc <= MAX_ASSOC; ++assoc) {
        long cur = test(arr, 1 << MAX_IDX_BITS, assoc);
        if (last * 2 < cur) {
            assoc--;
            break;
        }
        last = cur;
    }
    if (!assoc) {
      printf("Failed to probe associativity\n");
      return 1;
    }
    printf("Supposed associativity: %d\n", assoc);

    // Probing idx bitness
    last = test(arr, 1 << MAX_IDX_BITS, assoc + 1);
    int idx_bits = MAX_IDX_BITS - 1;
    for (; idx_bits > 0; --idx_bits) {
        long cur = test(arr, 1 << idx_bits, assoc + 1);
        if (cur * 2 < last) {
            idx_bits++;
            break;
        }
    }
    if (!idx_bits) {
      printf("Failed to probe number of sets\n");
      return 1;
    }
    printf("Supposed number of sets: %d\n", 1 << idx_bits);
    printf("Supposed cache size(in Kb): %d\n", ((1 << idx_bits) * assoc * LINE_SZ) >> 10);

    return 0;
}

