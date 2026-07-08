cmake_minimum_required(VERSION 3.22)

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake" CACHE FILEPATH "Toolchain file")

project(stm32f746g_lvgl_framework C ASM)

include(CheckIncludeFile)
check_include_file(stdint.h TARGET_HAS_STDINT_H)
if(NOT TARGET_HAS_STDINT_H)
    message(FATAL_ERROR
        "The selected arm-none-eabi toolchain cannot compile <stdint.h>. "
        "Install a complete Arm GNU Toolchain package that includes target C library headers/newlib, "
        "then reconfigure from a clean build directory."
    )
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

set(PROJECT_TARGET ${PROJECT_NAME})
set(MCU_FAMILY STM32F7xx)
set(MCU_PART STM32F746xx)
set(BOARD_NAME STM32F746G-DISCO)

add_executable(${PROJECT_TARGET})

target_compile_definitions(${PROJECT_TARGET}
    PRIVATE
        ${MCU_PART}
        USE_HAL_DRIVER
        LV_CONF_INCLUDE_SIMPLE
)

target_compile_options(${PROJECT_TARGET}
    PRIVATE
        -Wall
        -Wextra
        -Wshadow
        -Wdouble-promotion
        -Wformat=2
        -Wundef
        -Wno-unused-parameter
        -ffunction-sections
        -fdata-sections
)

target_link_options(${PROJECT_TARGET}
    PRIVATE
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        -Wl,-Map=${PROJECT_TARGET}.map
)

include(cmake/stm32-generated.cmake)

add_subdirectory(Config)
add_subdirectory(Utilities)
add_subdirectory(BSP)
add_subdirectory(FileSystem)
add_subdirectory(LVGL)
add_subdirectory(Application)
add_subdirectory(RTOS)

stm32_collect_generated_sources(${PROJECT_TARGET})

set_property(TARGET ${PROJECT_TARGET} PROPERTY OUTPUT_NAME "${PROJECT_TARGET}")
set_property(TARGET ${PROJECT_TARGET} PROPERTY SUFFIX ".elf")

add_custom_command(TARGET ${PROJECT_TARGET} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_TARGET}> ${PROJECT_TARGET}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_TARGET}> ${PROJECT_TARGET}.bin
    COMMENT "Generating HEX and BIN images"
)
