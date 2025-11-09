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
  @file tests/test_profiling.cpp
  @brief Tests for profiling functions
  @author Ilya Buravov (ilburale@gmail.com)
*/

#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

#include "dementia/memory.h"
#include "dementia/metadata.h"
#include "dementia/profiling.h"

class ProfilingTest : public ::testing::Test
{
protected:
  void SetUp ( ) override
  {
    // Ensure clean state before each test
    // Note: We can't directly reset profiling state, but we can verify
    // that tests clean up after themselves
  }

  void TearDown ( ) override
  {
    // Verify no memory leaks
    EXPECT_EQ (get_total_memory_usage ( ), 0)
      << "Memory leak detected: some memory blocks were not freed";
  }
};

TEST_F (ProfilingTest, GetMemoryUsageByTag_EmptyState)
{
  EXPECT_EQ (get_memory_usage_by_tag (0), 0);
  EXPECT_EQ (get_memory_usage_by_tag (10), 0);
  EXPECT_EQ (get_memory_usage_by_tag (MAX_MEMORY_TAG_VALUE), 0);
}

TEST_F (ProfilingTest, GetMemoryUsageByTag_SingleAllocation)
{
  const size_t   size = 100;
  const uint32_t tag  = 1;

  void *block = remember (size, tag);
  ASSERT_NE (block, nullptr);

  EXPECT_EQ (get_memory_usage_by_tag (tag), size);
  EXPECT_EQ (get_memory_usage_by_tag (0), 0);
  EXPECT_EQ (get_memory_usage_by_tag (2), 0);

  forget (block);
}

TEST_F (ProfilingTest, GetMemoryUsageByTag_MultipleAllocationsSameTag)
{
  const size_t   size1 = 100;
  const size_t   size2 = 200;
  const size_t   size3 = 300;
  const uint32_t tag   = 2;

  void *block1 = remember (size1, tag);
  void *block2 = remember (size2, tag);
  void *block3 = remember (size3, tag);

  ASSERT_NE (block1, nullptr);
  ASSERT_NE (block2, nullptr);
  ASSERT_NE (block3, nullptr);

  EXPECT_EQ (get_memory_usage_by_tag (tag), size1 + size2 + size3);

  forget (block1);
  EXPECT_EQ (get_memory_usage_by_tag (tag), size2 + size3);

  forget (block2);
  EXPECT_EQ (get_memory_usage_by_tag (tag), size3);

  forget (block3);
  EXPECT_EQ (get_memory_usage_by_tag (tag), 0);
}

TEST_F (ProfilingTest, GetMemoryUsageByTag_MultipleAllocationsDifferentTags)
{
  const size_t   size = 50;
  const uint32_t tag1 = 10;
  const uint32_t tag2 = 20;
  const uint32_t tag3 = 30;

  void *block1 = remember (size, tag1);
  void *block2 = remember (size, tag2);
  void *block3 = remember (size, tag3);

  ASSERT_NE (block1, nullptr);
  ASSERT_NE (block2, nullptr);
  ASSERT_NE (block3, nullptr);

  EXPECT_EQ (get_memory_usage_by_tag (tag1), size);
  EXPECT_EQ (get_memory_usage_by_tag (tag2), size);
  EXPECT_EQ (get_memory_usage_by_tag (tag3), size);
  EXPECT_EQ (get_memory_usage_by_tag (0), 0);
  EXPECT_EQ (get_memory_usage_by_tag (15), 0);

  forget (block1);
  forget (block2);
  forget (block3);
}

TEST_F (ProfilingTest, GetMemoryUsageByTag_InvalidTag_TooLarge)
{
  const size_t   size        = 100;
  const uint32_t valid_tag   = 10;
  const uint32_t invalid_tag = MAX_MEMORY_TAG_VALUE + 1;

  void *block = remember (size, valid_tag);
  ASSERT_NE (block, nullptr);

  EXPECT_EQ (get_memory_usage_by_tag (valid_tag), size);
  EXPECT_EQ (get_memory_usage_by_tag (invalid_tag), 0)
    << "Invalid tag (>MAX_MEMORY_TAG_VALUE) should return 0";

  forget (block);
}

TEST_F (ProfilingTest, GetMemoryUsageByTag_MaxTagValue)
{
  const size_t   size = 100;
  const uint32_t tag  = MAX_MEMORY_TAG_VALUE;

  void *block = remember (size, tag);
  ASSERT_NE (block, nullptr);

  EXPECT_EQ (get_memory_usage_by_tag (tag), size);

  forget (block);
}

