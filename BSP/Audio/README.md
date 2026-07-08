# BSP Audio

## Purpose

Provides a stable audio playback boundary for the external codec and I2S/SAI/DMA plumbing.

## Public API

`bsp_audio_init()`, `bsp_audio_play()`, `bsp_audio_pause()`, `bsp_audio_resume()`,
`bsp_audio_set_volume()`, `bsp_audio_set_mute()`, `bsp_audio_stop()` and
`bsp_audio_get_state()`.

Playback currently supports PCM16 stereo. Pass interleaved signed 16-bit samples
to `bsp_audio_play()` in left/right order. `sample_count` is the number of
`int16_t` values in the buffer, not the number of stereo frames.

The STM32F746G-DISCO audio BSP uses SAI2, DMA2 Stream 4 and the WM8994 codec.
DMA is configured in circular mode by the ST board driver, so a buffer passed to
`bsp_audio_play()` loops until `bsp_audio_stop()` is called.

## Dependencies

ST board audio driver, WM8994 component driver, SAI, DMA, I2C and codec control
over the board audio I2C bus.

## Initialization Order

Audio can be initialized lazily by a media service after core BSP initialization.
