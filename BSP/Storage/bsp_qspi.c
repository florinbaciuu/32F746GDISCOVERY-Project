#include "bsp_qspi.h"

#include <stdbool.h>

#if __has_include("stm32746g_discovery_qspi.h")
#include "stm32746g_discovery_qspi.h"
#define ST_DISCOVERY_QSPI_AVAILABLE 1
#else
#define ST_DISCOVERY_QSPI_AVAILABLE 0
#endif

static bool qspi_initialized;

app_status_t bsp_qspi_init(void)
{
#if ST_DISCOVERY_QSPI_AVAILABLE
    if (qspi_initialized) {
        return APP_OK;
    }

    if (BSP_QSPI_Init() != QSPI_OK) {
        return APP_ERROR;
    }

    qspi_initialized = true;
    return APP_OK;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_qspi_get_info(bsp_qspi_info_t *info)
{
    if (info == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if ST_DISCOVERY_QSPI_AVAILABLE
    QSPI_Info st_info = {0};

    if (bsp_qspi_init() != APP_OK) {
        return APP_ERROR;
    }

    if (BSP_QSPI_GetInfo(&st_info) != QSPI_OK) {
        return APP_ERROR;
    }

    info->flash_size_bytes = st_info.FlashSize;
    info->sector_size_bytes = st_info.EraseSectorSize;
    info->sector_count = st_info.EraseSectorsNumber;
    info->page_size_bytes = st_info.ProgPageSize;
    info->page_count = st_info.ProgPagesNumber;
    return APP_OK;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_qspi_read(uint32_t address, void *data, size_t length)
{
    if ((data == NULL) || (length == 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if ST_DISCOVERY_QSPI_AVAILABLE
    if (bsp_qspi_init() != APP_OK) {
        return APP_ERROR;
    }

    if (BSP_QSPI_Read((uint8_t *)data, address, (uint32_t)length) != QSPI_OK) {
        return APP_ERROR;
    }

    return APP_OK;
#else
    (void)address;
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_qspi_write(uint32_t address, const void *data, size_t length)
{
    if ((data == NULL) || (length == 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if ST_DISCOVERY_QSPI_AVAILABLE
    if (bsp_qspi_init() != APP_OK) {
        return APP_ERROR;
    }

    if (BSP_QSPI_Write((uint8_t *)data, address, (uint32_t)length) != QSPI_OK) {
        return APP_ERROR;
    }

    return APP_OK;
#else
    (void)address;
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_qspi_erase_block(uint32_t address)
{
#if ST_DISCOVERY_QSPI_AVAILABLE
    if (bsp_qspi_init() != APP_OK) {
        return APP_ERROR;
    }

    if (BSP_QSPI_Erase_Block(address) != QSPI_OK) {
        return APP_ERROR;
    }

    return APP_OK;
#else
    (void)address;
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_qspi_get_status(void)
{
#if ST_DISCOVERY_QSPI_AVAILABLE
    if (bsp_qspi_init() != APP_OK) {
        return APP_ERROR;
    }

    return (BSP_QSPI_GetStatus() == QSPI_OK) ? APP_OK : APP_ERROR;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_qspi_enable_memory_mapped(void)
{
#if ST_DISCOVERY_QSPI_AVAILABLE
    if (bsp_qspi_init() != APP_OK) {
        return APP_ERROR;
    }

    if (BSP_QSPI_EnableMemoryMappedMode() != QSPI_OK) {
        return APP_ERROR;
    }

    return APP_OK;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}
