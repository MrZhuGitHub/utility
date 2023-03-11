project(${SAMPLES})

add_executable(basedCoroutine basedCoroutine.cc)
target_link_libraries(basedCoroutine ${COROUTINE})

add_executable(swapContext swapContext.cc)