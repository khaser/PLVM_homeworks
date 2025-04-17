# Multythread memcpy function

It's essential that threads using for coping are prelaunched

## Build
```
make
```

## Running tests
### Without sanitizers
```
./test
```

### Address sanitizer
```
./test-asan
```

### Thread sanitizer
```
./test-tsan
```

## Benchmarking
Results of running verion without sanitizers on i5-4300M:
```
Benchmarking parallel_memcpy...
Workers: 0      time: 0.130566s
Workers: 1      time: 0.129702s
Workers: 2      time: 0.079387s
Workers: 3      time: 0.0699343s
Workers: 4      time: 0.0611492s
Workers: 5      time: 0.0646105s
Workers: 6      time: 0.0587874s
Workers: 7      time: 0.0640604s
Workers: 8      time: 0.0607779s
```
