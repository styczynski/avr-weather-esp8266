
message(STATUS "Configuring build...")

math(EXPR F_CPU_MHZ "${F_CPU} / 1000000")

add_definitions(-DF_CPU=${F_CPU})

###
### Internals probably well configured for your purposes
###

# Important project paths
set(BASE_PATH    "${${PROJECT_NAME}_SOURCE_DIR}")
set(INC_PATH     "${BASE_PATH}/include" "${BASE_PATH}/src")
set(SRC_PATH     "${BASE_PATH}/src")
set(LIB_DIR_PATH "${BASE_PATH}/lib")

# Files to be compiled
file(GLOB SRC_FILES "${SRC_PATH}/*.cpp"
                    "${SRC_PATH}/*.cc"
                    "${SRC_PATH}/*.c"
                    "${SRC_PATH}/*.cxx"
                    "${SRC_PATH}/*.S"
                    "${SRC_PATH}/*.s"
                    "${SRC_PATH}/*.sx"
                    "${SRC_PATH}/*.asm")

set(LIB_SRC_FILES)
set(LIB_INC_PATH)

file(GLOB LIBRARIES "${LIB_DIR_PATH}/*")
foreach(subdir ${LIBRARIES})
    file(GLOB lib_files "${subdir}/*.cpp"
                        "${subdir}/*.cc"
                        "${subdir}/*.c"
                        "${subdir}/*.cxx"
                        "${subdir}/*.S"
                        "${subdir}/*.s"
                        "${subdir}/*.sx"
                        "${subdir}/*.asm")
    if(IS_DIRECTORY ${subdir})
        list(APPEND LIB_INC_PATH  "${subdir}")
    endif()
    list(APPEND LIB_SRC_FILES "${lib_files}")
endforeach()

# Compiler flags
set(CSTANDARD "-std=gnu99")
set(CDEBUG    "-gstabs -g -ggdb")
set(CWARN     "-Wall -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CTUNING   "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
set(COPT      "-Os -Wl,-u,vfprintf -lprintf_flt -Wl,-u,vfscanf -lscanf_flt -lm")
set(CMCU      "-mmcu=${MCU}")
set(CDEFS     "-DF_CPU=${F_CPU} -DBAUD=${BAUD}")

set(CFLAGS   "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING}")
set(CXXFLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CTUNING}")

set(CMAKE_C_FLAGS   "${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")
set(CMAKE_ASM_FLAGS   "${CFLAGS}")


list(APPEND LIB_INC_PATH "${BASE_PATH}/lib")

# Project setup
include_directories(${INC_PATH} ${LIB_INC_PATH})
add_executable(${PROJECT_NAME} ${SRC_FILES} ${LIB_SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

# Compiling targets
add_custom_target(strip ALL      ${AVRSTRIP} "${PROJECT_NAME}.elf" DEPENDS ${PROJECT_NAME})
add_custom_target(hex   ALL      ${OBJCOPY} -R .eeprom -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS strip)
add_custom_target(eeprom         ${OBJCOPY} -j .eeprom --change-section-lma .eeprom=0 -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.eeprom" DEPENDS strip)
add_custom_target(disassemble    ${OBJDUMP} -S "${PROJECT_NAME}.elf" > "${PROJECT_NAME}.lst" DEPENDS strip)
# Flashing targets
add_custom_target(upload         ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
add_custom_target(upload_usbtiny ${AVRDUDE} -c usbtiny -p ${MCU} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
add_custom_target(upload_usbasp  ${AVRDUDE} -c usbasp -p ${MCU} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
add_custom_target(upload_ardisp  ${AVRDUDE} -c avrisp -p ${MCU} -b 19200 -P ${USBPORT} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
add_custom_target(upload_109     ${AVRDUDE} -c avr109 -p ${MCU} -b 9600 -P ${USBPORT} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
add_custom_target(upload_eeprom  ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U eeprom:w:${PROJECT_NAME}.hex DEPENDS eeprom)
# Fuses (For ATMega328P-PU, Calculated using http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p)
add_custom_target(reset          ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -e)
#add_custom_target(fuses_1mhz    ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0x62:m)
#add_custom_target(fuses_8mhz    ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0xE2:m)
#add_custom_target(fuses_16mhz   ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0xFF:m)
#add_custom_target(fuses_uno     ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0x05:m)
#add_custom_target(set_eeprom_save_fuse   ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U hfuse:w:0xD1:m)
#add_custom_target(clear_eeprom_save_fuse ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U hfuse:w:0xD9:m)

# Utilities targets
add_custom_target(avr_terminal   ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -nt)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")


# Check doxygen installation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Point doxyfile
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    # Add "doc" target for generating docuemntation with Doxygen
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)


# Print configuration
message("*")
message("* Project configuration:")
message("*")
message("* Project Name  :\t${PROJECT_NAME}")
message("* Target device :\t${MCU} (${F_CPU_MHZ} MHz)")
message("* Programmer    :\t${PROG_TYPE}")
message("*")
