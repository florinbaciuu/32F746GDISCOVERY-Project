#include "app_log.h"

#include <stdarg.h>
#include <stdio.h>

#define APP_LOG_LINE_BUFFER_SIZE 192U

static app_log_writer_t log_writer;
static app_log_tick_reader_t log_tick_reader;
static app_log_level_t log_level = APP_LOG_LEVEL_INFO;

static const char *level_to_string(app_log_level_t level)
{
    switch (level) {
    case APP_LOG_LEVEL_ERROR:
        return "E";
    case APP_LOG_LEVEL_WARN:
        return "W";
    case APP_LOG_LEVEL_INFO:
        return "I";
    case APP_LOG_LEVEL_DEBUG:
        return "D";
    default:
        return "?";
    }
}

static uint32_t timestamp_ms(void)
{
    if (log_tick_reader == NULL) {
        return 0U;
    }

    return log_tick_reader();
}

static void write_line(const char *line, size_t length)
{
    if (log_writer == NULL) {
        return;
    }

    log_writer(line, length);
}

void app_log_init(app_log_writer_t writer, app_log_tick_reader_t tick_reader)
{
    log_writer = writer;
    log_tick_reader = tick_reader;
}

void app_log_set_level(app_log_level_t level)
{
    log_level = level;
}

void app_log(app_log_level_t level, const char *module, const char *message)
{
    app_logf(level, module, "%s", (message != NULL) ? message : "");
}

void app_logf(app_log_level_t level, const char *module, const char *format, ...)
{
    if ((level > log_level) || (log_writer == NULL)) {
        return;
    }

    char line[APP_LOG_LINE_BUFFER_SIZE];
    const char *safe_module = (module != NULL) ? module : "app";
    const char *safe_format = (format != NULL) ? format : "";

    int used = snprintf(line,
                        sizeof(line),
                        "[%010lu] %s/%s: ",
                        (unsigned long)timestamp_ms(),
                        level_to_string(level),
                        safe_module);
    if (used < 0) {
        return;
    }

    size_t offset = (used < (int)sizeof(line)) ? (size_t)used : (sizeof(line) - 1U);

    va_list args;
    va_start(args, format);
    used = vsnprintf(&line[offset], sizeof(line) - offset, safe_format, args);
    va_end(args);

    if (used < 0) {
        return;
    }

    size_t length = offset + ((used < (int)(sizeof(line) - offset)) ? (size_t)used : (sizeof(line) - offset - 1U));
    if ((length + 2U) < sizeof(line)) {
        line[length++] = '\r';
        line[length++] = '\n';
        line[length] = '\0';
    } else {
        line[sizeof(line) - 3U] = '\r';
        line[sizeof(line) - 2U] = '\n';
        line[sizeof(line) - 1U] = '\0';
        length = sizeof(line) - 1U;
    }

    write_line(line, length);
}
