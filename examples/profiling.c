#include <stdio.h>
#include <stdlib.h>

#include <dementia/memory.h>
#include <dementia/profiling.h>

int main (void)
{
  // Allocate memory blocks with different tags
  const int tag1_size = 100;
  const int tag2_size = 200;
  const int tag3_size = 50;

  int *block1 = remember (sizeof (int) * tag1_size, 1);
  if (block1 == NULL)
  {
    fputs ("Failed to allocate memory for block1\n", stderr);
    return EXIT_FAILURE;
  }

  int *block2 = remember (sizeof (int) * tag2_size, 2);
  if (block2 == NULL)
  {
    fputs ("Failed to allocate memory for block2\n", stderr);
    forget (block1);
    return EXIT_FAILURE;
  }

  int *block3 = remember (sizeof (int) * tag3_size, 1);
  if (block3 == NULL)
  {
    fputs ("Failed to allocate memory for block3\n", stderr);
    forget (block1);
    forget (block2);
    return EXIT_FAILURE;
  }

  // Display memory usage statistics
  puts ("=== Memory Usage Statistics ===\n");

  const size_t total_usage = get_total_memory_usage ( );
  printf ("Total memory allocated: %zu bytes\n\n", total_usage);

  const size_t tag1_usage = get_memory_usage_by_tag (1);
  printf ("Memory allocated with tag 1: %zu bytes\n", tag1_usage);

  const size_t tag2_usage = get_memory_usage_by_tag (2);
  printf ("Memory allocated with tag 2: %zu bytes\n", tag2_usage);

  const size_t tag3_usage = get_memory_usage_by_tag (3);
  printf ("Memory allocated with tag 3: %zu bytes\n\n", tag3_usage);

  // Display detailed information about allocated blocks
  puts ("=== Allocated Memory Blocks Info ===\n");
  const PrintBlocksInfoArgs print_args = {.max_count = 10, .out = stdout, .colorize = 1};
  const size_t block_count             = print_allocated_memory_blocks_info (&print_args);
  printf ("\nTotal blocks allocated: %zu\n\n", block_count);

  // Free one block and show updated statistics
  puts ("=== After freeing block2 ===\n");
  forget (block2);

  const size_t total_after_free = get_total_memory_usage ( );
  printf ("Total memory allocated: %zu bytes\n", total_after_free);

  const size_t tag2_after_free = get_memory_usage_by_tag (2);
  printf ("Memory allocated with tag 2: %zu bytes\n\n", tag2_after_free);

  // Display remaining blocks
  puts ("=== Remaining Memory Blocks Info ===\n");
  const PrintBlocksInfoArgs remaining_args = {
    .max_count = 10, .out = stdout, .colorize = 1
  };
  const size_t remaining_count = print_allocated_memory_blocks_info (&remaining_args);
  printf ("\nRemaining blocks: %zu\n", remaining_count);

  // Clean up remaining allocations
  forget (block1);
  forget (block3);

  return EXIT_SUCCESS;
}
