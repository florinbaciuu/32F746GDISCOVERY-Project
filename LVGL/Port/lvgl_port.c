#include "lvgl_port.h"

#if __has_include("lvgl.h")
#include "lvgl.h"
#define LVGL_AVAILABLE 1
#else
#define LVGL_AVAILABLE 0
typedef struct _lv_display_t lv_display_t;
typedef struct _lv_indev_t lv_indev_t;
#endif

#include "board_config.h"
#include "bsp_display.h"
#include "bsp_touch.h"

#if LVGL_AVAILABLE
static void lvgl_display_flush_cb(lv_display_t *display, const lv_area_t *area, uint8_t *px_map);
static void lvgl_touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data);

static lv_display_t *display;
static lv_indev_t *touch_indev;

static void *const draw_buffer_0 = (void *)BOARD_LVGL_DRAW_BUFFER_0_ADDR;
static void *const draw_buffer_1 = (void *)BOARD_LVGL_DRAW_BUFFER_1_ADDR;
#endif

app_status_t lvgl_port_init(void)
{
#if LVGL_AVAILABLE
    lv_init();

    const bsp_display_config_t *config = bsp_display_get_config();
    display = lv_display_create(config->width, config->height);
    if (display == NULL) {
        return APP_ERROR;
    }

    lv_display_set_color_format(display, LV_COLOR_FORMAT_XRGB8888);
    lv_display_set_flush_cb(display, lvgl_display_flush_cb);
    lv_display_set_buffers(display,
                           draw_buffer_0,
                           draw_buffer_1,
                           BOARD_LVGL_DRAW_BUFFER_SIZE,
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    touch_indev = lv_indev_create();
    if (touch_indev == NULL) {
        return APP_ERROR;
    }

    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch_indev, lvgl_touch_read_cb);
#endif

    return APP_OK;
}

void lvgl_port_tick_inc(void)
{
#if LVGL_AVAILABLE
    lv_tick_inc(BOARD_LVGL_TICK_PERIOD_MS);
#endif
}

void lvgl_port_task_handler(void)
{
#if LVGL_AVAILABLE
    lv_timer_handler();
#endif
}

#if LVGL_AVAILABLE
static void lvgl_display_flush_cb(lv_display_t *display_handle, const lv_area_t *area, uint8_t *px_map)
{
    const bsp_display_area_t bsp_area = {
        .x1 = (uint16_t)area->x1,
        .y1 = (uint16_t)area->y1,
        .x2 = (uint16_t)area->x2,
        .y2 = (uint16_t)area->y2,
    };

    (void)bsp_display_flush(&bsp_area, px_map);
    lv_display_flush_ready(display_handle);
}

static void lvgl_touch_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    (void)indev;

    bsp_touch_state_t state;
    if (bsp_touch_read(&state) != APP_OK) {
        data->state = LV_INDEV_STATE_RELEASED;
        return;
    }

    data->point.x = state.x;
    data->point.y = state.y;
    data->state = state.pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}
#endif
