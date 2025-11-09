add_executable(example main.c)
target_link_libraries(example PRIVATE dementia)
target_compile_options(example PRIVATE ${DEMENTIA_COMPILE_OPTIONS})