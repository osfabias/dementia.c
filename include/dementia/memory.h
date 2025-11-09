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
