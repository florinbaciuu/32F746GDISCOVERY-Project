#ifndef BSP_TOUCH_H
#define BSP_TOUCH_H

#include <stdbool.h>
#include <stdint.h>

#include "app_error.h"

typedef struct {
    bool pressed;
    uint16_t x;
    uint16_t y;
} bsp_touch_state_t;

app_status_t bsp_touch_init(void);
app_status_t bsp_touch_read(bsp_touch_state_t *state);

#endif

