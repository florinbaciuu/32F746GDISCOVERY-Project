#include "bsp_touch.h"

#include <stddef.h>

#include "board_config.h"

#if __has_include("stm32746g_discovery_ts.h")
#include "stm32746g_discovery_ts.h"
#define ST_DISCOVERY_TOUCH_AVAILABLE 1
#else
#define ST_DISCOVERY_TOUCH_AVAILABLE 0
#endif

app_status_t bsp_touch_init(void)
{
#if ST_DISCOVERY_TOUCH_AVAILABLE
    if (BSP_TS_Init(BOARD_DISPLAY_WIDTH, BOARD_DISPLAY_HEIGHT) != TS_OK) {
        return APP_ERROR;
    }
#endif

    return APP_OK;
}

app_status_t bsp_touch_read(bsp_touch_state_t *state)
{
    if (state == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if ST_DISCOVERY_TOUCH_AVAILABLE
    TS_StateTypeDef ts_state;
    if (BSP_TS_GetState(&ts_state) != TS_OK) {
        return APP_ERROR;
    }

    state->pressed = ts_state.touchDetected > 0U;
    state->x = state->pressed ? ts_state.touchX[0] : 0U;
    state->y = state->pressed ? ts_state.touchY[0] : 0U;
#else
    state->pressed = false;
    state->x = 0U;
    state->y = 0U;
#endif

    return APP_OK;
}
