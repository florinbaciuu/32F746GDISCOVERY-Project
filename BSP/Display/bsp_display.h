#ifndef BSP_DISPLAY_H
#define BSP_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

#include "app_error.h"

typedef enum {
    BSP_DISPLAY_PIXEL_FORMAT_ARGB8888 = 0,
    BSP_DISPLAY_PIXEL_FORMAT_RGB565
} bsp_display_pixel_format_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    bsp_display_pixel_format_t pixel_format;
    void *framebuffer_0;
    void *framebuffer_1;
} bsp_display_config_t;

typedef struct {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
} bsp_display_area_t;

app_status_t bsp_display_init(void);
const bsp_display_config_t *bsp_display_get_config(void);
app_status_t bsp_display_flush(const bsp_display_area_t *area, const void *pixels);
void bsp_display_set_backlight(bool enabled);

#endif

