project(${SAMPLES})

add_executable(client client.cc)
target_link_libraries(client ${COROUTINE})

add_executable(server server.cc)
target_link_libraries(server ${COROUTINE})

add_executable(basedCoroutine basedCoroutine.cc)
target_link_libraries(basedCoroutine ${COROUTINE})

add_executable(swapContext swapContext.cc)