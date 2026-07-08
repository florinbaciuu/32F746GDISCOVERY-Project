#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include "app_error.h"

app_status_t lvgl_port_init(void);
void lvgl_port_tick_inc(void);
void lvgl_port_task_handler(void);

#endif

