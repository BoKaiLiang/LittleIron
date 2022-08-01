# cmake的註解是從'#'開始的

cmake_minimum_required(VERSION 3.10) # --- 最低版本 ---
project(LittleIron) # --- 專案名稱 ---

#變數所有src資料夾中的source files
file(GLOB sources src/*.c) 

# 不需要建置docs, examples, tests
# from -  https://www.glfw.org/docs/latest/compile_guide.html#compile_options_shared
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

set(TARGET LittleIron)

add_executable( # 標記執行黨和原始碼
    ${TARGET}
    src/iron_asset.c
    src/iron_util.c
    src/iron_math.c
    src/iron_render_window.c
    src/iron_entity.c
    src/iron_main.c
)

add_subdirectory(external/glfw) # 引用由glfw建置的lib
include_directories(external/glfw/include)
include_directories(external/)

# 如果編譯器是mscv++
# ref: https://dotblogs.com.tw/thkaw/2013/12/11/133462
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

target_link_libraries( # 與glfw lib進行連接
    LittleIron
    glfw
)

# Copy assets file to build directory
set(ASSETS_SRC "${CMAKE_SOURCE_DIR}/assets")
set(ASSETS_DST "$<TARGET_FILE_DIR:${TARGET}>/assets")
if (WIN32)
    set(ASSETS_CMD copy_directory)
    set(ASSETS_MSG "Copy assets")
endif()

add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
    "${ASSETS_MSG}: ${ASSETS_SRC} to ${ASSETS_DST}"
)
add_custom_command(
    TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E ${ASSETS_CMD}
    "${ASSETS_SRC}" "${ASSETS_DST}"
)