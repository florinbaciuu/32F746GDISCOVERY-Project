target_sources(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Port/lvgl_port.c
)

target_include_directories(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/Port
        ${CMAKE_SOURCE_DIR}/Config
)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/lvgl/CMakeLists.txt")
    set(LV_BUILD_CONF_DIR "${CMAKE_SOURCE_DIR}/Config" CACHE PATH "Directory containing lv_conf.h" FORCE)
    set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "Build LVGL demos" FORCE)
    set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "Build LVGL examples" FORCE)
    set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "Use LVGL internal ThorVG" FORCE)
    set(CONFIG_LV_USE_THORVG_EXTERNAL OFF CACHE BOOL "Use external ThorVG" FORCE)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lvgl)
    target_link_libraries(${PROJECT_TARGET} PRIVATE lvgl)
else()
    message(WARNING "LVGL source tree not found at LVGL/lvgl. Add LVGL v9 as a submodule or copy before building.")
endif()
