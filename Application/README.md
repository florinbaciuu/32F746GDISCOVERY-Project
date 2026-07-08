# Application

## Purpose

Top-level application shell, reusable services and screens.

## Public API

`app_init()` and `app_run()` are the only functions generated `main.c` should call.

## Dependencies

BSP, LVGL port and application services.

## Initialization Order

Called after generated HAL peripheral initialization. `app_init()` initializes BSP and LVGL, then creates the initial screen. `app_run()` owns the foreground loop.

