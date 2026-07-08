#include "FreeRTOS.h"
#include "task.h"

#include "app_log.h"

void vApplicationStackOverflowHook(void *task_handle, char *task_name)
{
    (void)task_handle;
    APP_LOG_ERROR("rtos", "stack overflow: %s", (task_name != NULL) ? task_name : "?");
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}

void vApplicationMallocFailedHook(void)
{
    APP_LOG_ERROR("rtos", "malloc failed");
    taskDISABLE_INTERRUPTS();
    for (;;) {
    }
}

void vApplicationIdleHook(void)
{
}

void vApplicationTickHook(void)
{
}
