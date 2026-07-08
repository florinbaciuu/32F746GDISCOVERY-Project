#ifndef BSP_IO_H
#define BSP_IO_H

#include <stdbool.h>

#include "app_error.h"

typedef enum {
    BSP_LED_GREEN = 0,
    BSP_LED_COUNT
} bsp_led_t;

typedef enum {
    BSP_BUTTON_USER = 0,
    BSP_BUTTON_COUNT
} bsp_button_t;

app_status_t bsp_io_init(void);
void bsp_led_set(bsp_led_t led, bool enabled);
bool bsp_button_read(bsp_button_t button);

#endif

