function(stm32_collect_generated_sources target_name)
    set(core_sources
        Core/Src/main.c
        Core/Src/stm32f7xx_hal_msp.c
        Core/Src/stm32f7xx_it.c
        Core/Src/system_stm32f7xx.c
        Core/Src/syscalls.c
        Core/Src/sysmem.c
        Core/Src/runtime_hooks.c
    )

    set(startup_candidates
        Core/Startup/startup_stm32f746xx.s
        startup_stm32f746xx.s
    )

    foreach(source_file IN LISTS core_sources startup_candidates)
        if(EXISTS "${CMAKE_SOURCE_DIR}/${source_file}")
            target_sources(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/${source_file}")
        endif()
    endforeach()

    if(EXISTS "${CMAKE_SOURCE_DIR}/Core/Inc")
        target_include_directories(${target_name} PRIVATE Core/Inc)
    endif()

    if(EXISTS "${CMAKE_SOURCE_DIR}/Drivers")
        target_include_directories(${target_name}
            PRIVATE
                Drivers/STM32F7xx_HAL_Driver/Inc
                Drivers/STM32F7xx_HAL_Driver/Inc/Legacy
                Drivers/CMSIS/Device/ST/STM32F7xx/Include
                Drivers/CMSIS/Include
                Drivers/BSP/STM32746G-Discovery
                Drivers/BSP/Components/Common
                Drivers/BSP/Components/rk043fn48h
                Drivers/BSP/Components/ft5336
                Drivers/BSP/Components/wm8994
                Drivers/BSP/Components/n25q128a
                Drivers/BSP/Components/w25q128j
                Utilities/Fonts
        )

        file(GLOB HAL_SOURCES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/*.c"
        )
        list(FILTER HAL_SOURCES EXCLUDE REGEX ".*_template\\.c$")
        target_sources(${target_name} PRIVATE ${HAL_SOURCES})

        set(board_support_sources
            Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c
            Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.c
            Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.c
            Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.c
            Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_audio.c
            Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.c
            Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_qspi.c
            Drivers/BSP/Components/rk043fn48h/rk043fn48h.c
            Drivers/BSP/Components/ft5336/ft5336.c
            Drivers/BSP/Components/wm8994/wm8994.c
            Drivers/BSP/Components/n25q128a/n25q128a.c
        )

        foreach(source_file IN LISTS board_support_sources)
            if(EXISTS "${CMAKE_SOURCE_DIR}/${source_file}")
                target_sources(${target_name} PRIVATE "${CMAKE_SOURCE_DIR}/${source_file}")
            endif()
        endforeach()
    endif()

    set(linker_script "${CMAKE_SOURCE_DIR}/STM32F746NGHX_FLASH.ld")
    if(EXISTS "${linker_script}")
        target_link_options(${target_name} PRIVATE -T${linker_script})
    else()
        message(WARNING "STM32 linker script not found. Generate STM32F746NGHX_FLASH.ld with CubeMX before linking firmware.")
    endif()
endfunction()
