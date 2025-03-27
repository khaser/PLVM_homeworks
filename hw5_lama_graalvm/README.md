# Lama implementation builded on GraalVM Truffle library

### Building
* shadow jar
```
gradle build
```
* GraalVM native image
```
gradle nativeCompile
```

### Testing
Submodule contains two test suits: official Lama `regression`, and useful for debugging `smoke`.

```
make (regression|smoke)_(java|native)
```

List of disabled `regression` tests and reasons for that can be found in `Makefile`.

### Benchmarking

#### Sort benchmark results
```
Upstream (recursive AST interpreter): 7.55 s
Upstream (recursive bytecode interpreter): 3.07 s
Lamai (runtime validation): ~1.95 s
Lamav (preprocess validation): ~1.87 s
Lama truffle native image: ~31 s
```

### Known issues

#### major unsupported features
* first class functions
* custom infix operators
* module system
#### misc unsupported features
* pattern matching type checks
* string pattern matching
* eta expansion

