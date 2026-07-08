# STM32F746 Discovery LVGL Framework

Reusable firmware framework for the STM32F746G-DISCO board using CMake, Ninja, arm-none-eabi-gcc and LVGL v9.

This repository is organized as a production firmware foundation, not as a CubeMX example. CubeMX or the official STM32CubeF7 board templates own only low-level HAL code, startup code, clocks, linker script and peripheral initialization. Application logic, board policy and LVGL integration live in separate reusable modules.

## Architecture

- `Core/`: CubeMX-generated startup, HAL MSP, interrupt handlers, clock setup and `main.c`.
- `Drivers/`: CubeMX-provided CMSIS and STM32 HAL drivers.
- `BSP/`: board support package. This is the only layer that should talk directly to HAL or board-specific low-level drivers.
- `FileSystem/`: SD-card filesystem abstraction over FatFS.
- `LVGL/`: LVGL source tree plus the display and input porting layer.
- `Application/`: reusable application shell, screens and services.
- `RTOS/`: FreeRTOS integration, kernel hooks and scheduler glue.
- `Middlewares/`: vendored third-party middleware, currently FreeRTOS and FatFS.
- `Config/`: board configuration and `lv_conf.h`.
- `Utilities/`: shared status, logging and small platform-independent helpers.
- `Assets/`: fonts and images.
- `Examples/`: focused sample applications built on top of the framework.
- `cmake/`: cross-toolchain and generated-code integration helpers.

## Layering Rules

Application code calls BSP and service APIs. It must not call HAL directly.

LVGL-facing code stays in `LVGL/Port` and screen modules. Peripheral drivers should not include LVGL headers.

CubeMX-generated files remain regeneratable. Keep user application code out of generated sections except for minimal bootstrapping calls such as `app_init()` and `app_run()`.

## BSP API

The BSP exposes high-level board capabilities:

- `bsp_init()`
- `bsp_display_init()`
- `bsp_display_flush()`
- `bsp_touch_init()`
- `bsp_touch_read()`
- `bsp_audio_init()`
- `bsp_audio_play()`
- `bsp_sdcard_init()`
- `bsp_sdcard_read_blocks()`
- `bsp_sdcard_write_blocks()`
- `bsp_led_set()`
- `bsp_button_read()`
- `bsp_tick_get()`

Each BSP submodule has a narrow public header and owns its hardware dependencies internally.

## Initialization Order

1. `mpu_config()` and CPU cache enable.
2. `HAL_Init()`.
3. 216 MHz HSE/PLL system clock setup.
4. `app_init()`.
5. `app_run()`, which creates RTOS tasks and starts the FreeRTOS scheduler.

`Core/Src/main.c` only bootstraps the framework:

```c
#include "app.h"

int main(void)
{
    mpu_config();
    cpu_cache_enable();
    HAL_Init();
    system_clock_config();

    if (app_init() != APP_OK) {
        Error_Handler();
    }

    app_run();
}
```

## FreeRTOS Integration

FreeRTOS is vendored under `Middlewares/Third_Party/FreeRTOS` and wired through `RTOS/CMakeLists.txt`.

The current baseline keeps hardware initialization deterministic before the scheduler starts, then creates focused application tasks:

- `gui`: owns LVGL task handling and all GUI updates.
- `system`: owns LED heartbeat and periodic system logs.

`SysTick_Handler()` advances HAL, LVGL and FreeRTOS ticks. FreeRTOS owns `SVC` and `PendSV`.

The kernel configuration lives in `Config/FreeRTOSConfig.h`:

- 1 ms kernel tick.
- 64 KB FreeRTOS heap using `heap_4`.
- Stack overflow and malloc-failed hooks.
- Cortex-M7 GCC port `ARM_CM7/r0p1`.

## File System

The active filesystem target is the microSD card only.

`FileSystem/` exposes a small reusable API over FatFS:

- `fs_init()`
- `fs_mount(FS_VOLUME_SD, format_if_needed)`
- `fs_unmount(FS_VOLUME_SD)`
- `fs_get_info(FS_VOLUME_SD, ...)`
- `fs_file_open()`, `fs_file_read()`, `fs_file_write()`, `fs_file_close()`
- `fs_dir_open()`, `fs_dir_read()`, `fs_dir_close()`

The filesystem layer depends on:

- FatFS under `Middlewares/Third_Party/FatFs`
- `BSP/Storage/bsp_sdcard.*`
- the ST board SD driver wrapped by the BSP

The framework deliberately does not mount or format storage automatically at boot. Applications or storage services should explicitly decide when to mount or format the SD card.

Internal MCU flash is reserved for firmware code and constants. It is not used as a filesystem. External QSPI flash is not part of the current filesystem architecture.

## LVGL Integration

`LVGL/Port` creates the LVGL display and input devices. The display port delegates pixel transfer to `bsp_display_flush()` and reads touch state through `bsp_touch_read()`.

The initial display configuration uses SDRAM framebuffers at:

- `0xC0000000`
- `0xC0000000 + width * height * 4`

The LVGL heap is placed in SDRAM at `0xC0100000`, after the double framebuffer region. This keeps internal SRAM available for stack, HAL state and performance-sensitive data.

