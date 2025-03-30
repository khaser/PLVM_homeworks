# Sequential pool
### Running
Compile & run four variants:
```
make -j8
```

### Benchmarking
Create & delete linked list of `10000000` elements.

Results on I5-4300M:
```
Executing: ./test_malloc
Time used: 16453090 usec
Memory used: 20480262144 bytes
Overhead: 87.5%

Executing: ./test_localPools
Time used: 998666 usec
Memory used: 6505807872 bytes
Overhead: 60.7%

Executing: ./test_lockFreePool
Time used: 10466339 usec
Memory used: 10240917504 bytes
Overhead: 75.0%

Executing: ./test_mutexPool
Time used: 62700381 usec
Memory used: 10241441792 bytes
Overhead: 75.0%
```
