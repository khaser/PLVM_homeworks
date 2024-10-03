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
* Different RISC-V cores, bare-metal & under Linux (FPGA, CloudBEAR)
* Intel Core i5-4300M, under Linux (author laptop)
* AMD A12

