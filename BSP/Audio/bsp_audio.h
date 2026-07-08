#ifndef BSP_AUDIO_H
#define BSP_AUDIO_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include "app_error.h"

typedef enum {
    BSP_AUDIO_FORMAT_PCM16_MONO = 0,
    BSP_AUDIO_FORMAT_PCM16_STEREO
} bsp_audio_format_t;

typedef enum {
    BSP_AUDIO_OUTPUT_SPEAKER = 0,
    BSP_AUDIO_OUTPUT_HEADPHONE,
    BSP_AUDIO_OUTPUT_BOTH
} bsp_audio_output_t;

typedef enum {
    BSP_AUDIO_STATE_UNINITIALIZED = 0,
    BSP_AUDIO_STATE_READY,
    BSP_AUDIO_STATE_PLAYING,
    BSP_AUDIO_STATE_PAUSED,
    BSP_AUDIO_STATE_ERROR
} bsp_audio_state_t;

typedef struct {
    uint32_t sample_rate_hz;
    bsp_audio_format_t format;
    bsp_audio_output_t output;
    uint8_t volume_percent;
} bsp_audio_config_t;

app_status_t bsp_audio_init(const bsp_audio_config_t *config);
app_status_t bsp_audio_play(const void *samples, size_t sample_count);
app_status_t bsp_audio_pause(void);
app_status_t bsp_audio_resume(void);
app_status_t bsp_audio_set_volume(uint8_t volume_percent);
app_status_t bsp_audio_set_mute(bool muted);
void bsp_audio_stop(void);
bsp_audio_state_t bsp_audio_get_state(void);

#endif
