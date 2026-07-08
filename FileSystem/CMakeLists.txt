target_sources(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/fs.c
        ${CMAKE_CURRENT_LIST_DIR}/FatFs/fs_fatfs_diskio.c
        ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FatFs/source/ff.c
)

target_include_directories(${PROJECT_TARGET}
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/Middlewares/Third_Party/FatFs/source
)