DMA2D acceleration is enabled in `Config/lv_conf.h` when `stm32f7xx_hal.h` is available. The BSP display flush path uses DMA2D memory-to-memory transfers from the LVGL draw buffer into the active LTDC framebuffer. Cache maintenance is scoped to the source draw buffer and the affected framebuffer rows instead of cleaning the full framebuffer on every flush.

## Current Board Baseline

The project includes a buildable STM32F746G-DISCO baseline assembled from the local official STM32CubeF7 package:

- `STM32F746G-DISCO.ioc` from the STM32CubeMX board database.
- `Core/Startup/startup_stm32f746xx.s`.
- `STM32F746NGHX_FLASH.ld`.
- `Core/Src/main.c`, `system_stm32f7xx.c`, `stm32f7xx_it.c`, `stm32f7xx_hal_msp.c`, `syscalls.c`.
- STM32F7 HAL/CMSIS under `Drivers/`.
- Official ST board drivers under `Drivers/BSP/STM32746G-Discovery`.
- FatFS middleware under `Middlewares/Third_Party/FatFs` for SD-card file access.

The framework BSP wraps the ST board drivers so application code still uses only the project BSP API.

## Serial Logging

Runtime logs are emitted through the ST-LINK Virtual COM Port using USART1 at `115200 8N1`.

On macOS, find the port and open a terminal with:

```sh
ls /dev/cu.usbmodem*
screen /dev/cu.usbmodemXXXX 115200
```

Exit `screen` with `Ctrl-A`, then `Ctrl-\`.

## Getting LVGL

Add LVGL v9 under `LVGL/lvgl`:

```sh
git submodule add https://github.com/lvgl/lvgl.git LVGL/lvgl
cd LVGL/lvgl
git checkout release/v9.2
```

## CubeMX Generation

When regenerating with CubeMX, generate only:

- HAL and CMSIS driver files
- startup assembly
- linker script
- clock configuration
- peripheral initialization

Recommended CubeMX output paths:

- `Core/Inc`
- `Core/Src`
- `Core/Startup`
- `Drivers`
- `STM32F746NGHX_FLASH.ld`

Keep all custom reusable code outside `Core/` and `Drivers/`.

## Build

Install the toolchain on macOS:

```sh
brew install cmake ninja arm-none-eabi-gcc
```

If STM32CubeIDE is installed, the CMake toolchain file automatically prefers the complete ST-provided Arm GNU Toolchain inside the CubeIDE application bundle. This is usually better than the Homebrew `arm-none-eabi-gcc` formula because it includes the target C library headers needed by STM32 firmware builds.

If the build fails with `fatal error: stdint.h: No such file or directory`, the compiler is present but the target C library headers are missing. Install a complete Arm GNU Toolchain package that includes newlib/C library headers. Arm publishes official Arm GNU Toolchain downloads for Cortex-M bare-metal development at <https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads>.

Configure and build the normal debug firmware:

```sh
cmake --preset debug
cmake --build --preset debug
```

Build the size-optimized production firmware:

```sh
cmake --preset release-size
cmake --build --preset release-size
```

Optionally test link-time optimization for the smallest production image:

```sh
cmake --preset release-size-lto
cmake --build --preset release-size-lto
```

`release-size-lto` enables `ENABLE_LTO=ON`. Keep it as an explicit production choice and validate it on hardware before treating it as the default flashing image.

Each build prints an automatic firmware size report after linking and produces:

- `build/stm32f746g_lvgl_framework.elf`
- `build/stm32f746g_lvgl_framework.hex`
- `build/stm32f746g_lvgl_framework.bin`

For the production presets, replace `build/` with `build-release-size/` or `build-release-size-lto/`.

## Flashing

Using OpenOCD:

```sh
openocd -f interface/stlink.cfg -f target/stm32f7x.cfg \
  -c "program build/stm32f746g_lvgl_framework.elf verify reset exit"
```

## Debugging

Use VS Code Cortex-Debug with OpenOCD:

```json
{
  "name": "Debug STM32F746G-DISCO",
  "type": "cortex-debug",
  "request": "launch",
  "servertype": "openocd",
  "cwd": "${workspaceFolder}",
  "executable": "${workspaceFolder}/build/stm32f746g_lvgl_framework.elf",
  "configFiles": [
    "interface/stlink.cfg",
    "target/stm32f7x.cfg"
  ],
  "device": "STM32F746NGHx"
}
```

## Future Expansion

Applications such as a media player, Winamp-style UI, image viewer, file explorer, dashboard, oscilloscope, settings app, terminal, OTA updater or game launcher should be implemented above the BSP. New hardware behavior should be added behind existing BSP module boundaries or as a new BSP submodule with a focused public API.

## Next Engineering Steps

1. Generate the CubeMX baseline for STM32F746NG and commit it separately.
2. Add LVGL v9 as a submodule under `LVGL/lvgl`.
3. Replace placeholder BSP internals with STM32F746G-DISCO HAL/BSP calls.
4. Add DMA2D-backed display fill/blit paths and cache maintenance.
5. Add a focused storage/media task that mounts the SD card on demand and exposes file events to applications.
6. Add a file explorer screen over the existing `fs_*` API.
7. Add unit-testable application services that can build on host where practical.
8. Add focused RTOS tasks for media playback, audio and USB as those subsystems come online.
