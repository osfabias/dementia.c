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
  @file include/dementia/profiling.h
  @brief Profiling functions
  @author Ilya Buravov (ilburale@gmail.com)
*/

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "dementia/apidef.h"

/*=============================================================================
   STRUCTURES
 *=============================================================================*/

/*
  @brief Arguments for printing allocated memory blocks information
*/
typedef struct
{
  const size_t max_count; /* Maximum number of entries to print */
  FILE *const  out;       /* Output file stream to print information to */
  const bool   colorize;  /* Whether to colorize the output */
} PrintBlocksInfoArgs;

/*=============================================================================
   FUNCTIONS
 *=============================================================================*/

/*
  @brief Returns the number of bytes allocated with the specified tag
  @param tag Numeric tag to filter memory blocks by
  @return Number of bytes allocated with the specified tag
*/
__DEMENTIA_API__ size_t get_memory_usage_by_tag (uint32_t tag);

/*
  @brief Returns the total number of bytes currently allocated by the library
  @return Total number of bytes currently allocated by the library
*/
__DEMENTIA_API__ size_t get_total_memory_usage (void);

/*
  @brief Prints metadata for each memory block currently allocated by the library
  @param args Arguments containing max_count, out, and colorize
  @return Number of memory blocks currently allocated
  @note If there are no memory blocks allocated by the library, this function
        does nothing and returns 0
*/
__DEMENTIA_API__ size_t
print_allocated_memory_blocks_info (const PrintBlocksInfoArgs *args);
