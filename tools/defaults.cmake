message(STATUS "Configuring default setup from defaults.cmake")

# Default target usbasp / atmega32 / 1 MHz
set(MCU   atmega32)
set(F_CPU 1000000)
set(BAUD  9600)
set(PROG_TYPE usbasp)
set(PROG_ARGS )

# Toolchain executables
set(AVRCPP   avr-g++)
set(AVRC     avr-gcc)
set(AVRSTRIP avr-strip)
set(OBJCOPY  avr-objcopy)
set(OBJDUMP  avr-objdump)
set(AVRSIZE  avr-size)
set(AVRDUDE  avrdude)
