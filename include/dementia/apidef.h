/*
  Copyright 2025 Osfabias

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

/*
  @file include/dementia/apidef.h
  @brief API definition macros for cross-platform compatibility
  @author Ilya Buravov (ilburale@gmail.com)
  @details This header file defines macros for proper function export/import
           declarations across different compilers and platforms. It also
           provides C++ compatibility through extern "C" linkage.
*/

#pragma once

/*=============================================================================
   EXPORT/IMPORT DECLARATION MACROS
 *=============================================================================*/

/*
  @brief Internal macro for Windows DLL export/import declarations
  @details This macro handles the Windows-specific __declspec(dllexport/dllimport)
           declarations for shared libraries. It is automatically defined based
           on whether the library is being built or consumed.

  @note This macro is internal and should not be used directly in user code.
        Use @ref __DEMENTIA_API__ instead.

  @par Behavior:
  - When building shared library on Windows: expands to __declspec(dllexport)
  - When consuming shared library on Windows: expands to __declspec(dllimport)
  - On other platforms or static libraries: expands to nothing

  @par Example:
  @code
    // When building the library:
    __DEMENTIA_DECLSPEC__ int my_function(void);
    // Expands to: __declspec(dllexport) int my_function(void);

    // When using the library:
    __DEMENTIA_DECLSPEC__ int my_function(void);
    // Expands to: __declspec(dllimport) int my_function(void);
  @endcode
*/
#if defined(__DEMENTIA_SHARED__) && defined(_MSC_VER)
#  ifdef __DEMENTIA_EXPORT__
#    define __DEMENTIA_DECLSPEC__ __declspec (dllexport)
#  else
#    define __DEMENTIA_DECLSPEC__ __declspec (dllimport)
#  endif
#else
#  define __DEMENTIA_DECLSPEC__
#endif

/*
  @brief C++ linkage specification macro
  @details This macro provides proper C linkage when the header is included
           in C++ code, ensuring functions can be called from C++ without
           name mangling issues.

  @note This macro is internal and should not be used directly in user code.
        Use @ref __DEMENTIA_API__ instead.

  @par Behavior:
  - When included in C++ code: expands to extern "C"
  - When included in C code: expands to extern

  @par Example:
  @code
    // In C++ code:
    __DEMENTIA_EXTERN__ int my_function(void);
    // Expands to: extern "C" int my_function(void);

    // In C code:
    __DEMENTIA_EXTERN__ int my_function(void);
    // Expands to: extern int my_function(void);
  @endcode
*/
#ifdef __cplusplus
#  define __DEMENTIA_EXTERN__ extern "C"
#else
#  define __DEMENTIA_EXTERN__ extern
#endif

/*=============================================================================
 * PUBLIC API MACRO
 *=============================================================================*/

/*
  @brief Main API declaration macro for library functions
  @details This is the primary macro that should be used to declare all
           public library functions. It automatically handles:
           - Windows DLL export/import declarations
           - C++ compatibility through extern "C" linkage
           - Cross-platform compatibility

  @par Usage:
  Place this macro before the return type of any function that should be
  part of the public library API.

  @par Example:
  @code
    // Function declaration in header file:
    __DEMENTIA_API__ int dementia_init(void);
    __DEMENTIA_API__ int dementia_process_data(const void* data, size_t size);

    // Function definition in source file:
    __DEMENTIA_API__ int dementia_init(void) {
        return 0;
    }
  @endcode

  @note This macro should be used for all public API functions.
  @note The macro automatically handles C++ compatibility.

  @see __DEMENTIA_DECLSPEC__ for export/import details
  @see __DEMENTIA_EXTERN__ for C++ linkage details
*/
#define __DEMENTIA_API__ __DEMENTIA_DECLSPEC__ __DEMENTIA_EXTERN__
