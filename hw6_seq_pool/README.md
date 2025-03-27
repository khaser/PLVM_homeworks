# Sequential pool
### Running
```
make test && ./test
```
### Benchmarking
Create & delete linked list of `10000000` elements.

Standart allocator:

```
Time used: 337109 usec
Memory used: 1222705152 bytes
Overhead: 86.9%
```

Sequential pool allocator:

```
Time used: 33648 usec
Memory used: 583254016 bytes
Overhead: 72.6%
```
