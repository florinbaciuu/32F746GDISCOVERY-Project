#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <stdint.h>

typedef struct {
    const char *cpu_name;
    uint32_t core_clock_hz;
    uint32_t uptime_seconds;
    uint32_t display_width;
    uint32_t display_height;
    uint32_t sdram_base_address;
    uint32_t sdram_size_bytes;
    uint32_t framebuffer_0_address;
    uint32_t framebuffer_1_address;
    uint32_t framebuffer_size_bytes;
    uint32_t lvgl_heap_address;
    uint32_t lvgl_heap_size_bytes;
    uint32_t lvgl_draw_buffer_count;
    uint32_t lvgl_draw_buffer_size_bytes;
    uint32_t lvgl_draw_buffer_lines;
    uint32_t rtos_heap_total_bytes;
    uint32_t rtos_heap_free_bytes;
    uint32_t rtos_heap_min_free_bytes;
    uint32_t rtos_task_count;
} system_info_snapshot_t;

void system_info_get_snapshot(system_info_snapshot_t *snapshot);

#endif
