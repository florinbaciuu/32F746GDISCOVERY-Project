#ifndef APP_ERROR_H
#define APP_ERROR_H

typedef enum {
    APP_OK = 0,
    APP_ERROR = -1,
    APP_ERROR_INVALID_ARGUMENT = -2,
    APP_ERROR_NOT_READY = -3,
    APP_ERROR_TIMEOUT = -4,
    APP_ERROR_UNSUPPORTED = -5
} app_status_t;

#endif

