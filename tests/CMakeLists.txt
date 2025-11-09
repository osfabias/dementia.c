#=============================================================================
# TESTS CONFIGURATION
#=============================================================================

# Setup CMake options
set(CMAKE_CXX_STANDARD 17) # GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Find all test source files
file(GLOB TEST_SOURCES "*.cpp")

# Create one executable per test source and wire it up
set(_ALL_TEST_TARGETS "")
foreach(_SRC IN LISTS TEST_SOURCES)
  get_filename_component(_NAME "${_SRC}" NAME_WE)

  add_executable(${_NAME} "${_SRC}")

  # Link with the main library and Google Test using official targets
  target_link_libraries(${_NAME} PRIVATE
    dementia
    GTest::gtest_main
  )

  # Apply same compile options as the main library
  if(DEMENTIA_COMPILE_OPTIONS)
    target_compile_options(${_NAME} PRIVATE ${DEMENTIA_COMPILE_OPTIONS})
  endif()

  list(APPEND _ALL_TEST_TARGETS ${_NAME})
endforeach()

# Include Google Test module and discover tests automatically
include(GoogleTest)
foreach(_TGT IN LISTS _ALL_TEST_TARGETS)
  gtest_discover_tests(${_TGT})
endforeach()

# Enable code coverage for tests if requested
if(DEMENTIA_ENABLE_COVERAGE AND CMAKE_BUILD_TYPE STREQUAL "Debug")
  if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    foreach(_TGT IN LISTS _ALL_TEST_TARGETS)
      target_compile_options(${_TGT} PRIVATE --coverage)
      target_link_libraries(${_TGT} PRIVATE --coverage)
    endforeach()
    message(STATUS "Code coverage enabled for tests")
  endif()
endif()

