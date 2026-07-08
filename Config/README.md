# Config

## Purpose

Central compile-time configuration for the board and LVGL.

## Public API

`board_config.h` defines board dimensions, framebuffer addresses and timing constants. `lv_conf.h` configures LVGL.

## Dependencies

None directly, but values must match CubeMX memory, LTDC and SDRAM configuration.

## Initialization Order

Configuration is included at compile time.

