#include "bsp_display.h"

#include "board_config.h"

#if __has_include("stm32f7xx_hal.h")
#include "stm32f7xx_hal.h"
#define STM32_HAL_AVAILABLE 1
#else
#define STM32_HAL_AVAILABLE 0
#endif

#if __has_include("stm32746g_discovery_lcd.h")
#include "stm32746g_discovery_lcd.h"
#define ST_DISCOVERY_LCD_AVAILABLE 1
#else
#define ST_DISCOVERY_LCD_AVAILABLE 0
#endif

static bsp_display_config_t display_config = {
    .width = BOARD_DISPLAY_WIDTH,
    .height = BOARD_DISPLAY_HEIGHT,
    .pixel_format = BSP_DISPLAY_PIXEL_FORMAT_ARGB8888,
    .framebuffer_0 = (void *)BOARD_FRAMEBUFFER_0_ADDR,
    .framebuffer_1 = (void *)BOARD_FRAMEBUFFER_1_ADDR,
};

#if STM32_HAL_AVAILABLE
#define BSP_DISPLAY_DMA2D_TIMEOUT_MS 100U
#define BSP_DISPLAY_DCACHE_LINE_SIZE 32U

static DMA2D_HandleTypeDef dma2d_handle;

static app_status_t dma2d_copy_area(const bsp_display_area_t *area,
                                    const void *pixels,
                                    uint32_t width,
                                    uint32_t height);
static void clean_dcache_range(const void *address, uint32_t size);
static void clean_invalidate_dcache_range(const void *address, uint32_t size);
static void invalidate_dcache_range(const void *address, uint32_t size);
static void maintain_framebuffer_area(const bsp_display_area_t *area,
                                      uint32_t width,
                                      void (*maintenance_fn)(const void *address, uint32_t size));
static void get_aligned_cache_range(const void *address, uint32_t size, uint32_t *aligned_address, int32_t *aligned_size);
#endif

app_status_t bsp_display_init(void)
{
#if ST_DISCOVERY_LCD_AVAILABLE
    if (BSP_LCD_Init() != LCD_OK) {
        return APP_ERROR;
    }

    BSP_LCD_LayerDefaultInit(0U, (uint32_t)display_config.framebuffer_0);
    BSP_LCD_SelectLayer(0U);
    BSP_LCD_Clear(LCD_COLOR_BLUE);
    BSP_LCD_DisplayOn();
#endif

#if STM32_HAL_AVAILABLE
    __HAL_RCC_DMA2D_CLK_ENABLE();
#endif

    return APP_OK;
}

const bsp_display_config_t *bsp_display_get_config(void)
{
    return &display_config;
}

