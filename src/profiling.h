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
  @file src/profiling.h
  @brief Internal profiling function declarations
  @author Ilya Buravov (ilburale@gmail.com)
  @details This header contains internal function declarations for the profiling
           system. These functions are used internally by the memory management
           system and are not part of the public API.
*/

#pragma once

#include <stddef.h>

#include "dementia/metadata.h"

/*=============================================================================
   INTERNAL STRUCTURES
 *=============================================================================*/

/*
  @brief Arguments for updating an allocation
*/
typedef struct
{
  const void *old_user_block; /* Old pointer to the user's memory block */
  const void *new_user_block; /* New pointer to the user's memory block */
  size_t      new_size;       /* New size of the allocated block */
} AllocationUpdateArgs;

/*=============================================================================
   INTERNAL PROFILING FUNCTIONS
 *=============================================================================*/

/*
  @brief Registers a new memory allocation
  @param user_block Pointer to the user's memory block
  @param metadata Memory block metadata
  @param size Size of the allocated block
*/
void profiling_register_allocation (
  const void *user_block, const MemoryMetadata *metadata, size_t size
);

/*
  @brief Unregisters a memory allocation
  @param user_block Pointer to the user's memory block to unregister
*/
void profiling_unregister_allocation (const void *user_block);

/*
  @brief Updates the size and pointer of an existing allocation
  @param args Arguments containing old_user_block, new_user_block, and new_size
*/
void profiling_update_allocation (const AllocationUpdateArgs *args);
