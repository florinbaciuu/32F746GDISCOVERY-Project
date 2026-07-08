# BSP Display

## Purpose

Owns display policy for LTDC, DMA2D and SDRAM framebuffers.

## Public API

`bsp_display_init()`, `bsp_display_get_config()`, `bsp_display_flush()` and `bsp_display_set_backlight()`.

## Dependencies

Board configuration, SDRAM, LTDC, DMA2D and cache maintenance hooks.

## Initialization Order

SDRAM, DMA2D and LTDC must be initialized by generated code before `bsp_display_init()`.

