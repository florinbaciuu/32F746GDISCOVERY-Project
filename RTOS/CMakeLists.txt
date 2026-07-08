set(FREERTOS_ROOT ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source)

target_sources(${PROJECT_TARGET}
    PRIVATE
        ${FREERTOS_ROOT}/croutine.c
        ${FREERTOS_ROOT}/event_groups.c
        ${FREERTOS_ROOT}/list.c
        ${FREERTOS_ROOT}/queue.c
        ${FREERTOS_ROOT}/stream_buffer.c
        ${FREERTOS_ROOT}/tasks.c
        ${FREERTOS_ROOT}/timers.c
        ${FREERTOS_ROOT}/portable/GCC/ARM_CM7/r0p1/port.c
        ${FREERTOS_ROOT}/portable/MemMang/heap_4.c
        ${CMAKE_CURRENT_LIST_DIR}/rtos_hooks.c
)

target_include_directories(${PROJECT_TARGET}
    PRIVATE
        ${FREERTOS_ROOT}/include
        ${FREERTOS_ROOT}/portable/GCC/ARM_CM7/r0p1
)
