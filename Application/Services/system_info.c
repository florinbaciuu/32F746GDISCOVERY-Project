#include "system_info.h"

#include "board_config.h"
#include "bsp.h"
#include "FreeRTOS.h"
#include "task.h"

void system_info_get_snapshot(system_info_snapshot_t *snapshot)
{
    if (snapshot == NULL) {
        return;
    }

    snapshot->cpu_name = "STM32F746NG / Cortex-M7";
    snapshot->core_clock_hz = SystemCoreClock;
    snapshot->uptime_seconds = bsp_tick_get() / 1000U;
    snapshot->display_width = BOARD_DISPLAY_WIDTH;
    snapshot->display_height = BOARD_DISPLAY_HEIGHT;
    snapshot->sdram_base_address = BOARD_SDRAM_BASE_ADDRESS;
    snapshot->sdram_size_bytes = BOARD_SDRAM_SIZE;
    snapshot->framebuffer_0_address = BOARD_FRAMEBUFFER_0_ADDR;
    snapshot->framebuffer_1_address = BOARD_FRAMEBUFFER_1_ADDR;
    snapshot->framebuffer_size_bytes = BOARD_FRAMEBUFFER_SIZE;
    snapshot->lvgl_heap_address = BOARD_LVGL_HEAP_ADDR;
    snapshot->lvgl_heap_size_bytes = BOARD_LVGL_HEAP_SIZE;
    snapshot->lvgl_draw_buffer_count = 2U;
    snapshot->lvgl_draw_buffer_size_bytes = BOARD_LVGL_DRAW_BUFFER_SIZE;
    snapshot->lvgl_draw_buffer_lines = BOARD_LVGL_DRAW_BUFFER_LINES;
    snapshot->rtos_heap_total_bytes = configTOTAL_HEAP_SIZE;
    snapshot->rtos_heap_free_bytes = (uint32_t)xPortGetFreeHeapSize();
    snapshot->rtos_heap_min_free_bytes = (uint32_t)xPortGetMinimumEverFreeHeapSize();
    snapshot->rtos_task_count = (uint32_t)uxTaskGetNumberOfTasks();
}
