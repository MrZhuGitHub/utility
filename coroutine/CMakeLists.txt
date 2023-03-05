project(${CO_LIB})

aux_source_directory(. SRC_DIR)

add_library(${CO_LIB} SHARED ${SRC_DIR})

install(TARGETS ${CO_LIB}
    LIBRARY DESTINATION /usr/lib/
)