#include "bsp_audio.h"

#include "board_config.h"

static bsp_audio_config_t active_config = {
    .sample_rate_hz = BOARD_AUDIO_SAMPLE_RATE_HZ,
    .format = BSP_AUDIO_FORMAT_PCM16_STEREO,
};

app_status_t bsp_audio_init(const bsp_audio_config_t *config)
{
    if (config != NULL) {
        active_config = *config;
    }

    return APP_OK;
}

app_status_t bsp_audio_play(const void *samples, size_t sample_count)
{
    if ((samples == NULL) || (sample_count == 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    (void)active_config;
    return APP_ERROR_UNSUPPORTED;
}

void bsp_audio_stop(void)
{
}

