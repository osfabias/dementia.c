add_executable(remember_and_forget remember_and_forget.c)
target_link_libraries(remember_and_forget PRIVATE dementia)
target_compile_options(remember_and_forget PRIVATE ${DEMENTIA_COMPILE_OPTIONS})

add_executable(recollect recollect.c)
target_link_libraries(recollect PRIVATE dementia)
target_compile_options(recollect PRIVATE ${DEMENTIA_COMPILE_OPTIONS})

add_executable(profiling profiling.c)
target_link_libraries(profiling PRIVATE dementia)
target_compile_options(profiling PRIVATE ${DEMENTIA_COMPILE_OPTIONS})

add_executable(benchmark benchmark.c)
target_link_libraries(benchmark PRIVATE dementia)
target_compile_options(benchmark PRIVATE ${DEMENTIA_COMPILE_OPTIONS})
# Link rt library for clock_gettime on Linux (not needed on macOS)
if(UNIX AND NOT APPLE)
  target_link_libraries(benchmark PRIVATE rt)
endif()