TEST_F (ProfilingTest, GetMemoryUsageByTag_AfterRecollect)
{
  const size_t   initial_size = 100;
  const size_t   new_size     = 200;
  const uint32_t tag          = 5;

  void *block = remember (initial_size, tag);
  ASSERT_NE (block, nullptr);

  EXPECT_EQ (get_memory_usage_by_tag (tag), initial_size);

  void *new_block = recollect (block, new_size);
  ASSERT_NE (new_block, nullptr);

  EXPECT_EQ (get_memory_usage_by_tag (tag), new_size);

  forget (new_block);
}

TEST_F (ProfilingTest, GetTotalMemoryUsage_EmptyState)
{
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (ProfilingTest, GetTotalMemoryUsage_SingleAllocation)
{
  const size_t   size = 100;
  const uint32_t tag  = 6;

  void *block = remember (size, tag);
  ASSERT_NE (block, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), size);

  forget (block);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (ProfilingTest, GetTotalMemoryUsage_MultipleAllocations)
{
  const size_t size1 = 100;
  const size_t size2 = 200;
  const size_t size3 = 300;

  void *block1 = remember (size1, 7);
  void *block2 = remember (size2, 8);
  void *block3 = remember (size3, 9);

  ASSERT_NE (block1, nullptr);
  ASSERT_NE (block2, nullptr);
  ASSERT_NE (block3, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), size1 + size2 + size3);

  forget (block2);
  EXPECT_EQ (get_total_memory_usage ( ), size1 + size3);

  forget (block1);
  EXPECT_EQ (get_total_memory_usage ( ), size3);

  forget (block3);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (ProfilingTest, GetTotalMemoryUsage_AfterRecollect)
{
  const size_t   initial_size = 100;
  const size_t   new_size     = 250;
  const uint32_t tag          = 11;

  void *block = remember (initial_size, tag);
  ASSERT_NE (block, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), initial_size);

  void *new_block = recollect (block, new_size);
  ASSERT_NE (new_block, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), new_size);

  forget (new_block);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (ProfilingTest, GetTotalMemoryUsage_MultipleRecollects)
{
  const size_t   size1 = 100;
  const size_t   size2 = 200;
  const size_t   size3 = 150;
  const uint32_t tag   = 12;

  void *block = remember (size1, tag);
  ASSERT_NE (block, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), size1);

  block = recollect (block, size2);
  ASSERT_NE (block, nullptr);
  EXPECT_EQ (get_total_memory_usage ( ), size2);

  block = recollect (block, size3);
  ASSERT_NE (block, nullptr);
  EXPECT_EQ (get_total_memory_usage ( ), size3);

  forget (block);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_NullArgs)
{
  const size_t result = print_allocated_memory_blocks_info (nullptr);
  EXPECT_EQ (result, 0) << "Should return 0 when args is NULL";
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_NullOut)
{
  const PrintBlocksInfoArgs args = {.max_count = 10, .out = nullptr, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 0) << "Should return 0 when out is NULL";
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_EmptyState)
{
  const PrintBlocksInfoArgs args = {.max_count = 10, .out = stdout, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 0) << "Should return 0 when no blocks are allocated";
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_SingleBlock)
{
  const size_t   size = 100;
  const uint32_t tag  = 20;

  void *block = remember (size, tag);
  ASSERT_NE (block, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = stdout, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 1) << "Should return 1 when one block is allocated";

  forget (block);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_MultipleBlocks)
{
  const size_t size   = 50;
  void        *block1 = remember (size, 21);
  void        *block2 = remember (size, 22);
  void        *block3 = remember (size, 23);

  ASSERT_NE (block1, nullptr);
  ASSERT_NE (block2, nullptr);
  ASSERT_NE (block3, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = stdout, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 3) << "Should return 3 when three blocks are allocated";

  forget (block1);
  forget (block2);
  forget (block3);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_MaxCountLimit)
{
  const size_t size   = 50;
  void        *block1 = remember (size, 24);
  void        *block2 = remember (size, 25);
  void        *block3 = remember (size, 26);
  void        *block4 = remember (size, 27);
  void        *block5 = remember (size, 28);

  ASSERT_NE (block1, nullptr);
  ASSERT_NE (block2, nullptr);
  ASSERT_NE (block3, nullptr);
  ASSERT_NE (block4, nullptr);
  ASSERT_NE (block5, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 2, .out = stdout, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (
    result, 5
  ) << "Should return total count (5) even if max_count limits output";

  forget (block1);
  forget (block2);
  forget (block3);
  forget (block4);
  forget (block5);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_ZeroMaxCount)
{
  const size_t size  = 50;
  void        *block = remember (size, 29);
  ASSERT_NE (block, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 0, .out = stdout, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 1) << "Should return total count even if max_count is 0";

  forget (block);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_ColorizeTrue)
{
  const size_t size  = 50;
  void        *block = remember (size, 30);
  ASSERT_NE (block, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = stdout, .colorize = true};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 1);

  forget (block);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_ColorizeFalse)
{
  const size_t size  = 50;
  void        *block = remember (size, 31);
  ASSERT_NE (block, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = stdout, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 1);

  forget (block);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_ToFile)
{
  const size_t size  = 50;
  void        *block = remember (size, 32);
  ASSERT_NE (block, nullptr);

  FILE *file = tmpfile ( );
  ASSERT_NE (file, nullptr) << "Failed to create temporary file";

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = file, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 1);

  // Verify file was written to
  fseek (file, 0, SEEK_END);
  const long file_size = ftell (file);
  EXPECT_GT (file_size, 0) << "File should contain output";

  fclose (file);
  forget (block);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_VerifyOutputFormat)
{
  const size_t   size = 100;
  const uint32_t tag  = 33;

  void *block = remember (size, tag);
  ASSERT_NE (block, nullptr);

  FILE *file = tmpfile ( );
  ASSERT_NE (file, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = file, .colorize = false};

  const size_t result = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result, 1);

  // Read back the file
  rewind (file);
  char         buffer[ 1024 ];
  const size_t bytes_read = fread (buffer, 1, sizeof (buffer) - 1, file);
  buffer[ bytes_read ] = '\0';

  // Verify output contains expected fields
  std::string output (buffer);
  EXPECT_NE (output.find ("block"), std::string::npos) << "Output should contain 'block'";
  EXPECT_NE (output.find ("tag:"), std::string::npos) << "Output should contain 'tag:'";
  EXPECT_NE (output.find ("size:"), std::string::npos) << "Output should contain 'size:'";
  EXPECT_NE (output.find ("origin:"), std::string::npos)
    << "Output should contain 'origin:'";

  fclose (file);
  forget (block);
}

