project(${TESTS})

add_subdirectory(./googletest-1.13.0/)

include_directories({PROJECT_SOURCE_DIR}/test/googletest-1.13.0/)

aux_source_directory(. SRC_DIR)

message("test")

add_executable(${TESTS} ${SRC_DIR})
target_link_libraries(${TESTS} gtest)