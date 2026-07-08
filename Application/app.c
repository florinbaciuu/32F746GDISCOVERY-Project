#include "app.h"

#include "app_log.h"
#include "bsp.h"
#include "home_screen.h"
#include "lvgl_port.h"

#include "FreeRTOS.h"
#include "task.h"

#define GUI_TASK_STACK_WORDS    2048U
#define GUI_TASK_PRIORITY       (tskIDLE_PRIORITY + 3U)
#define GUI_TASK_PERIOD_MS      5U

#define SYSTEM_TASK_STACK_WORDS 512U
#define SYSTEM_TASK_PRIORITY    (tskIDLE_PRIORITY + 1U)
#define SYSTEM_TASK_PERIOD_MS   50U
#define SYSTEM_HEARTBEAT_MS     500U
#define SYSTEM_LOG_PERIOD_MS    5000U

static void gui_task(void *argument);
static void system_task(void *argument);
static app_status_t create_application_tasks(void);
static void log_board_diagnostics(void);

app_status_t app_init(void)
{
    app_status_t status = bsp_init();
    if (status != APP_OK) {
        return status;
    }
    APP_LOG_INFO("app", "boot");

    status = lvgl_port_init();
    if (status != APP_OK) {
        APP_LOG_ERROR("app", "lvgl port init failed");
        return status;
    }
    APP_LOG_INFO("app", "lvgl ready");

    home_screen_create();
    APP_LOG_INFO("app", "home screen ready");

    return APP_OK;
}

void app_run(void)
{
    if (create_application_tasks() != APP_OK) {
        return;
    }

    APP_LOG_INFO("rtos", "starting scheduler");
    vTaskStartScheduler();

    APP_LOG_ERROR("rtos", "scheduler returned");
    for (;;) {
    }
}

static app_status_t create_application_tasks(void)
{
    BaseType_t created = xTaskCreate(gui_task,
                                     "gui",
                                     GUI_TASK_STACK_WORDS,
                                     NULL,
                                     GUI_TASK_PRIORITY,
                                     NULL);
    if (created != pdPASS) {
        APP_LOG_ERROR("rtos", "failed to create gui task");
        return APP_ERROR;
    }

    created = xTaskCreate(system_task,
                          "system",
                          SYSTEM_TASK_STACK_WORDS,
                          NULL,
                          SYSTEM_TASK_PRIORITY,
                          NULL);
    if (created != pdPASS) {
        APP_LOG_ERROR("rtos", "failed to create system task");
        return APP_ERROR;
    }

    APP_LOG_INFO("rtos", "tasks created");
    return APP_OK;
}

static void gui_task(void *argument)
{
    (void)argument;

    TickType_t last_wake = xTaskGetTickCount();

    APP_LOG_INFO("gui", "task started");

    for (;;) {
        lvgl_port_task_handler();
        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(GUI_TASK_PERIOD_MS));
    }
}

static void system_task(void *argument)
{
    (void)argument;

    TickType_t last_wake = xTaskGetTickCount();
    uint32_t last_heartbeat = bsp_tick_get();
    uint32_t last_log = bsp_tick_get();
    bool led_on = false;

    APP_LOG_INFO("system", "task started");
    log_board_diagnostics();

    for (;;) {
        const uint32_t now = bsp_tick_get();
        if ((now - last_heartbeat) >= SYSTEM_HEARTBEAT_MS) {
            led_on = !led_on;
            bsp_led_set(BSP_LED_GREEN, led_on);
            last_heartbeat = now;
        }

        if ((now - last_log) >= SYSTEM_LOG_PERIOD_MS) {
            APP_LOG_DEBUG("system", "alive");
            last_log = now;
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(SYSTEM_TASK_PERIOD_MS));
    }
}

static void log_board_diagnostics(void)
{
    bsp_sdram_test_result_t sdram_result;
    app_status_t status = bsp_diagnostics_sdram_quick_test(&sdram_result);
    if (status == APP_OK) {
        APP_LOG_INFO("sdram",
                     "quick test ok: addr=0x%08lX size=%lu bytes",
                     (unsigned long)sdram_result.tested_address,
                     (unsigned long)sdram_result.tested_bytes);
    } else {
        APP_LOG_ERROR("sdram",
                      "quick test failed: addr=0x%08lX expected=0x%08lX actual=0x%08lX",
                      (unsigned long)sdram_result.failing_address,
                      (unsigned long)sdram_result.expected_value,
                      (unsigned long)sdram_result.actual_value);
    }
}
