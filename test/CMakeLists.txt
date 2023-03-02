include_directories({PROJECT_SOURCE_DIR}/test/googletest-1.13.0/)

project(test)

aux_source_directory(. SRC_DIR)

add_executable(${PROJECT_NAME} SRC_DIR)