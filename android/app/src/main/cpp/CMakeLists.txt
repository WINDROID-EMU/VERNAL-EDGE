cmake_minimum_required(VERSION 3.22.1)
project(VernalEdgeRunner C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -Wno-everything")

# Path to workspace runner
set(RUNNER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../runner")

include_directories(
    ${RUNNER_DIR}/include
    ${RUNNER_DIR}/src/bzip2
    ${ANDROID_NDK}/sources/android/native_app_glue
)

# Core runner C sources
set(RUNNER_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/android_main.c
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c
    ${RUNNER_DIR}/src/rvalue.c
    ${RUNNER_DIR}/src/ds.c
    ${RUNNER_DIR}/src/instance.c
    ${RUNNER_DIR}/src/room.c
    ${RUNNER_DIR}/src/data_win.c
    ${RUNNER_DIR}/src/graphics.c
    ${RUNNER_DIR}/src/gles_loader.c
    ${RUNNER_DIR}/src/audio.c
    ${RUNNER_DIR}/src/input.c
    ${RUNNER_DIR}/src/files.c
    ${RUNNER_DIR}/src/builtins_dispatcher.c
    ${RUNNER_DIR}/src/gml_globals_stub.c
    ${RUNNER_DIR}/src/main_loop.c
    ${RUNNER_DIR}/src/bzip2/bzlib.c
    ${RUNNER_DIR}/src/bzip2/blocksort.c
    ${RUNNER_DIR}/src/bzip2/compress.c
    ${RUNNER_DIR}/src/bzip2/crctable.c
    ${RUNNER_DIR}/src/bzip2/decompress.c
    ${RUNNER_DIR}/src/bzip2/huffman.c
    ${RUNNER_DIR}/src/bzip2/randtable.c
)

add_library(main SHARED ${RUNNER_SRCS})

target_link_libraries(main
    android
    log
    EGL
    GLESv3
    aaudio
    m
)
