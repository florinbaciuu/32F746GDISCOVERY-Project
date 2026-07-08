#ifndef BSP_SDCARD_H
#define BSP_SDCARD_H

#include <stdbool.h>
#include <stdint.h>

#include "app_error.h"

typedef struct {
    uint32_t block_count;
    uint32_t block_size_bytes;
} bsp_sdcard_info_t;

app_status_t bsp_sdcard_init(void);
bool bsp_sdcard_is_present(void);
app_status_t bsp_sdcard_get_info(bsp_sdcard_info_t *info);
app_status_t bsp_sdcard_read_blocks(uint32_t first_block, void *data, uint32_t block_count);
app_status_t bsp_sdcard_write_blocks(uint32_t first_block, const void *data, uint32_t block_count);

#endif
