# LVGL Port

## Purpose

Adapts LVGL display flushing and touch input to the framework BSP.

## Public API

`lvgl_port_init()`, `lvgl_port_tick_inc()` and `lvgl_port_task_handler()`.

## Dependencies

LVGL v9, BSP display and BSP touch.

## Initialization Order

Initialize after BSP display and touch are ready. Tick increment should be called from the system tick path at `BOARD_LVGL_TICK_PERIOD_MS`.

