#ifndef BSP_QSPI_H
#define BSP_QSPI_H

#include <stddef.h>
#include <stdint.h>

#include "app_error.h"

typedef struct {
    uint32_t flash_size_bytes;
    uint32_t sector_size_bytes;
    uint32_t sector_count;
    uint32_t page_size_bytes;
    uint32_t page_count;
} bsp_qspi_info_t;

app_status_t bsp_qspi_init(void);
app_status_t bsp_qspi_get_info(bsp_qspi_info_t *info);
app_status_t bsp_qspi_read(uint32_t address, void *data, size_t length);
app_status_t bsp_qspi_write(uint32_t address, const void *data, size_t length);
app_status_t bsp_qspi_erase_block(uint32_t address);
app_status_t bsp_qspi_get_status(void);
app_status_t bsp_qspi_enable_memory_mapped(void);

#endif
