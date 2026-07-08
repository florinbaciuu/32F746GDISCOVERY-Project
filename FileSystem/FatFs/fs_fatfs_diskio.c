#include "diskio.h"

#include "bsp_sdcard.h"

#include <stddef.h>

DSTATUS disk_initialize(BYTE pdrv)
{
    if (pdrv != 0U) {
        return STA_NOINIT;
    }

    return (bsp_sdcard_init() == APP_OK) ? 0U : STA_NOINIT;
}

DSTATUS disk_status(BYTE pdrv)
{
    if (pdrv != 0U) {
        return STA_NOINIT;
    }

    return bsp_sdcard_is_present() ? 0U : STA_NODISK;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
    if ((pdrv != 0U) || (buff == NULL) || (count == 0U)) {
        return RES_PARERR;
    }

    return (bsp_sdcard_read_blocks((uint32_t)sector, buff, count) == APP_OK) ? RES_OK : RES_ERROR;
}

DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
    if ((pdrv != 0U) || (buff == NULL) || (count == 0U)) {
        return RES_PARERR;
    }

    return (bsp_sdcard_write_blocks((uint32_t)sector, buff, count) == APP_OK) ? RES_OK : RES_ERROR;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff)
{
    bsp_sdcard_info_t info;

    if (pdrv != 0U) {
        return RES_PARERR;
    }

    switch (cmd) {
    case CTRL_SYNC:
        return RES_OK;

    case GET_SECTOR_COUNT:
        if ((buff == NULL) || (bsp_sdcard_get_info(&info) != APP_OK)) {
            return RES_ERROR;
        }
        *(DWORD *)buff = info.block_count;
        return RES_OK;

    case GET_SECTOR_SIZE:
        if ((buff == NULL) || (bsp_sdcard_get_info(&info) != APP_OK)) {
            return RES_ERROR;
        }
        *(WORD *)buff = (WORD)info.block_size_bytes;
        return RES_OK;

    case GET_BLOCK_SIZE:
        if (buff == NULL) {
            return RES_PARERR;
        }
        *(DWORD *)buff = 1U;
        return RES_OK;

    default:
        return RES_PARERR;
    }
}

DWORD get_fattime(void)
{
    return 0U;
}
