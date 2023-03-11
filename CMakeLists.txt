cmake_minimum_required(VERSION 2.8)

project(Utility)

set(COROUTINE "mycoroutine")
set(SAMPLES "sample")
set(TESTS "utilityTest")

if(CMAKE_BUILD_TYPE STREQUAL "debug")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -std=c++2a -o0 -lpthread")
    set(OUTPUT_PATH "Debug")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "-Wall -o3 -std=c++2a -lpthread")
    set(OUTPUT_PATH "Release")
endif()

include_directories(${PROJECT_SOURCE_DIR}/coroutine/)
include_directories(${PROJECT_SOURCE_DIR}/cppNewFeature/)
include_directories(${PROJECT_SOURCE_DIR}/lockfree/)
include_directories(${PROJECT_SOURCE_DIR}/threadpool/)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/${OUTPUT_PATH}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${OUTPUT_PATH}/)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${OUTPUT_PATH}/)

link_directories(${PROJECT_SOURCE_DIR}/lib/${OUTPUT_PATH}/)

add_subdirectory(${PROJECT_SOURCE_DIR}/coroutine/)

if(${SAMPLE} STREQUAL "yes")
    add_subdirectory(${PROJECT_SOURCE_DIR}/sample/)
endif()

if(${TEST} STREQUAL "yes")
    add_subdirectory(${PROJECT_SOURCE_DIR}/test/)
endif()
