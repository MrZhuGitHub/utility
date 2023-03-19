project(${TESTS})

add_subdirectory(./googletest/)

include_directories({PROJECT_SOURCE_DIR}/test/googletest/)

aux_source_directory(. SRC_DIR)

message("test")

add_executable(${TESTS} ${SRC_DIR})
target_link_libraries(${TESTS} gtest)