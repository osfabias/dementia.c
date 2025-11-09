#pragma once

#include <stddef.h>
#include <stdio.h>

/*
  @brief Prints an array of integers to stdout
  @param array Pointer to the array of integers
  @param count Number of elements in the array
  @details Prints the array in the format: [value1, value2, ..., valueN]
*/
inline static void print_int_array (const int *const array, const size_t count)
{
  if (array == NULL) { return; }

  fputs ("[", stdout);
  for (size_t i = 0; i < count; ++i)
  {
    printf ("%d%s", array[ i ], i + 1 < count ? ", " : "");
  }
  fputs ("]", stdout);
}
