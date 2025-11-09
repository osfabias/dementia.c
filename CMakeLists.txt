cmake_minimum_required(VERSION 3.16)

#=============================================================================
# PROJECT CONFIGURATION
#=============================================================================
# Basic project setup, versioning, and testing framework initialization
project(
  "dementia.c"
  VERSION 0.1.0
  DESCRIPTION "Helps you to keep track of all your memory."
)

#-----------------------------------------------------------------------------
# BUILD MODE DETECTION
#-----------------------------------------------------------------------------
# Detect if this is a standalone build or being included as a subdirectory
string(
  COMPARE EQUAL
  "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}"
  DEMENTIA_IS_STANDALONE_BUILD
)


#-----------------------------------------------------------------------------
# COMPILER-SPECIFIC SETTINGS
#-----------------------------------------------------------------------------
# Configure compiler flags and options for different toolchains
set(DEMENTIA_DEFAULT_BUILD_TYPE "Debug")

if (MSVC)
  set(DEMENTIA_COMPILE_OPTIONS
    /W4
    # add new msvc compile options here...
  )
else()
  set(DEMENTIA_COMPILE_OPTIONS
    -Wall
    -Wextra
    -Wpedantic

    -Wno-gnu-zero-variadic-macro-arguments
    # add new clang or gnu compile options here...
  )
endif()

#-----------------------------------------------------------------------------
# CMAKE GLOBAL SETTINGS
#-----------------------------------------------------------------------------
# Set global CMake behavior, C standard, and build system options
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_USE_RELATIVE_PATHS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ${DEMENTIA_IS_STANDALONE_BUILD})

#-----------------------------------------------------------------------------
# PROJECT OPTIONS
#-----------------------------------------------------------------------------
# Include user-configurable build options
include(CMakeOptions.txt)

#-----------------------------------------------------------------------------
# BUILD TYPE CONFIGURATION
#-----------------------------------------------------------------------------
# Set default build type for standalone builds
if (DEMENTIA_IS_STANDALONE_BUILD AND CMAKE_BUILD_TYPE STREQUAL "")
  message("${PROJECT_NAME} | warn: Build type wasn't specified. Automatically set to \"${DEMENTIA_DEFAULT_BUILD_TYPE}\".")
  set(CMAKE_BUILD_TYPE ${DEMENTIA_DEFAULT_BUILD_TYPE})
endif()


#-----------------------------------------------------------------------------
# BUILD INFORMATION DISPLAY
#-----------------------------------------------------------------------------
# Print project information and configuration summary
include(CMakeInfo.txt)

#-----------------------------------------------------------------------------
# SOURCE FILES CONFIGURATION
#-----------------------------------------------------------------------------
# Define source files for the library
set(DEMENTIA_SOURCE_FILES
  src/memory.c
  # add new source files here...
)

#=============================================================================
# LIBRARY TARGET CREATION
#=============================================================================
# Create the main library target with appropriate configuration
if(DEMENTIA_BUILD_SHARED)
  add_library(dementia SHARED ${DEMENTIA_SOURCE_FILES})
  target_compile_definitions(dementia PUBLIC __DEMENTIA_SHARED__)
  target_compile_definitions(dementia PRIVATE __DEMENTIA_EXPORT__)
else()
  add_library(dementia STATIC ${DEMENTIA_SOURCE_FILES})
endif()

target_include_directories(dementia PUBLIC include)
target_include_directories(dementia PRIVATE .)

target_compile_options(dementia PRIVATE ${DEMENTIA_COMPILE_OPTIONS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(dementia PUBLIC __DEMENTIA_DEBUG__)
endif()

if(DEMENTIA_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

if(DEMENTIA_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
