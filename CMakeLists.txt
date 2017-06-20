cmake_minimum_required(VERSION 2.8)

# Project name
set(PROJECT_NAME AvrProject)

# Include defaults
include(./tools/defaults.cmake)

# Variables regarding the AVR chip
set(MCU   atmega32)
set(F_CPU 8000000)
set(BAUD  9600)
set(PROG_TYPE usbasp)
set(PROG_ARGS )

# Set compilers
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_CXX_COMPILER ${AVRCPP})
set(CMAKE_C_COMPILER   ${AVRC})
set(CMAKE_ASM_COMPILER   ${AVRC})

# Set default avr compilers
project (${PROJECT_NAME} C CXX ASM)


# Include build system configuration
include(./tools/build.cmake)
