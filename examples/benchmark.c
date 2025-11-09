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
  @file examples/benchmark.c
  @brief Benchmark comparing standard C memory functions with dementia library
  @author Ilya Buravov (ilburale@gmail.com)
*/

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <dementia/memory.h>

/*=============================================================================
   TIMING UTILITIES
 *=============================================================================*/

static double get_time_ns (void)
{
  struct timespec ts;
  clock_gettime (CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

static void print_time_diff (const double time_ns, const size_t iterations)
{
  const double avg_time_ns = time_ns / (double)iterations;
  const double avg_time_us = avg_time_ns / 1000.0;
  const double avg_time_ms = avg_time_us / 1000.0;

  if (avg_time_ns < 1000.0)
  {
    printf ("%.2f ns", avg_time_ns);
  }
  else if (avg_time_us < 1000.0)
  {
    printf ("%.2f us", avg_time_us);
  }
  else
  {
    printf ("%.2f ms", avg_time_ms);
  }
}

/*=============================================================================
   BENCHMARK FUNCTIONS
 *=============================================================================*/

static void benchmark_malloc_vs_remember (const size_t size, const size_t iterations)
{
  printf ("\n=== malloc vs remember (size: %zu bytes, iterations: %zu) ===\n", size,
          iterations);

  // Benchmark malloc
  const double malloc_start = get_time_ns ( );
  for (size_t i = 0; i < iterations; ++i)
  {
    void *ptr = malloc (size);
    if (ptr == NULL)
    {
      fputs ("malloc failed\n", stderr);
      return;
    }
    free (ptr);
  }
  const double malloc_end   = get_time_ns ( );
  const double malloc_total = malloc_end - malloc_start;

  // Benchmark remember
  const double remember_start = get_time_ns ( );
  for (size_t i = 0; i < iterations; ++i)
  {
    void *ptr = remember (size, 0);
    if (ptr == NULL)
    {
      fputs ("remember failed\n", stderr);
      return;
    }
    forget (ptr);
  }
  const double remember_end   = get_time_ns ( );
  const double remember_total = remember_end - remember_start;

  // Print results
  printf ("malloc:  ");
  print_time_diff (malloc_total, iterations);
  printf (" per operation\n");

  printf ("remember: ");
  print_time_diff (remember_total, iterations);
  printf (" per operation\n");

  const double overhead = remember_total - malloc_total;
  const double overhead_percent = (overhead / malloc_total) * 100.0;
  printf ("Overhead: ");
  print_time_diff (overhead, iterations);
  printf (" per operation (%.2f%%)\n", overhead_percent);
}

static void benchmark_free_vs_forget (const size_t size, const size_t iterations)
{
  printf ("\n=== free vs forget (size: %zu bytes, iterations: %zu) ===\n", size,
          iterations);

  // Pre-allocate blocks for free benchmark
  void **malloc_blocks = (void **)malloc (sizeof (void *) * iterations);
  if (malloc_blocks == NULL)
  {
    fputs ("Failed to allocate memory for malloc blocks\n", stderr);
    return;
  }

  for (size_t i = 0; i < iterations; ++i)
  {
    malloc_blocks[ i ] = malloc (size);
    if (malloc_blocks[ i ] == NULL)
    {
      fputs ("malloc failed\n", stderr);
      for (size_t j = 0; j < i; ++j) { free (malloc_blocks[ j ]); }
      free (malloc_blocks);
      return;
    }
  }

  // Benchmark free
  const double free_start = get_time_ns ( );
  for (size_t i = 0; i < iterations; ++i) { free (malloc_blocks[ i ]); }
  const double free_end   = get_time_ns ( );
  const double free_total = free_end - free_start;

  free (malloc_blocks);

  // Pre-allocate blocks for forget benchmark
  void **remember_blocks = (void **)malloc (sizeof (void *) * iterations);
  if (remember_blocks == NULL)
  {
    fputs ("Failed to allocate memory for remember blocks\n", stderr);
    return;
  }

  for (size_t i = 0; i < iterations; ++i)
  {
    remember_blocks[ i ] = remember (size, 0);
    if (remember_blocks[ i ] == NULL)
    {
      fputs ("remember failed\n", stderr);
      for (size_t j = 0; j < i; ++j) { forget (remember_blocks[ j ]); }
      free (remember_blocks);
      return;
    }
  }

  // Benchmark forget
  // Free in reverse order (LIFO) since remember adds to head of list
  // This makes each forget O(1) instead of O(n)
  const double forget_start = get_time_ns ( );
  for (size_t i = iterations; i > 0; --i) { forget (remember_blocks[ i - 1 ]); }
  const double forget_end   = get_time_ns ( );
  const double forget_total = forget_end - forget_start;

  free (remember_blocks);

  // Print results
  printf ("free:   ");
  print_time_diff (free_total, iterations);
  printf (" per operation\n");

  printf ("forget: ");
  print_time_diff (forget_total, iterations);
  printf (" per operation\n");

  const double overhead = forget_total - free_total;
  const double overhead_percent = (overhead / free_total) * 100.0;
  printf ("Overhead: ");
  print_time_diff (overhead, iterations);
  printf (" per operation (%.2f%%)\n", overhead_percent);
}

static void benchmark_realloc_vs_recollect (const size_t initial_size, const size_t new_size,
                                            const size_t iterations)
{
  printf ("\n=== realloc vs recollect (initial: %zu bytes, new: %zu bytes, iterations: %zu) "
          "===\n",
          initial_size, new_size, iterations);

  // Benchmark realloc
  const double realloc_start = get_time_ns ( );
  for (size_t i = 0; i < iterations; ++i)
  {
    void *ptr = malloc (initial_size);
    if (ptr == NULL)
    {
      fputs ("malloc failed\n", stderr);
      return;
    }

    void *new_ptr = realloc (ptr, new_size);
    if (new_ptr == NULL)
    {
      free (ptr);
      fputs ("realloc failed\n", stderr);
      return;
    }

    free (new_ptr);
  }
  const double realloc_end   = get_time_ns ( );
  const double realloc_total = realloc_end - realloc_start;

  // Benchmark recollect
  const double recollect_start = get_time_ns ( );
  for (size_t i = 0; i < iterations; ++i)
  {
    void *ptr = remember (initial_size, 0);
    if (ptr == NULL)
    {
      fputs ("remember failed\n", stderr);
      return;
    }

    void *new_ptr = recollect (ptr, new_size);
    if (new_ptr == NULL)
    {
      fputs ("recollect failed\n", stderr);
      return;
    }

    forget (new_ptr);
  }
  const double recollect_end   = get_time_ns ( );
  const double recollect_total = recollect_end - recollect_start;

  // Print results
  printf ("realloc:  ");
  print_time_diff (realloc_total, iterations);
  printf (" per operation\n");

  printf ("recollect: ");
  print_time_diff (recollect_total, iterations);
  printf (" per operation\n");

  const double overhead = recollect_total - realloc_total;
  const double overhead_percent = (overhead / realloc_total) * 100.0;
  printf ("Overhead: ");
  print_time_diff (overhead, iterations);
  printf (" per operation (%.2f%%)\n", overhead_percent);
}

/*=============================================================================
   MAIN
 *=============================================================================*/

int main (void)
{
  puts ("=== Dementia Library Benchmark ===\n");
  puts ("Comparing standard C memory functions with dementia library functions\n");

  const size_t small_size  = 64;
  const size_t medium_size = 1024;
  const size_t large_size  = 1024 * 1024;  // 1 MB

  const size_t iterations = 10000;

  // Benchmark malloc vs remember
  benchmark_malloc_vs_remember (small_size, iterations);
  benchmark_malloc_vs_remember (medium_size, iterations);
  benchmark_malloc_vs_remember (large_size, iterations / 10);  // Fewer iterations for large

  // Benchmark free vs forget
  benchmark_free_vs_forget (small_size, iterations);
  benchmark_free_vs_forget (medium_size, iterations);
  benchmark_free_vs_forget (large_size, iterations / 10);  // Fewer iterations for large

  // Benchmark realloc vs recollect
  benchmark_realloc_vs_recollect (small_size, medium_size, iterations);
  benchmark_realloc_vs_recollect (medium_size, large_size, iterations / 10);
  benchmark_realloc_vs_recollect (large_size, large_size / 2, iterations / 10);

  puts ("\n=== Benchmark Complete ===\n");

  return EXIT_SUCCESS;
}

