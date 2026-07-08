# BSP Storage

## Purpose

Owns board-specific microSD presence and low-level storage initialization.

## Public API

`bsp_sdcard_init()` and `bsp_sdcard_is_present()`.

## Dependencies

Generated SDMMC, GPIO, DMA and optional FatFs disk I/O glue.

## Initialization Order

Initialize after generated SDMMC/GPIO setup and before mounting higher-level filesystems.

