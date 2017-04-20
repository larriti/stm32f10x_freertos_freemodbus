# STM32F10x port FreeRTOS and FreeModbus with the CMake

### Version requirements
* Use the offical STM32F10x_StdPeriph_Driver V3.5
* Use the FreeRTOS V8.2.3
* Use CMake less than V3.1

### Dependencies
```
arm-none-eabi-gcc
arm-none-eabi-gdb
stlink
```

### How to Install
1. make a directory in the project directory
```shell
  1. mkdir build
  2. cd build
```

2. cmake the project directory CMakeList.txt
```shell
  cmake ..
```

3. Build the Source
```shell
  make
```

### clean the all Build fil
```shell
  make clean-all
```

### flash with st-link V1/V2/V2.1
```shell
  make flash
```

