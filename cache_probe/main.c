#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>

#define MAX_OFFSET_SZ 8 // maximum offset address part in bits
#define MAX_LINE_SZ (1 << MAX_OFFSET_SZ) // in bytes

#define MAX_IDX_SZ 12 // maximum index address part in bits
#define MAX_SETS (1 << MAX_IDX_SZ)

#define MAX_STRIDE ((int) (MAX_SETS * MAX_LINE_SZ / sizeof(uintptr_t)))
#define MAX_ASSOC 20

#define touch(x) __asm__ volatile ( "" : "+r" (x) )

struct timespec getClock() {
   struct timespec tp;
   clock_gettime(CLOCK_MONOTONIC, &tp);
   return tp;
}

long long diffClock(const struct timespec a, const struct timespec b) {
   return (a.tv_sec - b.tv_sec) * 1e9 + (a.tv_nsec - b.tv_nsec);
}

// For linux we should run more iterations to compensate non deterministic environment
#define ITERS 1000

long n; // arr size
uintptr_t* arr;

#define X10(x) x; x; x; x; x; x; x; x; x; x;
#define X1000(x) X10(X10(X10(x)));

long long test_line_size(int ss, int bs, int spots) {
    long m = spots * bs;
    for (long i = 0; i + ss < m; i += bs) {
        arr[i] = (uintptr_t) &arr[i + ss];
        arr[i + ss] = (uintptr_t) &arr[(i + bs) % m];
    }
    volatile struct timespec start_time = getClock();
    volatile uintptr_t* cur = (uintptr_t*) arr;
    for (int i = 0; i < ITERS; ++i) {
        X1000(cur = (uintptr_t*) (*cur));
    }
    touch(cur);
    volatile struct timespec stop_time = getClock();
    return diffClock(stop_time, start_time);
}

long long test(int stride_idx, int spots) {
    long m = spots * stride_idx;
    for (long i = 0; i < m; i += stride_idx) {
        arr[i] = (uintptr_t) &arr[(i + m - stride_idx) % m];
    }
    volatile struct timespec start_time = getClock();
    volatile uintptr_t* cur = (uintptr_t*) (*arr);
    for (int i = 0; i < ITERS; ++i) {
        X1000(cur = (uintptr_t*) (*cur));
    }
    touch(cur);
    volatile struct timespec stop_time = getClock();
    return diffClock(stop_time, start_time);
}

int main() {

    n = MAX_ASSOC * MAX_STRIDE;
    arr = aligned_alloc(MAX_SETS * MAX_LINE_SZ, n * sizeof(uintptr_t));
    printf("n: %ld, arr: %p\n", n, arr);

    // Calculate spots/stride table
    long long ss_table[MAX_IDX_SZ * MAX_OFFSET_SZ + 1][MAX_ASSOC + 1];
    for (int spots = 1; spots <= MAX_ASSOC; ++spots) {
        for (int idx_bit = 1; (1l << idx_bit) <= MAX_STRIDE; ++idx_bit) {
            ss_table[idx_bit][spots] = test(1 << idx_bit, spots);
        }
    }

    printf("Spots/Stride table\n");
    for (int idx_bit = 1; (1 << idx_bit) <= MAX_STRIDE; ++idx_bit) {
        printf("\t%d", 1 << idx_bit);
    }
    printf("\n");

    for (int spots = 1; spots <= MAX_ASSOC; ++spots) {
        printf("%d", spots);
        for (int idx_bit = 1; (1 << idx_bit) <= MAX_STRIDE; ++idx_bit) {
            printf("\t%lld", ss_table[idx_bit][spots]);
        }
        printf("\n");
    }

    long long acc = 0;
    int cnt = 0;
    for (int spots = 1; spots <= MAX_ASSOC; ++spots) {
        acc += ss_table[1][spots] + ss_table[2][spots];
        cnt += 2;
    }
    for (int idx_bit = 1; (1 << idx_bit) <= MAX_STRIDE; ++idx_bit) {
        acc += ss_table[idx_bit][1] + ss_table[idx_bit][2];
        cnt += 2;
    }
    float threshold = 1.15 * acc / cnt;
    printf("Deviation threshold: %2f\n", threshold);

    // Assume that L1 data cache is smaller than TLB data cache
    // Search valuable deviation and select run that fits into smallest array
    struct var {
        int assoc;
        int stride; // in sizeof(uintptr_t)
    } candidate = {MAX_ASSOC, MAX_STRIDE};
    for (int spots = 1; spots < MAX_ASSOC; ++spots) {
        for (int idx_bit = 1; (1 << idx_bit) <= MAX_STRIDE; ++idx_bit) {
            if (ss_table[idx_bit][spots + 1] > threshold &&
                ss_table[idx_bit + 1][spots] > threshold &&
                ss_table[idx_bit][spots] > threshold) {
                if (candidate.assoc * candidate.stride > (spots - 1) * (1 << idx_bit)) {
                    candidate.assoc = (spots - 1);
                    candidate.stride = (1 << idx_bit);
                }
                // printf("Spots: %d, stride: %d, measure: %d", spots, 1 << idx_bit, spots << idx_bit)
                break;
            }
        }
    }

    // Cache line detection
    // Use founded associative+2 & stride to ensure that L1 misses, but TLB hits
    // candidate_cache_line <  real_cache_line => L1 cache hit = 0.5
    // candidate_cache_line >= real_cache_line => L1 cache hit = 0
    int cache_line; // in sizeof(uintptr_t)
    long long last_time = test_line_size(1, candidate.stride, candidate.assoc + 2);
    for (cache_line = 2; cache_line <= candidate.stride; cache_line<<=1) {
        long long cur_time = test_line_size(cache_line, candidate.stride, candidate.assoc + 2);
        if (cur_time > last_time * 1.2) {
            break;
        }
    }

    printf("Cache line: %ld bytes\n", cache_line * sizeof(uintptr_t));
    printf("Number of sets: %d\n", candidate.stride / cache_line);
    printf("Associative: %d\n", candidate.assoc);
    printf("Cache size: %ld Kb\n", (candidate.stride * candidate.assoc * sizeof(uintptr_t)) >> 10);

    return 0;
}
