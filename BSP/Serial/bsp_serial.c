#include "bsp_serial.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#if __has_include("stm32f7xx_hal.h")
#include "stm32f7xx_hal.h"
#define STM32_HAL_AVAILABLE 1
#else
#define STM32_HAL_AVAILABLE 0
#endif

#define BSP_SERIAL_TX_TIMEOUT_MS 100U

#if STM32_HAL_AVAILABLE
static UART_HandleTypeDef serial_uart;
static bool serial_initialized;

static void serial_gpio_init(void)
{
    GPIO_InitTypeDef gpio = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    gpio.Mode = GPIO_MODE_AF_PP;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio.Alternate = GPIO_AF7_USART1;

    gpio.Pin = GPIO_PIN_9;
    HAL_GPIO_Init(GPIOA, &gpio);

    gpio.Pin = GPIO_PIN_7;
    HAL_GPIO_Init(GPIOB, &gpio);
}
#endif

app_status_t bsp_serial_init(void)
{
#if STM32_HAL_AVAILABLE
    serial_gpio_init();
    __HAL_RCC_USART1_CLK_ENABLE();

    serial_uart.Instance = USART1;
    serial_uart.Init.BaudRate = 115200U;
    serial_uart.Init.WordLength = UART_WORDLENGTH_8B;
    serial_uart.Init.StopBits = UART_STOPBITS_1;
    serial_uart.Init.Parity = UART_PARITY_NONE;
    serial_uart.Init.Mode = UART_MODE_TX_RX;
    serial_uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    serial_uart.Init.OverSampling = UART_OVERSAMPLING_16;
    serial_uart.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    serial_uart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&serial_uart) != HAL_OK) {
        serial_initialized = false;
        return APP_ERROR;
    }

    serial_initialized = true;
#endif

    return APP_OK;
}

app_status_t bsp_serial_write(const char *data, size_t length)
{
    if ((data == NULL) && (length > 0U)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

#if STM32_HAL_AVAILABLE
    if (!serial_initialized) {
        return APP_ERROR;
    }

    while (length > 0U) {
        const uint16_t chunk = (length > UINT16_MAX) ? UINT16_MAX : (uint16_t)length;
        if (HAL_UART_Transmit(&serial_uart, (uint8_t *)data, chunk, BSP_SERIAL_TX_TIMEOUT_MS) != HAL_OK) {
            return APP_ERROR;
        }

        data += chunk;
        length -= chunk;
    }
#else
    (void)data;
    (void)length;
#endif

    return APP_OK;
}

app_status_t bsp_serial_write_string(const char *message)
{
    if (message == NULL) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    return bsp_serial_write(message, strlen(message));
}

int __io_putchar(int ch)
{
    const char c = (char)ch;
    (void)bsp_serial_write(&c, 1U);
    return ch;
}
