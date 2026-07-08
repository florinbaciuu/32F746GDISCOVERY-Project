#include "bsp_sdcard.h"

#if __has_include("stm32746g_discovery_sd.h")
#include "stm32746g_discovery_sd.h"
#define ST_DISCOVERY_SD_AVAILABLE 1
#else
#define ST_DISCOVERY_SD_AVAILABLE 0
#endif

#define BSP_SDCARD_IO_TIMEOUT_MS 5000U

app_status_t bsp_sdcard_init(void)
{
#if ST_DISCOVERY_SD_AVAILABLE
    if (!bsp_sdcard_is_present()) {
        return APP_ERROR_NOT_READY;
    }

    if (BSP_SD_Init() != MSD_OK) {
        return APP_ERROR;
    }
#endif

    return APP_OK;
}

bool bsp_sdcard_is_present(void)
{
#if ST_DISCOVERY_SD_AVAILABLE
    return BSP_SD_IsDetected() == SD_PRESENT;
#else
    return false;
#endif
}

app_status_t bsp_sdcard_get_info(bsp_sdcard_info_t *info)
{
    if (info == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if ST_DISCOVERY_SD_AVAILABLE
    HAL_SD_CardInfoTypeDef card_info;

    if (bsp_sdcard_init() != APP_OK) {
        return APP_ERROR_NOT_READY;
    }

    BSP_SD_GetCardInfo(&card_info);
    info->block_count = card_info.LogBlockNbr;
    info->block_size_bytes = card_info.LogBlockSize;
    return APP_OK;
#else
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_sdcard_read_blocks(uint32_t first_block, void *data, uint32_t block_count)
{
    if ((data == NULL) || (block_count == 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if ST_DISCOVERY_SD_AVAILABLE
    if (bsp_sdcard_init() != APP_OK) {
        return APP_ERROR_NOT_READY;
    }

    if (BSP_SD_ReadBlocks((uint32_t *)data, first_block, block_count, BSP_SDCARD_IO_TIMEOUT_MS) != MSD_OK) {
        return APP_ERROR;
    }

    return APP_OK;
#else
    (void)first_block;
    return APP_ERROR_UNSUPPORTED;
#endif
}

app_status_t bsp_sdcard_write_blocks(uint32_t first_block, const void *data, uint32_t block_count)
{
    if ((data == NULL) || (block_count == 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if ST_DISCOVERY_SD_AVAILABLE
    if (bsp_sdcard_init() != APP_OK) {
        return APP_ERROR_NOT_READY;
    }

    if (BSP_SD_WriteBlocks((uint32_t *)data, first_block, block_count, BSP_SDCARD_IO_TIMEOUT_MS) != MSD_OK) {
        return APP_ERROR;
    }

    return APP_OK;
#else
    (void)first_block;
    return APP_ERROR_UNSUPPORTED;
#endif
}
