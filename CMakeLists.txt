# CMake minimum version
cmake_minimum_required (VERSION 3.1)

# Project Infomation
project( PIRM )
enable_language(ASM)

# Reset output path
set (CMAKE_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Use the FreeRTOS default is OFF, and you can set ON
option(FreeRTOS "Use the FreeRTOS ?" ON)
option(Modbus "Use the Modbus ?" ON)

# Setting the DEFINE to environment variables, other device can set here
set (ENV{DEFINE_DEVICE} STM32F10X_HD)
set (ENV{DEFINE_DRIVER} USE_STDPERIPH_DRIVER)
set (ENV{MCPU} cortex-m3)
# Set the startup and flash scripts path to the variables
set (startup ${CMAKE_SOURCE_DIR}/Drivers/CMSIS/Device/source/startup/startup_stm32f10x_hd.s)
set (ldscript ${CMAKE_SOURCE_DIR}/ldscripts/stm32f103vet6.ld)

# Add the source path
aux_source_directory (User/source DIR_USER_SRCS)
aux_source_directory (Drivers/CMSIS/Device/source DIR_CMSIS_SRCS)

# Add the include path
include_directories (User/include)
include_directories (Drivers/CMSIS/Device/include )
include_directories (Drivers/CMSIS/include )
include_directories (Drivers/STM32F10x_StdPeriph_Driver/inc)

# Add the STM32F10x_StdPeriph_Driver sub directory
add_subdirectory(Drivers)
IF(FreeRTOS)
	# Add the FreeRTOS sub directory
	add_subdirectory(ThirdParty/FreeRTOS)
	include_directories (ThirdParty/FreeRTOS/include)
ENDIF()
IF(Modbus)
	# Add the FreeRTOS sub directory
	add_subdirectory(ThirdParty/Modbus)
	include_directories (ThirdParty/Modbus/include)
ENDIF()

# CC AR LD AS
set (CMAKE_C_COMPILER "arm-none-eabi-gcc")
set (CMAKE_C_AR "arm-none-eabi-ar")
set (CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
# STDLIB
set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)

# CFLAGS
set(CMAKE_C_FLAGS "-g -mthumb -fno-builtin -mcpu=$ENV{MCPU} -Wall -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D $ENV{DEFINE_DEVICE} -D $ENV{DEFINE_DRIVER}")
# CXXFLAGS
set (CMAKE_CXX_FLAGS "-g -mthumb -fno-builtin -mcpu=$ENV{MCPU} -Wall -std=c++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -mabi=aapcs -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")
# ASMFLAGS
set (CMAKE_ASM_FLAGS "-g -mthumb -mcpu=$ENV{MCPU} -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")
# LDFLAGS
set (CMAKE_EXE_LINKER_FLAGS "-g -Wl,--gc-sections -mthumb -mcpu=$ENV{MCPU} -mabi=aapcs -specs=nosys.specs" CACHE INTERNAL "executable linker flags")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${ldscript}")

# Generate the target
add_executable (${CMAKE_PROJECT_NAME}.elf ${DIR_USER_SRCS} ${DIR_CMSIS_SRCS} ${startup})
# Link the standard library to the target
target_link_libraries (${CMAKE_PROJECT_NAME}.elf STM32F10x_StdPeriph_Driver)
IF(FreeRTOS)
	# Link the FreeRTOS library to the target
	target_link_libraries (${CMAKE_PROJECT_NAME}.elf STM32F10x_FreeRTOS)
ENDIF()
IF(Modbus)
	# Link the FreeRTOS library to the target
	target_link_libraries (${CMAKE_PROJECT_NAME}.elf STM32F10x_Modbus)
ENDIF()

# Generate the binary file
add_custom_target (${CMAKE_PROJECT_NAME}.bin ALL arm-none-eabi-objcopy -Obinary "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.bin" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf)
# Generate the hex file
add_custom_target (${CMAKE_PROJECT_NAME}.hex ALL arm-none-eabi-objcopy -Oihex "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.hex" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf)
# Echo the size Infomation
add_custom_target (size ALL arm-none-eabi-size "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf" DEPENDS ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.elf)

# Make flash to the board by st-link
add_custom_target (flash COMMAND st-flash write ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_PROJECT_NAME}.bin 0x8000000)
# Make clean-all
add_custom_target (clean-all COMMAND rm -rf ${CMAKE_BINARY_DIR}/*)
