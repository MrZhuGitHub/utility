project(SAMPLE)

add_executable(basedCoroutine basedCoroutine.cc)
target_link_libraries(basedCoroutine ${CO_LIB})

add_executable(swapContext swapContext.cc)