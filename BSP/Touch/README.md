# BSP Touch

## Purpose

Provides a board-level touchscreen API independent of LVGL and application screens.

## Public API

`bsp_touch_init()` and `bsp_touch_read()`.

## Dependencies

Generated I2C/GPIO initialization and the touchscreen controller driver.

## Initialization Order

I2C and GPIO must be initialized before `bsp_touch_init()`.

