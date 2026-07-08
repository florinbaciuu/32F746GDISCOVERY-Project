target_sources(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/app.c
        ${CMAKE_CURRENT_LIST_DIR}/Screens/home_screen.c
        ${CMAKE_CURRENT_LIST_DIR}/Services/system_info.c
)

target_include_directories(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/Screens
        ${CMAKE_CURRENT_LIST_DIR}/Services
)
