#include <stdio.h>
#include <stdlib.h>

#include <dementia/memory.h>

#include "utils.h"

int main (void)
{
  // Allocate initial memory block using `remember` macro
  const int initial_count = 5;
  int      *numbers       = remember (sizeof (int) * initial_count, 2);

  if (numbers == NULL)
  {
    fputs ("Failed to allocate memory\n", stderr);
    return EXIT_FAILURE;
  }

  // Fill initial memory with some data
  for (int i = 0; i < initial_count; ++i) { numbers[ i ] = i * 10; }

  // Print initial allocation using the utility function
  fputs ("Initial allocation: ", stdout);
  print_int_array (numbers, initial_count);
  putchar ('\n');

  // Reallocate memory to a larger size using `recollect` function
  const int bigger_count = 10;
  numbers                = recollect (numbers, sizeof (int) * bigger_count);

  if (numbers == NULL)
  {
    fputs ("Failed to reallocate memory\n", stderr);
    return EXIT_FAILURE;
  }

  // Fill the newly allocated space with additional data
  puts ("\nAfter reallocation (larger size):");
  fputs ("All data: ", stdout);
  print_int_array (numbers, bigger_count);
  putchar ('\n');

  // Reallocate to a smaller size
  const int smaller_count = 3;
  numbers                 = recollect (numbers, sizeof (int) * smaller_count);

  if (numbers == NULL)
  {
    fputs ("Failed to reallocate memory\n", stderr);
    return EXIT_FAILURE;
  }

  puts ("\nAfter reallocation (smaller size):");
  fputs ("Remaining data: ", stdout);
  print_int_array (numbers, smaller_count);
  putchar ('\n');

  // Free allocated memory block using `forget` function
  forget (numbers);

  return EXIT_SUCCESS;
}
