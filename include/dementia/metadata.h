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
  @file include/dementia/metadata.h
  @brief Memory block metadata struct
  @author Ilya Buravov (ilburale@gmail.com)
*/

#pragma once

#include <stddef.h>
#include <stdint.h>

/* Metadata for a block of memory */
typedef struct
{
  /* Path to a file where a memory block allocated */
  const char *const origin_file_path;
  /* Name of a function where a memory block allocated */
  const char *const origin_function_name;
  /* Line number where a memory allocation function was called */
  const uint32_t origin_line_number;
  /*
    Unsigned integer number that can be used by the user to relate a block of
    memory to a particular part (module, subsystem) of the program
  */
  const uint32_t tag;
} MemoryMetadata;
