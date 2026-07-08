#include "fs.h"

#include "bsp_sdcard.h"

#include <stdio.h>
#include <string.h>

#define FATFS_SD_PREFIX "0:"

static app_status_t map_fatfs_result(FRESULT result);
static BYTE fatfs_open_mode(uint32_t flags);
static app_status_t build_fatfs_path(const char *path, char *buffer, size_t buffer_size);

static FATFS sd_fs;
static bool sd_mounted;

app_status_t fs_init(void)
{
    memset(&sd_fs, 0, sizeof(sd_fs));
    sd_mounted = false;
    return APP_OK;
}

app_status_t fs_mount(fs_volume_t volume, bool format_if_needed)
{
    if (volume != FS_VOLUME_SD) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    if (sd_mounted) {
        return APP_OK;
    }

    if (!bsp_sdcard_is_present()) {
        return APP_ERROR_NOT_READY;
    }

    FRESULT result = f_mount(&sd_fs, FATFS_SD_PREFIX, 1);
    if ((result != FR_OK) && format_if_needed) {
        uint8_t work[FF_MAX_SS];
        result = f_mkfs(FATFS_SD_PREFIX, NULL, work, sizeof(work));
        if (result == FR_OK) {
            result = f_mount(&sd_fs, FATFS_SD_PREFIX, 1);
        }
    }

    sd_mounted = (result == FR_OK);
    return map_fatfs_result(result);
}

app_status_t fs_unmount(fs_volume_t volume)
{
    if (volume != FS_VOLUME_SD) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    if (!sd_mounted) {
        return APP_OK;
    }

    app_status_t status = map_fatfs_result(f_mount(NULL, FATFS_SD_PREFIX, 0));
    sd_mounted = false;
    return status;
}

app_status_t fs_format(fs_volume_t volume)
{
    if (volume != FS_VOLUME_SD) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    uint8_t work[FF_MAX_SS];
    if (sd_mounted) {
        (void)fs_unmount(volume);
    }

    return map_fatfs_result(f_mkfs(FATFS_SD_PREFIX, NULL, work, sizeof(work)));
}