app_status_t bsp_display_flush(const bsp_display_area_t *area, const void *pixels)
{
    if ((area == NULL) || (pixels == NULL)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    if ((area->x2 < area->x1) || (area->y2 < area->y1) ||
        (area->x2 >= display_config.width) || (area->y2 >= display_config.height)) {
        return APP_ERROR_INVALID_ARGUMENT;
    }

    const uint32_t width = (uint32_t)area->x2 - (uint32_t)area->x1 + 1U;
    const uint32_t height = (uint32_t)area->y2 - (uint32_t)area->y1 + 1U;
    const uint32_t bytes_per_pixel = BOARD_DISPLAY_BPP;
#if STM32_HAL_AVAILABLE
    (void)bytes_per_pixel;
    return dma2d_copy_area(area, pixels, width, height);
#else
    (void)bytes_per_pixel;
    return APP_ERROR_UNSUPPORTED;
#endif
}

void bsp_display_set_backlight(bool enabled)
{
#if ST_DISCOVERY_LCD_AVAILABLE
    if (enabled) {
        BSP_LCD_DisplayOn();
    } else {
        BSP_LCD_DisplayOff();
    }
#else
    (void)enabled;
#endif
}

#if STM32_HAL_AVAILABLE
static app_status_t dma2d_copy_area(const bsp_display_area_t *area,
                                    const void *pixels,
                                    uint32_t width,
                                    uint32_t height)
{
    const uint32_t bytes_per_pixel = BOARD_DISPLAY_BPP;
    uint8_t *dst = (uint8_t *)display_config.framebuffer_0 +
                   (((uint32_t)area->y1 * display_config.width) + area->x1) * bytes_per_pixel;
    const uint32_t source_size = width * height * bytes_per_pixel;

    clean_dcache_range(pixels, source_size);
    maintain_framebuffer_area(area, width, clean_invalidate_dcache_range);

    dma2d_handle.Instance = DMA2D;
    dma2d_handle.Init.Mode = DMA2D_M2M;
    dma2d_handle.Init.ColorMode = DMA2D_ARGB8888;
    dma2d_handle.Init.OutputOffset = (uint32_t)display_config.width - width;
    dma2d_handle.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
    dma2d_handle.LayerCfg[1].InputAlpha = 0xFFU;
    dma2d_handle.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
    dma2d_handle.LayerCfg[1].InputOffset = 0U;

    if (HAL_DMA2D_Init(&dma2d_handle) != HAL_OK) {
        return APP_ERROR;
    }

    if (HAL_DMA2D_ConfigLayer(&dma2d_handle, 1U) != HAL_OK) {
        return APP_ERROR;
    }

    if (HAL_DMA2D_Start(&dma2d_handle, (uint32_t)pixels, (uint32_t)dst, width, height) != HAL_OK) {
        return APP_ERROR;
    }

    if (HAL_DMA2D_PollForTransfer(&dma2d_handle, BSP_DISPLAY_DMA2D_TIMEOUT_MS) != HAL_OK) {
        return APP_ERROR_TIMEOUT;
    }

    maintain_framebuffer_area(area, width, invalidate_dcache_range);
    return APP_OK;
}

static void clean_dcache_range(const void *address, uint32_t size)
{
    uint32_t aligned_address;
    int32_t aligned_size;

    get_aligned_cache_range(address, size, &aligned_address, &aligned_size);
    SCB_CleanDCache_by_Addr((uint32_t *)aligned_address, aligned_size);
}

static void clean_invalidate_dcache_range(const void *address, uint32_t size)
{
    uint32_t aligned_address;
    int32_t aligned_size;

    get_aligned_cache_range(address, size, &aligned_address, &aligned_size);
    SCB_CleanInvalidateDCache_by_Addr((uint32_t *)aligned_address, aligned_size);
}

static void invalidate_dcache_range(const void *address, uint32_t size)
{
    uint32_t aligned_address;
    int32_t aligned_size;

    get_aligned_cache_range(address, size, &aligned_address, &aligned_size);
    SCB_InvalidateDCache_by_Addr((uint32_t *)aligned_address, aligned_size);
}

static void maintain_framebuffer_area(const bsp_display_area_t *area,
                                      uint32_t width,
                                      void (*maintenance_fn)(const void *address, uint32_t size))
{
    const uint32_t row_bytes = width * BOARD_DISPLAY_BPP;
    const uint32_t stride_bytes = (uint32_t)display_config.width * BOARD_DISPLAY_BPP;
    uint8_t *row = (uint8_t *)display_config.framebuffer_0 +
                   (((uint32_t)area->y1 * display_config.width) + area->x1) * BOARD_DISPLAY_BPP;

    for (uint32_t y = area->y1; y <= area->y2; y++) {
        maintenance_fn(row, row_bytes);
        row += stride_bytes;
    }
}

static void get_aligned_cache_range(const void *address, uint32_t size, uint32_t *aligned_address, int32_t *aligned_size)
{
    const uint32_t start = (uint32_t)address;
    const uint32_t end = start + size;
    const uint32_t aligned_start = start & ~(BSP_DISPLAY_DCACHE_LINE_SIZE - 1U);
    const uint32_t aligned_end = (end + (BSP_DISPLAY_DCACHE_LINE_SIZE - 1U)) &
                                 ~(BSP_DISPLAY_DCACHE_LINE_SIZE - 1U);

    *aligned_address = aligned_start;
    *aligned_size = (int32_t)(aligned_end - aligned_start);
}
#endif
