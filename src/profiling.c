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
  @file src/profiling.c
  @brief Profiling functions implementation
  @author Ilya Buravov (ilburale@gmail.com)
*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dementia/metadata.h"
#include "dementia/profiling.h"

#include "src/profiling.h"
#include "src/terminal_styles.h"

/*=============================================================================
   INTERNAL STRUCTURES
 *=============================================================================*/

/*
  @brief Node in the linked list of allocated memory blocks
*/
typedef struct AllocationNode
{
  const MemoryMetadata   metadata;   /* Memory block metadata */
  size_t                 size;       /* Size of the allocated block */
  const void            *user_block; /* Pointer to user's memory block */
  struct AllocationNode *next;       /* Next node in the list */
} AllocationNode;

/*
  @brief Static state structure to track all memory allocations
*/
typedef struct
{
  AllocationNode *head;         /* Head of the linked list of allocations */
  size_t          total_blocks; /* Total number of allocated blocks */
  size_t          total_size;   /* Total size of all allocated blocks */
  size_t          tag_sizes[ MAX_MEMORY_TAG_VALUE + 1 ]; /* Sizes allocated by each tag */
} AllocationState;

/*=============================================================================
   STATIC STATE
 *=============================================================================*/

static AllocationState g_allocation_state = {0};

/*=============================================================================
   PUBLIC FUNCTIONS
 *=============================================================================*/

size_t get_memory_usage_by_tag (const uint32_t tag)
{
  if (tag > MAX_MEMORY_TAG_VALUE) { return 0; }

  return g_allocation_state.tag_sizes[ tag ];
}

size_t get_total_memory_usage (void) { return g_allocation_state.total_size; }

size_t print_allocated_memory_blocks_info (const PrintBlocksInfoArgs *const args)
{
  if (args == NULL || args->out == NULL || g_allocation_state.head == NULL) { return 0; }

  /* Format strings - kept statically inside the function */
  static const char *const FORMAT_COLORIZED =
    BOLD FG_CYAN "#%zu" STYLE_RESET " block " DIM "%p" STYLE_RESET ":\n"
                 "  tag: " FG_YELLOW "%u" STYLE_RESET "\n"
                 "  size: " FG_GREEN "%zu" STYLE_RESET " bytes\n"
                 "  origin: " FG_BLUE "%s" STYLE_RESET "()"
                 " at " FG_MAGENTA "%s" STYLE_RESET ":" DIM "%u" STYLE_RESET "\n";

  static const char *const FORMAT_PLAIN = "#%zu block %p:\n"
                                          "  tag: %u\n"
                                          "  size: %zu bytes\n"
                                          "  origin: %s() at %s:%u\n";

  const char *const format = args->colorize ? FORMAT_COLORIZED : FORMAT_PLAIN;

  size_t                printed = 0;
  const AllocationNode *current = g_allocation_state.head;

  while (current != NULL && printed < args->max_count)
  {
    fprintf (
      args->out, format, printed + 1, current->user_block, current->metadata.tag,
      current->size, current->metadata.origin_function_name,
      current->metadata.origin_file_path, current->metadata.origin_line_number
    );

    printed++;
    current = current->next;
  }

  return g_allocation_state.total_blocks;
}

/*=============================================================================
   INTERNAL FUNCTIONS
 *=============================================================================*/

/*
  @brief Registers a new memory allocation
  @param user_block Pointer to the user's memory block
  @param metadata Memory block metadata
  @param size Size of the allocated block
*/
void profiling_register_allocation (
  const void *const user_block, const MemoryMetadata *const metadata, const size_t size
)
{
  /* Create node on stack first */
  const AllocationNode temp_node = {
    .metadata = *metadata, .size = size, .user_block = user_block, .next = NULL
  };

  /* Allocate space for node on heap */
  AllocationNode *const node = (AllocationNode *)malloc (sizeof (AllocationNode));
  if (node == NULL) { return; }

  /* Copy node from stack to heap */
  memcpy (node, &temp_node, sizeof (AllocationNode));
  node->next = g_allocation_state.head;

  g_allocation_state.head = node;
  g_allocation_state.total_blocks++;
  g_allocation_state.total_size += size;

  /* Update tag-based size tracking */
  if (metadata->tag <= MAX_MEMORY_TAG_VALUE)
  {
    g_allocation_state.tag_sizes[ metadata->tag ] += size;
  }
}

/*
  @brief Unregisters a memory allocation
  @param user_block Pointer to the user's memory block to unregister
*/
void profiling_unregister_allocation (const void *const user_block)
{
  if (g_allocation_state.head == NULL) { return; }

  /* Check if the head node matches */
  if (g_allocation_state.head->user_block == user_block)
  {
    AllocationNode *const to_remove = g_allocation_state.head;
    g_allocation_state.head         = to_remove->next;

    /* Update total size and tag-based size tracking */
    g_allocation_state.total_size -= to_remove->size;
    if (to_remove->metadata.tag <= MAX_MEMORY_TAG_VALUE)
    {
      g_allocation_state.tag_sizes[ to_remove->metadata.tag ] -= to_remove->size;
    }

    free (to_remove);
    g_allocation_state.total_blocks--;
    return;
  }

  /* Search for the node in the list */
  AllocationNode *current = g_allocation_state.head;
  while (current->next != NULL)
  {
    if (current->next->user_block == user_block)
    {
      AllocationNode *const to_remove = current->next;
      current->next                   = to_remove->next;

      /* Update total size and tag-based size tracking */
      g_allocation_state.total_size -= to_remove->size;
      if (to_remove->metadata.tag <= MAX_MEMORY_TAG_VALUE)
      {
        g_allocation_state.tag_sizes[ to_remove->metadata.tag ] -= to_remove->size;
      }

      free (to_remove);
      g_allocation_state.total_blocks--;
      return;
    }
    current = current->next;
  }
}

/*
  @brief Updates the size and pointer of an existing allocation
  @param args Arguments containing old_user_block, new_user_block, and new_size
*/
void profiling_update_allocation (const AllocationUpdateArgs *const args)
{
  if (args == NULL) { return; }

  AllocationNode *current = g_allocation_state.head;
  while (current != NULL)
  {
    if (current->user_block == args->old_user_block)
    {
      const size_t old_size = current->size;
      current->user_block   = args->new_user_block;
      current->size         = args->new_size;

      /* Update total size and tag-based size tracking */
      g_allocation_state.total_size -= old_size;
      g_allocation_state.total_size += args->new_size;
      if (current->metadata.tag <= MAX_MEMORY_TAG_VALUE)
      {
        g_allocation_state.tag_sizes[ current->metadata.tag ] -= old_size;
        g_allocation_state.tag_sizes[ current->metadata.tag ] += args->new_size;
      }

      return;
    }
    current = current->next;
  }
}
