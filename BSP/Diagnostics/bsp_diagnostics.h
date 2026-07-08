#ifndef BSP_DIAGNOSTICS_H
#define BSP_DIAGNOSTICS_H

#include <stdint.h>

#include "app_error.h"

typedef struct {
    uint32_t tested_address;
    uint32_t tested_bytes;
    uint32_t failing_address;
    uint32_t expected_value;
    uint32_t actual_value;
} bsp_sdram_test_result_t;

app_status_t bsp_diagnostics_sdram_quick_test(bsp_sdram_test_result_t *result);

#endif
