cmake_minimum_required(VERSION 2.8)

project(Common)

set(CO_LIB "mycoroutine")
set(SAMPLE "sample")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++2a -o0 -lpthread")
    set(OUTPUT_PATH "Debug")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -o3 -std=c++2a -lpthread")
    set(OUTPUT_PATH "Release")
endif()

include_directories(${PROJECT_SOURCE_DIR}/coroutine/)
include_directories(${PROJECT_SOURCE_DIR}/c++20/)
include_directories(${PROJECT_SOURCE_DIR}/lockfree/)
include_directories(${PROJECT_SOURCE_DIR}/threadpool/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${OUTPUT_PATH}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${OUTPUT_PATH}/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${OUTPUT_PATH}/)

link_directories(${PROJECT_SOURCE_DIR}/lib/${OUTPUT_PATH}/)

add_subdirectory(${PROJECT_SOURCE_DIR}/coroutine/)
add_subdirectory(${PROJECT_SOURCE_DIR}/sample/)

if(${TEST} STREQUAL "YES")
    # add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
    message("build test!")
endif()
