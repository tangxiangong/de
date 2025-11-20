cmake_minimum_required(VERSION 4.0)
project(de LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -isysroot ${MACOS_SDK_PATH}")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(de SHARED
    src/ode.c
    src/utils.c
)

target_include_directories(de PUBLIC include)

set_target_properties(de PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(de PRIVATE m)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB BIN_SOURCES "${CMAKE_SOURCE_DIR}/src/bin/*.c")
foreach(BIN_SOURCE ${BIN_SOURCES})
    get_filename_component(BIN_NAME ${BIN_SOURCE} NAME_WE)

    add_executable(${BIN_NAME} ${BIN_SOURCE})

    target_link_libraries(${BIN_NAME} PRIVATE de)

    set_target_properties(${BIN_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
    )
endforeach()

add_custom_command(
    TARGET de POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${PROJECT_SOURCE_DIR}/compile_commands.json"
    COMMENT "Copying compile_commands.json to project root"
)
