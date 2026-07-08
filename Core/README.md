# Core

## Purpose

CubeMX-generated startup, clock, interrupt and peripheral initialization code.

## Public API

Only the generated initialization functions used by `main.c`, such as `SystemClock_Config()` and `MX_*_Init()`.

## Dependencies

STM32Cube HAL, CMSIS and the linker/startup files generated for STM32F746NG.

## Initialization Order

`HAL_Init()`, `SystemClock_Config()`, generated peripheral initialization, then `app_init()` and `app_run()`.

Keep reusable application code out of this directory so CubeMX regeneration stays safe.

