# BSP

## Purpose

Board Support Package for STM32F746G-DISCO. It hides HAL, board wiring and peripheral policy behind reusable board-level APIs.

## Public API

Primary entry points are declared in `bsp.h` and the submodule headers:

- display: `bsp_display_init()`, `bsp_display_flush()`
- touch: `bsp_touch_init()`, `bsp_touch_read()`
- audio: `bsp_audio_init()`, `bsp_audio_play()`
- storage: `bsp_sdcard_init()`, `bsp_sdcard_is_present()`
- IO: `bsp_led_set()`, `bsp_button_read()`
- timing: `bsp_tick_get()`, `bsp_delay_ms()`

## Dependencies

`Config`, `Utilities`, CubeMX-generated HAL handles and board component drivers where required.

## Initialization Order

Generated HAL peripheral initialization happens first. Then `bsp_init()` initializes IO, display and touch. Optional modules such as audio and SD card can be initialized by application services when needed.

