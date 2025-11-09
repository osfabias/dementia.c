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
  @file src/memory.c
  @brief Memory management functions implementation
  @author Ilya Buravov (ilburale@gmail.com)
*/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dementia/memory.h"
#include "dementia/metadata.h"

/*=============================================================================
   INTERNAL STRUCTURES
 *=============================================================================*/

/*
  @brief Internal structure that stores metadata before user's memory block
  @details This structure is placed before the user's allocated memory block.
           The layout in memory is: [MemoryBlockHeader][User's memory block]
*/
typedef struct
{
  MemoryMetadata metadata; /* Memory block metadata */
  size_t         size;     /* The size of the memory block */
} MemoryBlockHeader;

/*=============================================================================
   FUNCTIONS
 *=============================================================================*/

void *allocate_memory (const size_t size, const MemoryMetadata metadata)
{
  const size_t total_size = sizeof (MemoryBlockHeader) + size;

  MemoryBlockHeader *const header = (MemoryBlockHeader *)malloc (total_size);
  if (header == NULL) { return NULL; }

  memcpy (&header->metadata, &metadata, sizeof (MemoryMetadata));
  header->size = size;

  return (void *)(header + 1);
}

void *recollect (void *block, const size_t new_size)
{
  if (block == NULL) { return NULL; }

  MemoryBlockHeader *const old_header =
    (MemoryBlockHeader *)((char *)block - sizeof (MemoryBlockHeader));

  const size_t new_total_size = sizeof (MemoryBlockHeader) + new_size;

  MemoryBlockHeader *const new_header =
    (MemoryBlockHeader *)realloc (old_header, new_total_size);
  if (new_header == NULL) { return NULL; }

  new_header->size = new_size;

  return (void *)(new_header + 1);
}

void forget (void *block)
{
  if (block == NULL) { return; }

  MemoryBlockHeader *const header =
    (MemoryBlockHeader *)((char *)block - sizeof (MemoryBlockHeader));

  free (header);
}
