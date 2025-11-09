#include <stdio.h>
#include <stdlib.h>

#include <dementia/memory.h>

int main (void)
{
  // Allocate memory block using `remember` macro
  const int number_count = 10;
  int      *numbers      = remember (sizeof (int) * number_count, 1);

  if (numbers == NULL)
  {
    fputs ("Failed to allocate memory\n", stderr);
    return EXIT_FAILURE;
  }

  // Fill memory with some data
  for (int i = 0; i < number_count; ++i) { numbers[ i ] = i * 2; }

  // Print the data kept in memory
  fputs ("Saved data: [", stdout);
  for (int i = 0; i < number_count; ++i)
  {
    printf ("%d%s", numbers[ i ], i + 1 < number_count ? ", " : "");
  }
  puts ("]");

  // Free allocated memory block using `forget` function
  forget (numbers);

  return EXIT_SUCCESS;
}
