#ifndef BSP_H
#define BSP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "app_error.h"
#include "bsp_audio.h"
#include "bsp_display.h"
#include "bsp_diagnostics.h"
#include "bsp_io.h"
#include "bsp_qspi.h"
#include "bsp_serial.h"
#include "bsp_sdcard.h"
#include "bsp_touch.h"

app_status_t bsp_init(void);
uint32_t bsp_tick_get(void);
void bsp_delay_ms(uint32_t delay_ms);

#endif
