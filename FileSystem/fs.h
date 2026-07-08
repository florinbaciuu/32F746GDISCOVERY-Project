#ifndef FS_H
#define FS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "app_error.h"
#include "ff.h"

typedef enum {
    FS_VOLUME_SD = 0,
    FS_VOLUME_COUNT,
} fs_volume_t;

typedef enum {
    FS_OPEN_READ = 0x01,
    FS_OPEN_WRITE = 0x02,
    FS_OPEN_CREATE = 0x04,
    FS_OPEN_TRUNCATE = 0x08,
    FS_OPEN_APPEND = 0x10,
} fs_open_flags_t;

typedef enum {
    FS_SEEK_SET = 0,
    FS_SEEK_CUR,
    FS_SEEK_END,
} fs_seek_origin_t;

typedef struct {
    fs_volume_t volume;
    FIL native;
} fs_file_t;

typedef struct {
    fs_volume_t volume;
    DIR native;
} fs_dir_t;

typedef struct {
    char name[128];
    uint32_t size_bytes;
    bool is_directory;
} fs_dir_entry_t;

typedef struct {
    bool mounted;
    uint32_t total_bytes;
    uint32_t free_bytes;
} fs_volume_info_t;

app_status_t fs_init(void);
app_status_t fs_mount(fs_volume_t volume, bool format_if_needed);
app_status_t fs_unmount(fs_volume_t volume);
app_status_t fs_format(fs_volume_t volume);
app_status_t fs_get_info(fs_volume_t volume, fs_volume_info_t *info);

app_status_t fs_file_open(fs_volume_t volume, const char *path, uint32_t flags, fs_file_t *file);
app_status_t fs_file_close(fs_file_t *file);
app_status_t fs_file_read(fs_file_t *file, void *buffer, size_t size, size_t *bytes_read);
app_status_t fs_file_write(fs_file_t *file, const void *buffer, size_t size, size_t *bytes_written);
app_status_t fs_file_seek(fs_file_t *file, int32_t offset, fs_seek_origin_t origin);
app_status_t fs_file_tell(fs_file_t *file, uint32_t *position);
app_status_t fs_file_sync(fs_file_t *file);

app_status_t fs_mkdir(fs_volume_t volume, const char *path);
app_status_t fs_remove(fs_volume_t volume, const char *path);
app_status_t fs_rename(fs_volume_t volume, const char *old_path, const char *new_path);

app_status_t fs_dir_open(fs_volume_t volume, const char *path, fs_dir_t *dir);
app_status_t fs_dir_read(fs_dir_t *dir, fs_dir_entry_t *entry, bool *entry_valid);
app_status_t fs_dir_close(fs_dir_t *dir);

#endif
