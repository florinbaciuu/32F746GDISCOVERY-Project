#ifndef BSP_AUDIO_H
#define BSP_AUDIO_H

#include <stddef.h>
#include <stdint.h>

#include "app_error.h"

typedef enum {
    BSP_AUDIO_FORMAT_PCM16_MONO = 0,
    BSP_AUDIO_FORMAT_PCM16_STEREO
} bsp_audio_format_t;

typedef struct {
    uint32_t sample_rate_hz;
    bsp_audio_format_t format;
} bsp_audio_config_t;

app_status_t bsp_audio_init(const bsp_audio_config_t *config);
app_status_t bsp_audio_play(const void *samples, size_t sample_count);
void bsp_audio_stop(void);

#endif

