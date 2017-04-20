# CMake minimum version
cmake_minimum_required (VERSION 3.1)

# Set include path
include_directories(include)

# The need build source path and build all files
aux_source_directory (source DIR_STM32F10x_Modbus_SRCS)

# CC AR
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_C_AR "arm-none-eabi-ar")

# CFLAGS
set(CMAKE_C_FLAGS "-g -mthumb -fno-builtin -mcpu=$ENV{MCPU} -Wall -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")

# Generate the target to static linker library
add_library (STM32F10x_Modbus ${DIR_STM32F10x_Modbus_SRCS})
