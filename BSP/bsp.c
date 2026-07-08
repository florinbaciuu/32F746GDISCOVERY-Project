#include "bsp.h"

#include "app_log.h"

#if __has_include("stm32f7xx_hal.h")
#include "stm32f7xx_hal.h"
#endif

#if __has_include("FreeRTOS.h") && __has_include("task.h")
#include "FreeRTOS.h"
#include "task.h"
#define BSP_FREERTOS_AVAILABLE 1
#else
#define BSP_FREERTOS_AVAILABLE 0
#endif

static void bsp_log_writer(const char *data, size_t length);
static uint32_t bsp_log_tick_reader(void);

app_status_t bsp_init(void)
{
    app_status_t status = bsp_serial_init();
    if (status != APP_OK) {
        return status;
    }

    app_log_init(bsp_log_writer, bsp_log_tick_reader);
    app_log_set_level(APP_LOG_LEVEL_DEBUG);
    APP_LOG_INFO("bsp", "serial ready: USART1 115200 8N1");

    status = bsp_io_init();
    if (status != APP_OK) {
        APP_LOG_ERROR("bsp", "io init failed");
        return status;
    }
    APP_LOG_INFO("bsp", "io ready");

    status = bsp_display_init();
    if (status != APP_OK) {
        APP_LOG_ERROR("bsp", "display init failed");
        return status;
    }
    APP_LOG_INFO("bsp", "display ready");

    status = bsp_touch_init();
    if (status != APP_OK) {
        APP_LOG_ERROR("bsp", "touch init failed");
        return status;
    }
    APP_LOG_INFO("bsp", "touch ready");

    return APP_OK;
}

uint32_t bsp_tick_get(void)
{
#if __has_include("stm32f7xx_hal.h")
    return HAL_GetTick();
#else
    return 0U;
#endif
}

void bsp_delay_ms(uint32_t delay_ms)
{
#if BSP_FREERTOS_AVAILABLE
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        vTaskDelay(pdMS_TO_TICKS(delay_ms));
        return;
    }
#endif

#if __has_include("stm32f7xx_hal.h")
    HAL_Delay(delay_ms);
#else
    (void)delay_ms;
#endif
}

static void bsp_log_writer(const char *data, size_t length)
{
    (void)bsp_serial_write(data, length);
}

static uint32_t bsp_log_tick_reader(void)
{
    return bsp_tick_get();
}
