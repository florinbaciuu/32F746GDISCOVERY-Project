#include "bsp_diagnostics.h"

#include "board_config.h"

#include <stddef.h>

static const uint32_t test_patterns[] = {
    0x00000000UL,
    0xFFFFFFFFUL,
    0xA5A5A5A5UL,
    0x5A5A5A5AUL,
};

static app_status_t verify_pattern(volatile uint32_t *memory,
                                   uint32_t word_count,
                                   uint32_t pattern,
                                   bsp_sdram_test_result_t *result);

app_status_t bsp_diagnostics_sdram_quick_test(bsp_sdram_test_result_t *result)
{
    if (result == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    result->tested_address = BOARD_DIAGNOSTIC_SDRAM_TEST_ADDR;
    result->tested_bytes = BOARD_DIAGNOSTIC_SDRAM_TEST_SIZE;
    result->failing_address = 0U;
    result->expected_value = 0U;
    result->actual_value = 0U;

    volatile uint32_t *memory = (volatile uint32_t *)BOARD_DIAGNOSTIC_SDRAM_TEST_ADDR;
    const uint32_t word_count = BOARD_DIAGNOSTIC_SDRAM_TEST_SIZE / sizeof(uint32_t);

    for (uint32_t pattern_index = 0U;
         pattern_index < (sizeof(test_patterns) / sizeof(test_patterns[0]));
         pattern_index++) {
        const uint32_t pattern = test_patterns[pattern_index];

        for (uint32_t word = 0U; word < word_count; word++) {
            memory[word] = pattern ^ word;
        }

        app_status_t status = verify_pattern(memory, word_count, pattern, result);
        if (status != APP_OK) {
            return status;
        }
    }

    return APP_OK;
}

static app_status_t verify_pattern(volatile uint32_t *memory,
                                   uint32_t word_count,
                                   uint32_t pattern,
                                   bsp_sdram_test_result_t *result)
{
    for (uint32_t word = 0U; word < word_count; word++) {
        const uint32_t expected = pattern ^ word;
        const uint32_t actual = memory[word];

        if (actual != expected) {
            result->failing_address = (uint32_t)&memory[word];
            result->expected_value = expected;
            result->actual_value = actual;
            return APP_ERROR;
        }
    }

    return APP_OK;
}