app_status_t fs_get_info(fs_volume_t volume, fs_volume_info_t *info)
{
    if ((volume != FS_VOLUME_SD) || (info == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    info->mounted = sd_mounted;
    info->total_bytes = 0U;
    info->free_bytes = 0U;

    if (!sd_mounted) {
        return APP_ERROR_NOT_READY;
    }

    FATFS *fs;
    DWORD free_clusters;
    FRESULT result = f_getfree(FATFS_SD_PREFIX, &free_clusters, &fs);
    if (result != FR_OK) {
        return map_fatfs_result(result);
    }

    info->total_bytes = (uint32_t)((fs->n_fatent - 2U) * fs->csize * FF_MAX_SS);
    info->free_bytes = (uint32_t)(free_clusters * fs->csize * FF_MAX_SS);
    return APP_OK;
}

app_status_t fs_file_open(fs_volume_t volume, const char *path, uint32_t flags, fs_file_t *file)
{
    if ((volume != FS_VOLUME_SD) || (path == NULL) || (file == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    if (!sd_mounted) {
        return APP_ERROR_NOT_READY;
    }

    char fatfs_path[160];
    app_status_t status = build_fatfs_path(path, fatfs_path, sizeof(fatfs_path));
    if (status != APP_OK) {
        return status;
    }

    memset(file, 0, sizeof(*file));
    file->volume = volume;
    return map_fatfs_result(f_open(&file->native, fatfs_path, fatfs_open_mode(flags)));
}

app_status_t fs_file_close(fs_file_t *file)
{
    if (file == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    return map_fatfs_result(f_close(&file->native));
}

app_status_t fs_file_read(fs_file_t *file, void *buffer, size_t size, size_t *bytes_read)
{
    if ((file == NULL) || (buffer == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    UINT read_count = 0U;
    app_status_t status = map_fatfs_result(f_read(&file->native, buffer, (UINT)size, &read_count));
    if (bytes_read != NULL) {
        *bytes_read = read_count;
    }

    return status;
}

app_status_t fs_file_write(fs_file_t *file, const void *buffer, size_t size, size_t *bytes_written)
{
    if ((file == NULL) || (buffer == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    UINT write_count = 0U;
    app_status_t status = map_fatfs_result(f_write(&file->native, buffer, (UINT)size, &write_count));
    if (bytes_written != NULL) {
        *bytes_written = write_count;
    }

    return status;
}

app_status_t fs_file_seek(fs_file_t *file, int32_t offset, fs_seek_origin_t origin)
{
    if (file == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    FSIZE_t base = 0U;
    if (origin == FS_SEEK_CUR) {
        base = f_tell(&file->native);
    } else if (origin == FS_SEEK_END) {
        base = f_size(&file->native);
    }

    return map_fatfs_result(f_lseek(&file->native, (FSIZE_t)((int32_t)base + offset)));
}

app_status_t fs_file_tell(fs_file_t *file, uint32_t *position)
{
    if ((file == NULL) || (position == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    *position = (uint32_t)f_tell(&file->native);
    return APP_OK;
}

app_status_t fs_file_sync(fs_file_t *file)
{
    if (file == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    return map_fatfs_result(f_sync(&file->native));
}

app_status_t fs_mkdir(fs_volume_t volume, const char *path)
{
    if ((volume != FS_VOLUME_SD) || (path == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    char fatfs_path[160];
    app_status_t status = build_fatfs_path(path, fatfs_path, sizeof(fatfs_path));
    return (status == APP_OK) ? map_fatfs_result(f_mkdir(fatfs_path)) : status;
}

app_status_t fs_remove(fs_volume_t volume, const char *path)
{
    if ((volume != FS_VOLUME_SD) || (path == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    char fatfs_path[160];
    app_status_t status = build_fatfs_path(path, fatfs_path, sizeof(fatfs_path));
    return (status == APP_OK) ? map_fatfs_result(f_unlink(fatfs_path)) : status;
}

app_status_t fs_rename(fs_volume_t volume, const char *old_path, const char *new_path)
{
    if ((volume != FS_VOLUME_SD) || (old_path == NULL) || (new_path == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    char old_fatfs_path[160];
    char new_fatfs_path[160];
    app_status_t status = build_fatfs_path(old_path, old_fatfs_path, sizeof(old_fatfs_path));
    if (status != APP_OK) {
        return status;
    }

    status = build_fatfs_path(new_path, new_fatfs_path, sizeof(new_fatfs_path));
    return (status == APP_OK) ? map_fatfs_result(f_rename(old_fatfs_path, new_fatfs_path)) : status;
}

app_status_t fs_dir_open(fs_volume_t volume, const char *path, fs_dir_t *dir)
{
    if ((volume != FS_VOLUME_SD) || (path == NULL) || (dir == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    char fatfs_path[160];
    app_status_t status = build_fatfs_path(path, fatfs_path, sizeof(fatfs_path));
    if (status != APP_OK) {
        return status;
    }

    memset(dir, 0, sizeof(*dir));
    dir->volume = volume;
    return map_fatfs_result(f_opendir(&dir->native, fatfs_path));
}

app_status_t fs_dir_read(fs_dir_t *dir, fs_dir_entry_t *entry, bool *entry_valid)
{
    if ((dir == NULL) || (entry == NULL) || (entry_valid == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    memset(entry, 0, sizeof(*entry));
    *entry_valid = false;

    FILINFO info;
    FRESULT result = f_readdir(&dir->native, &info);
    if (result != FR_OK) {
        return map_fatfs_result(result);
    }

    if (info.fname[0] == '\0') {
        return APP_OK;
    }

    strncpy(entry->name, info.fname, sizeof(entry->name) - 1U);
    entry->size_bytes = (uint32_t)info.fsize;
    entry->is_directory = (info.fattrib & AM_DIR) != 0U;
    *entry_valid = true;
    return APP_OK;
}

app_status_t fs_dir_close(fs_dir_t *dir)
{
    if (dir == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    return map_fatfs_result(f_closedir(&dir->native));
}

static app_status_t map_fatfs_result(FRESULT result)
{
    switch (result) {
    case FR_OK:
        return APP_OK;
    case FR_INVALID_NAME:
    case FR_INVALID_OBJECT:
    case FR_INVALID_PARAMETER:
        return APP_ERROR_INVALID_ARGUMENT;
    case FR_NOT_READY:
    case FR_NOT_ENABLED:
    case FR_NO_FILESYSTEM:
        return APP_ERROR_NOT_READY;
    case FR_TIMEOUT:
        return APP_ERROR_TIMEOUT;
    default:
        return APP_ERROR;
    }
}

static BYTE fatfs_open_mode(uint32_t flags)
{
    BYTE mode = 0U;

    if ((flags & FS_OPEN_READ) != 0U) {
        mode |= FA_READ;
    }
    if ((flags & FS_OPEN_WRITE) != 0U) {
        mode |= FA_WRITE;
    }
    if ((flags & FS_OPEN_CREATE) != 0U) {
        mode |= FA_OPEN_ALWAYS;
    }
    if ((flags & FS_OPEN_TRUNCATE) != 0U) {
        mode |= FA_CREATE_ALWAYS;
    }
    if ((flags & FS_OPEN_APPEND) != 0U) {
        mode |= FA_OPEN_APPEND | FA_WRITE;
    }

    return mode;
}

static app_status_t build_fatfs_path(const char *path, char *buffer, size_t buffer_size)
{
    if ((path == NULL) || (buffer == NULL) || (buffer_size == 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    int written = snprintf(buffer, buffer_size, "%s%s%s", FATFS_SD_PREFIX, (path[0] == '/') ? "" : "/", path);
    return ((written > 0) && ((size_t)written < buffer_size)) ? APP_OK : APP_ERROR_INVALID_ARGUMENT;
}
