#include "main.h"

#include "app.h"
#include "bsp_io.h"

#if __has_include("stm32746g_discovery.h")
#include "stm32746g_discovery.h"
#endif

static void mpu_config(void);
static void cpu_cache_enable(void);
static void system_clock_config(void);

int main(void)
{
    mpu_config();
    cpu_cache_enable();

    HAL_Init();
    system_clock_config();

    if (app_init() != APP_OK) {
        Error_Handler();
    }

    app_run();
}

void Error_Handler(void)
{
    __disable_irq();
#if __has_include("stm32746g_discovery.h")
    BSP_LED_Init(LED_GREEN);
#endif
    while (1) {
#if __has_include("stm32746g_discovery.h")
        BSP_LED_Toggle(LED_GREEN);
        for (volatile uint32_t delay = 0U; delay < 500000U; delay++) {
        }
#endif
    }
}

static void system_clock_config(void)
{
    RCC_OscInitTypeDef osc = {0};
    RCC_ClkInitTypeDef clk = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    osc.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc.HSEState = RCC_HSE_ON;
    osc.HSIState = RCC_HSI_OFF;
    osc.PLL.PLLState = RCC_PLL_ON;
    osc.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    osc.PLL.PLLM = 25;
    osc.PLL.PLLN = 432;
    osc.PLL.PLLP = RCC_PLLP_DIV2;
    osc.PLL.PLLQ = 9;
    if (HAL_RCC_OscConfig(&osc) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

    clk.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
                    RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clk.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk.APB1CLKDivider = RCC_HCLK_DIV4;
    clk.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&clk, FLASH_LATENCY_7) != HAL_OK) {
        Error_Handler();
    }
}

static void cpu_cache_enable(void)
{
    SCB_EnableICache();
    SCB_EnableDCache();
}

static void mpu_config(void)
{
    MPU_Region_InitTypeDef mpu = {0};

    HAL_MPU_Disable();

    mpu.Enable = MPU_REGION_ENABLE;
    mpu.BaseAddress = 0x00000000U;
    mpu.Size = MPU_REGION_SIZE_4GB;
    mpu.AccessPermission = MPU_REGION_NO_ACCESS;
    mpu.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    mpu.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    mpu.IsShareable = MPU_ACCESS_SHAREABLE;
    mpu.Number = MPU_REGION_NUMBER0;
    mpu.TypeExtField = MPU_TEX_LEVEL0;
    mpu.SubRegionDisable = 0x87U;
    mpu.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    HAL_MPU_ConfigRegion(&mpu);

    mpu.Enable = MPU_REGION_ENABLE;
    mpu.BaseAddress = 0xC0000000U;
    mpu.Size = MPU_REGION_SIZE_8MB;
    mpu.AccessPermission = MPU_REGION_FULL_ACCESS;
    mpu.IsBufferable = MPU_ACCESS_BUFFERABLE;
    mpu.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    mpu.IsShareable = MPU_ACCESS_SHAREABLE;
    mpu.Number = MPU_REGION_NUMBER1;
    mpu.TypeExtField = MPU_TEX_LEVEL0;
    mpu.SubRegionDisable = 0x00U;
    mpu.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    HAL_MPU_ConfigRegion(&mpu);

    mpu.Enable = MPU_REGION_ENABLE;
    mpu.BaseAddress = 0xA0000000U;
    mpu.Size = MPU_REGION_SIZE_256MB;
    mpu.AccessPermission = MPU_REGION_FULL_ACCESS;
    mpu.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    mpu.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    mpu.IsShareable = MPU_ACCESS_SHAREABLE;
    mpu.Number = MPU_REGION_NUMBER2;
    mpu.TypeExtField = MPU_TEX_LEVEL0;
    mpu.SubRegionDisable = 0x00U;
    mpu.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    HAL_MPU_ConfigRegion(&mpu);

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
    Error_Handler();
}
#endif
