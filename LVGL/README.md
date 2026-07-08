# LVGL

## Purpose

Contains LVGL v9 and the board-specific LVGL port.

## Public API

The application uses LVGL normally in screen modules. Framework startup uses `lvgl_port_init()`, `lvgl_port_tick_inc()` and `lvgl_port_task_handler()`.

## Dependencies

BSP display, BSP touch, `Config/lv_conf.h` and LVGL source under `LVGL/lvgl`.

## Initialization Order

Call `bsp_init()` first, then `lvgl_port_init()`, then create screens and periodically call `lvgl_port_task_handler()`.

