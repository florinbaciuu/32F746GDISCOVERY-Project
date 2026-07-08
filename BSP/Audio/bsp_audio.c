#include "bsp_audio.h"

#include "board_config.h"

#if __has_include("stm32746g_discovery_audio.h")
#include "stm32746g_discovery_audio.h"
#define BSP_ST_AUDIO_AVAILABLE 1
#else
#define BSP_ST_AUDIO_AVAILABLE 0
#endif

#if __has_include("wm8994.h")
#include "wm8994.h"
#endif

#ifndef BOARD_AUDIO_DEFAULT_VOLUME
#define BOARD_AUDIO_DEFAULT_VOLUME 70U
#endif

static bsp_audio_config_t active_config = {
    .sample_rate_hz = BOARD_AUDIO_SAMPLE_RATE_HZ,
    .format = BSP_AUDIO_FORMAT_PCM16_STEREO,
    .output = BSP_AUDIO_OUTPUT_BOTH,
    .volume_percent = BOARD_AUDIO_DEFAULT_VOLUME,
};

static bsp_audio_state_t audio_state = BSP_AUDIO_STATE_UNINITIALIZED;

#if BSP_ST_AUDIO_AVAILABLE
static bool st_audio_ready = false;

static bool config_is_supported(const bsp_audio_config_t *config);
static uint16_t output_device_from_config(bsp_audio_output_t output);
static uint32_t bytes_for_sample_count(size_t sample_count);
static void clean_audio_buffer_for_dma(const void *buffer, uint32_t byte_count);
static app_status_t audio_result_to_status(uint8_t result);
#endif

