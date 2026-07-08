# Drivers

## Purpose

CubeMX-managed CMSIS, STM32 HAL and optional ST component drivers.

## Public API

HAL APIs are considered private to BSP and low-level driver modules. Application code should not include HAL headers directly.

## Dependencies

Generated CubeMX project configuration.

## Initialization Order

Drivers are initialized through generated `MX_*_Init()` functions before framework modules start.

