set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

if(NOT DEFINED ARM_NONE_EABI_TOOLCHAIN_BIN)
    file(GLOB STM32CUBEIDE_TOOLCHAIN_BINS
        "/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*.macosaarch64_*/tools/bin"
        "/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins/com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.*.macosx_*/tools/bin"
    )
    if(STM32CUBEIDE_TOOLCHAIN_BINS)
        list(SORT STM32CUBEIDE_TOOLCHAIN_BINS)
        list(REVERSE STM32CUBEIDE_TOOLCHAIN_BINS)
        list(GET STM32CUBEIDE_TOOLCHAIN_BINS 0 ARM_NONE_EABI_TOOLCHAIN_BIN)
    endif()
endif()

if(DEFINED ARM_NONE_EABI_TOOLCHAIN_BIN AND EXISTS "${ARM_NONE_EABI_TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}gcc")
    set(ARM_NONE_EABI_TOOLCHAIN_BIN "${ARM_NONE_EABI_TOOLCHAIN_BIN}" CACHE PATH "Directory containing arm-none-eabi tools")
    set(CMAKE_C_COMPILER "${ARM_NONE_EABI_TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_CXX_COMPILER "${ARM_NONE_EABI_TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}g++")
    set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}gcc")
    set(CMAKE_OBJCOPY "${ARM_NONE_EABI_TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}objcopy" CACHE INTERNAL "objcopy")
    set(CMAKE_SIZE "${ARM_NONE_EABI_TOOLCHAIN_BIN}/${TOOLCHAIN_PREFIX}size" CACHE INTERNAL "size")
else()
    set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
    set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
    set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy CACHE INTERNAL "objcopy")
    set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size CACHE INTERNAL "size")
endif()

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CPU_FLAGS
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -mno-unaligned-access
)

add_compile_options(${CPU_FLAGS})
add_link_options(${CPU_FLAGS})

add_compile_options(
    -MMD
    -MP
)

add_link_options(
    --specs=nano.specs
    --specs=nosys.specs
    -nostartfiles
)
