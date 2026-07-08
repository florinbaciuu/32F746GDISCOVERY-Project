#include "home_screen.h"

#include "app_log.h"
#include "system_info.h"

#if __has_include("lvgl.h")
#include "lvgl.h"
#define LVGL_AVAILABLE 1
#else
#define LVGL_AVAILABLE 0
#endif

#if LVGL_AVAILABLE
typedef struct {
    const char *title;
    const char *value;
} home_status_item_t;

enum {
    SYS_INFO_ROW_CPU = 0,
    SYS_INFO_ROW_CORE_CLOCK,
    SYS_INFO_ROW_UPTIME,
    SYS_INFO_ROW_DISPLAY,
    SYS_INFO_ROW_SDRAM,
    SYS_INFO_ROW_FRAMEBUFFER,
    SYS_INFO_ROW_LVGL_HEAP,
    SYS_INFO_ROW_DRAW_BUFFERS,
    SYS_INFO_ROW_RTOS_HEAP,
    SYS_INFO_ROW_RTOS_TASKS,
    SYS_INFO_ROW_COUNT,
};

static void create_home_tab(lv_obj_t *parent);
static void create_sys_info_tab(lv_obj_t *parent);
static void create_status_tile(lv_obj_t *parent, const home_status_item_t *item);
static void populate_sys_info_static_rows(lv_obj_t *table);
static void update_sys_info_table(lv_obj_t *table);
static uint32_t bytes_to_kib(uint32_t bytes);
static void sys_info_timer_cb(lv_timer_t *timer);
static void touch_button_event_cb(lv_event_t *event);

static lv_obj_t *touch_count_label;
static uint32_t touch_count;
#endif

void home_screen_create(void)
{
#if LVGL_AVAILABLE
    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x101820), 0);
    lv_obj_set_style_text_color(screen, lv_color_hex(0xF5F7FA), 0);
    lv_obj_set_style_text_font(screen, LV_FONT_DEFAULT, 0);

    lv_obj_t *tabview = lv_tabview_create(screen);
    lv_obj_set_size(tabview, LV_PCT(100), LV_PCT(100));
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_TOP);
    lv_tabview_set_tab_bar_size(tabview, 34);
    lv_obj_set_style_bg_color(tabview, lv_color_hex(0x101820), 0);

    lv_obj_t *tab_bar = lv_tabview_get_tab_bar(tabview);
    lv_obj_set_style_bg_color(tab_bar, lv_color_hex(0x172536), 0);
    lv_obj_set_style_text_color(tab_bar, lv_color_hex(0xF5F7FA), 0);

    lv_obj_t *home_tab = lv_tabview_add_tab(tabview, "Home");
    lv_obj_t *sys_info_tab = lv_tabview_add_tab(tabview, "Sys Info");

    create_home_tab(home_tab);
    create_sys_info_tab(sys_info_tab);
#endif
}

