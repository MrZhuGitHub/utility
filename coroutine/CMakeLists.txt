project(${COROUTINE})

aux_source_directory(. SRC_DIR)

add_library(${COROUTINE} SHARED ${SRC_DIR})

install(TARGETS ${COROUTINE}
    LIBRARY DESTINATION /usr/lib/
)