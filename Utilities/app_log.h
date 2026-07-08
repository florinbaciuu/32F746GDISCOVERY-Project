#ifndef APP_LOG_H
#define APP_LOG_H

typedef enum {
    APP_LOG_LEVEL_ERROR = 0,
    APP_LOG_LEVEL_WARN,
    APP_LOG_LEVEL_INFO,
    APP_LOG_LEVEL_DEBUG
} app_log_level_t;

#include <stddef.h>
#include <stdint.h>

typedef void (*app_log_writer_t)(const char *data, size_t length);
typedef uint32_t (*app_log_tick_reader_t)(void);

void app_log_init(app_log_writer_t writer, app_log_tick_reader_t tick_reader);
void app_log_set_level(app_log_level_t level);
void app_log(app_log_level_t level, const char *module, const char *message);
void app_logf(app_log_level_t level, const char *module, const char *format, ...);

#define APP_LOG_ERROR(module, format, ...) app_logf(APP_LOG_LEVEL_ERROR, (module), (format), ##__VA_ARGS__)
#define APP_LOG_WARN(module, format, ...)  app_logf(APP_LOG_LEVEL_WARN, (module), (format), ##__VA_ARGS__)
#define APP_LOG_INFO(module, format, ...)  app_logf(APP_LOG_LEVEL_INFO, (module), (format), ##__VA_ARGS__)
#define APP_LOG_DEBUG(module, format, ...) app_logf(APP_LOG_LEVEL_DEBUG, (module), (format), ##__VA_ARGS__)

#endif
