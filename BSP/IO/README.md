# BSP IO

## Purpose

Provides stable LED and button APIs for application code.

## Public API

`bsp_io_init()`, `bsp_led_set()` and `bsp_button_read()`.

## Dependencies

Generated GPIO initialization.

## Initialization Order

GPIO must be initialized before `bsp_io_init()`.