#if LVGL_AVAILABLE
static void create_home_tab(lv_obj_t *parent)
{
    static const home_status_item_t status_items[] = {
        {.title = "Display", .value = "LTDC + SDRAM"},
        {.title = "Render", .value = "LVGL v9"},
        {.title = "Touch", .value = "FT5336 ready"},
        {.title = "Target", .value = "STM32F746G"},
    };

    lv_obj_set_style_bg_color(parent, lv_color_hex(0x101820), 0);
    lv_obj_set_scrollbar_mode(parent, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *root = lv_obj_create(parent);
    lv_obj_remove_style_all(root);
    lv_obj_set_size(root, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(root, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(root, 14, 0);
    lv_obj_set_style_pad_row(root, 12, 0);

    lv_obj_t *title = lv_label_create(root);
    lv_label_set_text(title, "STM32F746G-DISCO Framework");
    lv_obj_set_style_text_font(title, LV_FONT_DEFAULT, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);

    lv_obj_t *subtitle = lv_label_create(root);
    lv_label_set_text(subtitle, "Reusable BSP + LVGL foundation");
    lv_obj_set_style_text_color(subtitle, lv_color_hex(0xB8C4D6), 0);

    lv_obj_t *grid = lv_obj_create(root);
    lv_obj_remove_style_all(grid);
    lv_obj_set_width(grid, LV_PCT(100));
    lv_obj_set_height(grid, 112);
    lv_obj_set_style_pad_column(grid, 8, 0);
    lv_obj_set_style_pad_row(grid, 8, 0);
    lv_obj_set_flex_flow(grid, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(grid, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (uint32_t i = 0U; i < (sizeof(status_items) / sizeof(status_items[0])); i++) {
        create_status_tile(grid, &status_items[i]);
    }

    lv_obj_t *button = lv_button_create(root);
    lv_obj_set_size(button, 210, 44);
    lv_obj_set_style_radius(button, 6, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(0x00A676), 0);
    lv_obj_add_event_cb(button, touch_button_event_cb, LV_EVENT_CLICKED, NULL);

    touch_count_label = lv_label_create(button);
    lv_label_set_text(touch_count_label, "Touch test: 0");
    lv_obj_center(touch_count_label);
}

static void create_sys_info_tab(lv_obj_t *parent)
{
    lv_obj_set_style_bg_color(parent, lv_color_hex(0x101820), 0);
    lv_obj_set_style_pad_all(parent, 8, 0);

    lv_obj_t *table = lv_table_create(parent);
    lv_obj_set_size(table, LV_PCT(100), LV_PCT(100));
    lv_obj_set_scrollbar_mode(table, LV_SCROLLBAR_MODE_AUTO);
    lv_table_set_column_count(table, 2);
    lv_table_set_row_count(table, SYS_INFO_ROW_COUNT);
    lv_table_set_column_width(table, 0, 142);
    lv_table_set_column_width(table, 1, 300);
    lv_obj_set_style_bg_color(table, lv_color_hex(0x142131), 0);
    lv_obj_set_style_border_color(table, lv_color_hex(0x314154), 0);
    lv_obj_set_style_border_width(table, 1, 0);
    lv_obj_set_style_text_color(table, lv_color_hex(0xF5F7FA), 0);
    lv_obj_set_style_pad_all(table, 6, LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(table, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_set_style_border_color(table, lv_color_hex(0x314154), LV_PART_ITEMS);

    populate_sys_info_static_rows(table);
    update_sys_info_table(table);

    (void)lv_timer_create(sys_info_timer_cb, 1000, table);
}

static void create_status_tile(lv_obj_t *parent, const home_status_item_t *item)
{
    lv_obj_t *tile = lv_obj_create(parent);
    lv_obj_set_size(tile, 108, 48);
    lv_obj_set_style_radius(tile, 6, 0);
    lv_obj_set_style_border_width(tile, 1, 0);
    lv_obj_set_style_border_color(tile, lv_color_hex(0x314154), 0);
    lv_obj_set_style_bg_color(tile, lv_color_hex(0x182635), 0);
    lv_obj_set_style_pad_all(tile, 6, 0);
    lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *title = lv_label_create(tile);
    lv_label_set_text(title, item->title);
    lv_obj_set_style_text_color(title, lv_color_hex(0x8FB7FF), 0);

    lv_obj_t *value = lv_label_create(tile);
    lv_label_set_text(value, item->value);
    lv_obj_set_style_text_color(value, lv_color_hex(0xF5F7FA), 0);
}

static void populate_sys_info_static_rows(lv_obj_t *table)
{
    system_info_snapshot_t snapshot;
    system_info_get_snapshot(&snapshot);

    lv_table_set_cell_value(table, SYS_INFO_ROW_CPU, 0, "CPU");
    lv_table_set_cell_value(table, SYS_INFO_ROW_CPU, 1, snapshot.cpu_name);

    lv_table_set_cell_value(table, SYS_INFO_ROW_CORE_CLOCK, 0, "Core clock");
    lv_table_set_cell_value(table, SYS_INFO_ROW_UPTIME, 0, "Uptime");

    lv_table_set_cell_value(table, SYS_INFO_ROW_DISPLAY, 0, "Display");
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_DISPLAY,
                                1,
                                "%lux%lu / XRGB8888",
                                (unsigned long)snapshot.display_width,
                                (unsigned long)snapshot.display_height);

    lv_table_set_cell_value(table, SYS_INFO_ROW_SDRAM, 0, "SDRAM");
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_SDRAM,
                                1,
                                "0x%08lX / %lu KiB",
                                (unsigned long)snapshot.sdram_base_address,
                                (unsigned long)bytes_to_kib(snapshot.sdram_size_bytes));

    lv_table_set_cell_value(table, SYS_INFO_ROW_FRAMEBUFFER, 0, "Framebuffers");
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_FRAMEBUFFER,
                                1,
                                "0x%08lX / 0x%08lX",
                                (unsigned long)snapshot.framebuffer_0_address,
                                (unsigned long)snapshot.framebuffer_1_address);

    lv_table_set_cell_value(table, SYS_INFO_ROW_LVGL_HEAP, 0, "LVGL heap");
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_LVGL_HEAP,
                                1,
                                "0x%08lX / %lu KiB",
                                (unsigned long)snapshot.lvgl_heap_address,
                                (unsigned long)bytes_to_kib(snapshot.lvgl_heap_size_bytes));

    lv_table_set_cell_value(table, SYS_INFO_ROW_DRAW_BUFFERS, 0, "Draw buffers");
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_DRAW_BUFFERS,
                                1,
                                "%lu x %lu KiB (%lu lines)",
                                (unsigned long)snapshot.lvgl_draw_buffer_count,
                                (unsigned long)bytes_to_kib(snapshot.lvgl_draw_buffer_size_bytes),
                                (unsigned long)snapshot.lvgl_draw_buffer_lines);

    lv_table_set_cell_value(table, SYS_INFO_ROW_RTOS_HEAP, 0, "RTOS heap");
    lv_table_set_cell_value(table, SYS_INFO_ROW_RTOS_TASKS, 0, "RTOS tasks");
}

static void update_sys_info_table(lv_obj_t *table)
{
    system_info_snapshot_t snapshot;
    system_info_get_snapshot(&snapshot);

    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_CORE_CLOCK,
                                1,
                                "%lu MHz",
                                (unsigned long)(snapshot.core_clock_hz / 1000000UL));
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_UPTIME,
                                1,
                                "%lu s",
                                (unsigned long)snapshot.uptime_seconds);
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_RTOS_HEAP,
                                1,
                                "%lu KiB total / %lu KiB free / %lu KiB min",
                                (unsigned long)bytes_to_kib(snapshot.rtos_heap_total_bytes),
                                (unsigned long)bytes_to_kib(snapshot.rtos_heap_free_bytes),
                                (unsigned long)bytes_to_kib(snapshot.rtos_heap_min_free_bytes));
    lv_table_set_cell_value_fmt(table,
                                SYS_INFO_ROW_RTOS_TASKS,
                                1,
                                "%lu active",
                                (unsigned long)snapshot.rtos_task_count);
}

static uint32_t bytes_to_kib(uint32_t bytes)
{
    return bytes / 1024U;
}

static void sys_info_timer_cb(lv_timer_t *timer)
{
    update_sys_info_table((lv_obj_t *)lv_timer_get_user_data(timer));
}

static void touch_button_event_cb(lv_event_t *event)
{
    (void)event;

    touch_count++;
    lv_label_set_text_fmt(touch_count_label, "Touch test: %lu", (unsigned long)touch_count);
    APP_LOG_INFO("ui", "touch button pressed: count=%lu", (unsigned long)touch_count);
}
#endif
