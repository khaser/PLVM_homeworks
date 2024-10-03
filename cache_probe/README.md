# Cache probe
### Build, run
```
make build
./build_/cache_probe
make clean
```
### System setup
Before running `cache_probe` under linux please ensure, that hyper-threading disabled:
```
echo off | sudo tee /sys/devices/system/cpu/smt/control
```

### Tested configurations
* Different RISC-V cores, bare-metal (FPGA)
* Intel Core i5-4300M, on Linux (author laptop)
* AMD A12-9720P, on Linux (author laptop)

For test results see `examples` directory

