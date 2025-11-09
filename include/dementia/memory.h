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
  @file include/dementia/memory.h
  @brief Memory management functions
  @author Ilya Buravov (ilburale@gmail.com)
*/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "dementia/apidef.h"
#include "dementia/metadata.h"

/*=============================================================================
   FUNCTIONS
 *=============================================================================*/

/*
  @brief Allocates a block of memory
  @param size Size of a memory block
  @param metadata Memory block metadata to tie to the memory block
  @return On success returns a pointer to a memory block of the requested size,
          otherwise returns `NULL`.
  @note As a library user you shouldn't use this function at all. It is better
        to use @ref remember macro with automatic metadata creation.
*/
__DEMENTIA_API__ void *allocate_memory (size_t size, MemoryMetadata metadata);

/*
  @brief Reallocates block of memory to fit the desired size
  @param block Memory block to resize
  @param new_size Size to resize a memory block to
  @return On success returns a pointer to a memory block of the requested size,
          otherwise returns `NULL`. If function fails, the original memory block
          stays in memory untouched.
  @warning If passed memory block wasn't previously returned
           by the @ref allocate_memory function or @ref remember macro
           the behaviour is undefined.
*/
__DEMENTIA_API__ void *recollect (void *block, size_t new_size);

/*
  @brief Frees allocated block of memory
  @param block Memory block to free
  @warning If passed memory block wasn't previously returned
           by the @ref allocate_memory function or @ref remember macro
           the behaviour is undefined.
*/
__DEMENTIA_API__ void forget (void *block);

/*=============================================================================
   MACROS
 *=============================================================================*/

/*
  @brief Allocates a block of memory
  @param _size Size of a memory block
  @param _tag A numeric tag of a memory block
  @return On success returns a pointer to a memory block of the requested size,
          otherwise returns `NULL`.
*/
#define remember(_size, _tag)                                      \
  allocate_memory (                                                \
    _size, (MemoryMetadata) {.origin_file_path     = __FILE__,     \
                             .origin_line_number   = __LINE__,     \
                             .origin_function_name = __FUNCTION__, \
                             .tag                  = _tag}         \
  )\

/*=============================================================================
   UTILITY FUNCTIONS
 *=============================================================================*/

/*
  @brief Prints an array of integers to stdout
  @param array Pointer to the array of integers
  @param count Number of elements in the array
  @details Prints the array in the format: [value1, value2, ..., valueN]
*/
static inline void
print_int_array (const int *const array, const size_t count)
{
  if (array == NULL) { return; }

  fputs ("[", stdout);
  for (size_t i = 0; i < count; ++i)
  {
    printf ("%d%s", array[ i ], i + 1 < count ? ", " : "");
  }
  fputs ("]", stdout);
}
