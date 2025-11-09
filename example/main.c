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
  @file example/main.c
  @brief Example program demonstrating library usage
  @author Ilya Buravov (ilburale@gmail.com)
  @details This file provides a simple example of how to use the library.
          It demonstrates basic library initialization and function calls.
          This serves as a template for users to understand library usage.
*/

#include <stdio.h>
#include <stdlib.h>

#include <dementia/memory.h>

int main (void)
{
  // Allocate memory block using `remember` macro
  const int number_count = 10;
  int      *numbers      = remember (sizeof (int) * number_count, 1);

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
