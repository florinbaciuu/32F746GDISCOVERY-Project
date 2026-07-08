#include "bsp_display.h"

#include <string.h>

#include "board_config.h"

#if __has_include("stm32f7xx_hal.h")
#include "stm32f7xx_hal.h"
#endif

#if __has_include("stm32746g_discovery_lcd.h")
#include "stm32746g_discovery_lcd.h"
#define ST_DISCOVERY_LCD_AVAILABLE 1
#else
#define ST_DISCOVERY_LCD_AVAILABLE 0
#endif

static bsp_display_config_t display_config = {
    .width = BOARD_DISPLAY_WIDTH,
    .height = BOARD_DISPLAY_HEIGHT,
    .pixel_format = BSP_DISPLAY_PIXEL_FORMAT_ARGB8888,
    .framebuffer_0 = (void *)BOARD_FRAMEBUFFER_0_ADDR,
    .framebuffer_1 = (void *)BOARD_FRAMEBUFFER_1_ADDR,
};

app_status_t bsp_display_init(void)
{
#if ST_DISCOVERY_LCD_AVAILABLE
    if (BSP_LCD_Init() != LCD_OK) {
        return APP_ERROR;
    }

    BSP_LCD_LayerDefaultInit(0U, (uint32_t)display_config.framebuffer_0);
    BSP_LCD_SelectLayer(0U);
    BSP_LCD_Clear(LCD_COLOR_BLUE);
    BSP_LCD_DisplayOn();
#endif

    return APP_OK;
}

const bsp_display_config_t *bsp_display_get_config(void)
{
    return &display_config;
}

app_status_t bsp_display_flush(const bsp_display_area_t *area, const void *pixels)
{
    if ((area == NULL) || (pixels == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    if ((area->x2 < area->x1) || (area->y2 < area->y1) ||
        (area->x2 >= display_config.width) || (area->y2 >= display_config.height)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    const uint32_t width = (uint32_t)area->x2 - (uint32_t)area->x1 + 1U;
    const uint32_t height = (uint32_t)area->y2 - (uint32_t)area->y1 + 1U;
    const uint32_t bytes_per_pixel = BOARD_DISPLAY_BPP;
    uint8_t *dst = (uint8_t *)display_config.framebuffer_0 +
                   (((uint32_t)area->y1 * display_config.width) + area->x1) * bytes_per_pixel;
    const uint8_t *src = (const uint8_t *)pixels;

    for (uint32_t row = 0; row < height; row++) {
        memcpy(dst, src, width * bytes_per_pixel);
        dst += (uint32_t)display_config.width * bytes_per_pixel;
        src += width * bytes_per_pixel;
    }

#if __has_include("stm32f7xx_hal.h")
    SCB_CleanDCache_by_Addr((uint32_t *)display_config.framebuffer_0, (int32_t)BOARD_FRAMEBUFFER_SIZE);
#endif

    return APP_OK;
}

void bsp_display_set_backlight(bool enabled)
{
#if ST_DISCOVERY_LCD_AVAILABLE
    if (enabled) {
        BSP_LCD_DisplayOn();
    } else {
        BSP_LCD_DisplayOff();
    }
#else
    (void)enabled;
#endif
}
