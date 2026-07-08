#include "bsp_io.h"

#if __has_include("stm32746g_discovery.h")
#include "stm32746g_discovery.h"
#define ST_DISCOVERY_IO_AVAILABLE 1
#else
#define ST_DISCOVERY_IO_AVAILABLE 0
#endif

app_status_t bsp_io_init(void)
{
#if ST_DISCOVERY_IO_AVAILABLE
    BSP_LED_Init(LED_GREEN);
    BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_GPIO);
#endif

    return APP_OK;
}

void bsp_led_set(bsp_led_t led, bool enabled)
{
    if (led >= BSP_LED_COUNT) {
        return;
    }

#if ST_DISCOVERY_IO_AVAILABLE
    if (enabled) {
        BSP_LED_On(LED_GREEN);
    } else {
        BSP_LED_Off(LED_GREEN);
    }
#else
    (void)enabled;
#endif
}

bool bsp_button_read(bsp_button_t button)
{
    if (button >= BSP_BUTTON_COUNT) {
        return false;
    }

#if ST_DISCOVERY_IO_AVAILABLE
    return BSP_PB_GetState(BUTTON_KEY) != 0U;
#endif

    return false;
}
