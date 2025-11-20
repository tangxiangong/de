cmake_minimum_required(VERSION 4.0)
project(de LANGUAGES CXX)

# find_package(FFTW3 CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -isysroot ${MACOS_SDK_PATH}")
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

file(GLOB_RECURSE MODULE_SOURCES "src/*.cppm")
add_library(de SHARED)

target_sources(de
    PUBLIC
        FILE_SET CXX_MODULES FILES
        ${MODULE_SOURCES}
)

set_target_properties(de PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib
    CXX_SCAN_FOR_MODULES ON
)
# target_link_libraries(de PRIVATE FFTW3::fftw3)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB BIN_SOURCES "${CMAKE_SOURCE_DIR}/src/bin/*.cpp")
foreach(BIN_SOURCE ${BIN_SOURCES})
    get_filename_component(BIN_NAME ${BIN_SOURCE} NAME_WE)

    add_executable(${BIN_NAME} ${BIN_SOURCE})

    target_link_libraries(${BIN_NAME} PRIVATE de)

    set_target_properties(${BIN_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
        CXX_SCAN_FOR_MODULES ON
    )
endforeach()

add_custom_command(
    TARGET de POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_BINARY_DIR}/compile_commands.json"
    "${PROJECT_SOURCE_DIR}/compile_commands.json"
    COMMENT "Copying compile_commands.json to project root"
)
