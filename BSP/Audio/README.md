# BSP Audio

## Purpose

Provides a stable audio playback boundary for the external codec and I2S/SAI/DMA plumbing.

## Public API

`bsp_audio_init()`, `bsp_audio_play()` and `bsp_audio_stop()`.

## Dependencies

Generated audio peripheral initialization, DMA and codec component driver.

## Initialization Order

Audio can be initialized lazily by a media service after core BSP initialization.