app_status_t bsp_audio_init(const bsp_audio_config_t *config)
{
    if (config != NULL) {
        active_config = *config;
    }

#if BSP_ST_AUDIO_AVAILABLE
    if (st_audio_ready) {
        (void)BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
        st_audio_ready = false;
    }

    if (!config_is_supported(&active_config)) {
        audio_state = BSP_AUDIO_STATE_ERROR;
        return APP_ERROR_UNSUPPORTED;
    }

    if (active_config.volume_percent > 100U) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    uint8_t result = BSP_AUDIO_OUT_Init(output_device_from_config(active_config.output),
                                        active_config.volume_percent,
                                        active_config.sample_rate_hz);
    if (result != AUDIO_OK) {
        audio_state = BSP_AUDIO_STATE_ERROR;
        return APP_ERROR;
    }

    st_audio_ready = true;
    audio_state = BSP_AUDIO_STATE_READY;
    return APP_OK;
#else
    audio_state = BSP_AUDIO_STATE_ERROR;
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_audio_play(const void *samples, size_t sample_count)
{
    if ((samples == NULL) || (sample_count == 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if BSP_ST_AUDIO_AVAILABLE
    if (!st_audio_ready) {
        app_status_t init_status = bsp_audio_init(NULL);
        if (init_status != APP_OK) {
            return init_status;
        }
    }

    if (!config_is_supported(&active_config)) {
        return APP_ERROR_UNSUPPORTED;
    }

    uint32_t byte_count = bytes_for_sample_count(sample_count);
    if (byte_count == 0U) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    clean_audio_buffer_for_dma(samples, byte_count);

    uint8_t result = BSP_AUDIO_OUT_Play((uint16_t *)samples, byte_count);
    if (result != AUDIO_OK) {
        audio_state = BSP_AUDIO_STATE_ERROR;
        return APP_ERROR;
    }

    audio_state = BSP_AUDIO_STATE_PLAYING;
    return APP_OK;
#else
    (void)sample_count;
    audio_state = BSP_AUDIO_STATE_ERROR;
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_audio_pause(void)
{
#if BSP_ST_AUDIO_AVAILABLE
    if (audio_state != BSP_AUDIO_STATE_PLAYING) {
        return APP_ERROR_NOT_READY;
    }

    app_status_t status = audio_result_to_status(BSP_AUDIO_OUT_Pause());
    if (status == APP_OK) {
        audio_state = BSP_AUDIO_STATE_PAUSED;
    } else {
        audio_state = BSP_AUDIO_STATE_ERROR;
    }

    return status;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_audio_resume(void)
{
#if BSP_ST_AUDIO_AVAILABLE
    if (audio_state != BSP_AUDIO_STATE_PAUSED) {
        return APP_ERROR_NOT_READY;
    }

    app_status_t status = audio_result_to_status(BSP_AUDIO_OUT_Resume());
    if (status == APP_OK) {
        audio_state = BSP_AUDIO_STATE_PLAYING;
    } else {
        audio_state = BSP_AUDIO_STATE_ERROR;
    }

    return status;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_audio_set_volume(uint8_t volume_percent)
{
    if (volume_percent > 100U) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    active_config.volume_percent = volume_percent;

#if BSP_ST_AUDIO_AVAILABLE
    if (!st_audio_ready) {
        return APP_OK;
    }

    app_status_t status = audio_result_to_status(BSP_AUDIO_OUT_SetVolume(volume_percent));
    if (status != APP_OK) {
        audio_state = BSP_AUDIO_STATE_ERROR;
    }

    return status;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_audio_set_mute(bool muted)
{
#if BSP_ST_AUDIO_AVAILABLE
    if (!st_audio_ready) {
        return APP_ERROR_NOT_READY;
    }

    app_status_t status = audio_result_to_status(BSP_AUDIO_OUT_SetMute(muted ? AUDIO_MUTE_ON : AUDIO_MUTE_OFF));
    if (status != APP_OK) {
        audio_state = BSP_AUDIO_STATE_ERROR;
    }

    return status;
#else
    (void)muted;
    return APP_ERROR_UNSUPPORTED;
#endif
}

void bsp_audio_stop(void)
{
#if BSP_ST_AUDIO_AVAILABLE
    if (st_audio_ready) {
        (void)BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
        st_audio_ready = false;
        audio_state = BSP_AUDIO_STATE_READY;
    }
#endif
}

bsp_audio_state_t bsp_audio_get_state(void)
{
    return audio_state;
}

#if BSP_ST_AUDIO_AVAILABLE
static bool config_is_supported(const bsp_audio_config_t *config)
{
    if (config->format != BSP_AUDIO_FORMAT_PCM16_STEREO) {
        return false;
    }

    switch (config->sample_rate_hz) {
    case AUDIO_FREQUENCY_8K:
    case AUDIO_FREQUENCY_11K:
    case AUDIO_FREQUENCY_16K:
    case AUDIO_FREQUENCY_22K:
    case AUDIO_FREQUENCY_32K:
    case AUDIO_FREQUENCY_44K:
    case AUDIO_FREQUENCY_48K:
    case AUDIO_FREQUENCY_96K:
        return true;
    default:
        return false;
    }
}

static uint16_t output_device_from_config(bsp_audio_output_t output)
{
    switch (output) {
    case BSP_AUDIO_OUTPUT_SPEAKER:
        return OUTPUT_DEVICE_SPEAKER;
    case BSP_AUDIO_OUTPUT_HEADPHONE:
        return OUTPUT_DEVICE_HEADPHONE;
    case BSP_AUDIO_OUTPUT_BOTH:
    default:
        return OUTPUT_DEVICE_BOTH;
    }
}

static uint32_t bytes_for_sample_count(size_t sample_count)
{
    if (sample_count > (UINT32_MAX / sizeof(int16_t))) {
        return 0U;
    }

    return (uint32_t)(sample_count * sizeof(int16_t));
}

static void clean_audio_buffer_for_dma(const void *buffer, uint32_t byte_count)
{
#if defined(__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    uintptr_t start = (uintptr_t)buffer & ~(uintptr_t)31U;
    uintptr_t end = ((uintptr_t)buffer + byte_count + 31U) & ~(uintptr_t)31U;

    SCB_CleanDCache_by_Addr((uint32_t *)start, (int32_t)(end - start));
#else
    (void)buffer;
    (void)byte_count;
#endif
}

static app_status_t audio_result_to_status(uint8_t result)
{
    return (result == AUDIO_OK) ? APP_OK : APP_ERROR;
}
#endif
