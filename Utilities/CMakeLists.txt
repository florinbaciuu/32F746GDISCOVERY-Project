target_sources(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/app_error.c
        ${CMAKE_CURRENT_LIST_DIR}/app_log.c
)

target_include_directories(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