TEST_F (ProfilingTest, PrintAllocatedMemoryBlocksInfo_AfterForget)
{
  const size_t size   = 50;
  void        *block1 = remember (size, 34);
  void        *block2 = remember (size, 35);

  ASSERT_NE (block1, nullptr);
  ASSERT_NE (block2, nullptr);

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = stdout, .colorize = false};

  const size_t result1 = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result1, 2);

  forget (block1);

  const size_t result2 = print_allocated_memory_blocks_info (&args);
  EXPECT_EQ (result2, 1);

  forget (block2);
}

TEST_F (ProfilingTest, Integration_AllProfilingFunctions)
{
  const size_t   size1 = 100;
  const size_t   size2 = 200;
  const uint32_t tag1  = 40;
  const uint32_t tag2  = 41;

  // Initial state
  EXPECT_EQ (get_total_memory_usage ( ), 0);
  EXPECT_EQ (get_memory_usage_by_tag (tag1), 0);
  EXPECT_EQ (get_memory_usage_by_tag (tag2), 0);

  // Allocate first block
  void *block1 = remember (size1, tag1);
  ASSERT_NE (block1, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), size1);
  EXPECT_EQ (get_memory_usage_by_tag (tag1), size1);
  EXPECT_EQ (get_memory_usage_by_tag (tag2), 0);

  const PrintBlocksInfoArgs args = {.max_count = 10, .out = stdout, .colorize = false};
  EXPECT_EQ (print_allocated_memory_blocks_info (&args), 1);

  // Allocate second block
  void *block2 = remember (size2, tag2);
  ASSERT_NE (block2, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), size1 + size2);
  EXPECT_EQ (get_memory_usage_by_tag (tag1), size1);
  EXPECT_EQ (get_memory_usage_by_tag (tag2), size2);
  EXPECT_EQ (print_allocated_memory_blocks_info (&args), 2);

  // Recollect first block
  void *new_block1 = recollect (block1, size1 * 2);
  ASSERT_NE (new_block1, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), size1 * 2 + size2);
  EXPECT_EQ (get_memory_usage_by_tag (tag1), size1 * 2);
  EXPECT_EQ (get_memory_usage_by_tag (tag2), size2);
  EXPECT_EQ (print_allocated_memory_blocks_info (&args), 2);

  // Free blocks
  forget (new_block1);
  EXPECT_EQ (get_total_memory_usage ( ), size2);
  EXPECT_EQ (get_memory_usage_by_tag (tag1), 0);
  EXPECT_EQ (get_memory_usage_by_tag (tag2), size2);
  EXPECT_EQ (print_allocated_memory_blocks_info (&args), 1);

  forget (block2);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
  EXPECT_EQ (get_memory_usage_by_tag (tag1), 0);
  EXPECT_EQ (get_memory_usage_by_tag (tag2), 0);
  EXPECT_EQ (print_allocated_memory_blocks_info (&args), 0);
}
