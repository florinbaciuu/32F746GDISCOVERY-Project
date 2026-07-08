#ifndef BSP_SERIAL_H
#define BSP_SERIAL_H

#include <stddef.h>

#include "app_error.h"

app_status_t bsp_serial_init(void);
app_status_t bsp_serial_write(const char *data, size_t length);
app_status_t bsp_serial_write_string(const char *message);

#endif
