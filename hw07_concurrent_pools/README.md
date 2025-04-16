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
Time used: 16072516 usec
Memory used: 20481310720 bytes
Overhead: 87.5%

Executing: ./test_localPools
Time used: 957626 usec
Memory used: 4326244352 bytes
Overhead: 40.8%

Executing: ./test_lockFreePool
Time used: 10506486 usec
Memory used: 10240393216 bytes
Overhead: 75.0%

Executing: ./test_mutexPool
Time used: 49745795 usec
Memory used: 10241441792 bytes
Overhead: 75.0%
```
