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
  @file tests/test_memory.cpp
  @brief Tests for memory management functions
  @author Ilya Buravov (ilburale@gmail.com)
*/

#include <cstring>
#include <gtest/gtest.h>
#include <stdint.h>

#include "dementia/memory.h"
#include "dementia/metadata.h"
#include "dementia/profiling.h"

class MemoryTest : public ::testing::Test
{
protected:
  void TearDown ( ) override
  {
    // Verify no memory leaks (total usage should be 0)
    EXPECT_EQ (get_total_memory_usage ( ), 0)
      << "Memory leak detected: some memory blocks were not freed";
  }
};

TEST_F (MemoryTest, AllocateMemory_Success)
{
  uint8_t *const block = (uint8_t *)remember (sizeof (uint8_t), 1);
  ASSERT_NE (block, nullptr) << "remember should return non-null pointer";

  // Verify memory is writable
  memset (block, 0xAA, sizeof (uint8_t));
  EXPECT_EQ (block[ 0 ], 0xAA) << "Memory should be writable";

  // Verify profiling tracked the allocation
  EXPECT_EQ (get_total_memory_usage ( ), sizeof (uint8_t));
  EXPECT_EQ (get_memory_usage_by_tag (1), sizeof (uint8_t));

  forget (block);
}

TEST_F (MemoryTest, AllocateMemory_ZeroSize)
{
  const void *const block = remember (0, 0);
  EXPECT_EQ (block, nullptr) << "On 0 bytes allocation function should return NULL";
}

TEST_F (MemoryTest, Recollect_Success_Expand)
{
  const size_t initial_size = 100;
  const size_t new_size     = 200;
  void        *block        = remember (initial_size, 4);
  ASSERT_NE (block, nullptr);

  // Write some data
  memset (block, 0xAA, initial_size);

  void *new_block = recollect (block, new_size);
  ASSERT_NE (new_block, nullptr) << "recollect should return non-null pointer";

  // Verify old data is preserved (at least the initial portion)
  const uint8_t *const bytes = (const uint8_t *)new_block;
  for (size_t i = 0; i < initial_size; ++i)
  {
    EXPECT_EQ (bytes[ i ], 0xAA) << "Old data should be preserved";
  }

  // Verify new size is tracked
  EXPECT_EQ (get_total_memory_usage ( ), new_size);
  EXPECT_EQ (get_memory_usage_by_tag (4), new_size);

  forget (new_block);
}

TEST_F (MemoryTest, Recollect_Success_Shrink)
{
  const size_t initial_size = 200;
  const size_t new_size     = 100;
  void        *block        = remember (initial_size, 5);
  ASSERT_NE (block, nullptr);

  // Write some data
  memset (block, 0xBB, initial_size);

  void *new_block = recollect (block, new_size);
  ASSERT_NE (new_block, nullptr);

  // Verify old data is preserved (at least the portion that fits)
  const uint8_t *const bytes = (const uint8_t *)new_block;
  for (size_t i = 0; i < new_size; ++i)
  {
    EXPECT_EQ (bytes[ i ], 0xBB) << "Old data should be preserved";
  }

  // Verify new size is tracked
  EXPECT_EQ (get_total_memory_usage ( ), new_size);
  EXPECT_EQ (get_memory_usage_by_tag (5), new_size);

  forget (new_block);
}

TEST_F (MemoryTest, Recollect_SameSize)
{
  const size_t size  = 100;
  void        *block = remember (size, 6);
  ASSERT_NE (block, nullptr);

  void *new_block = recollect (block, size);
  ASSERT_NE (new_block, nullptr);

  // Size should remain the same
  EXPECT_EQ (get_total_memory_usage ( ), size);
  EXPECT_EQ (get_memory_usage_by_tag (6), size);

  forget (new_block);
}

TEST_F (MemoryTest, Recollect_NullBlock)
{
  const void *const result = recollect (nullptr, 100);
  EXPECT_EQ (result, nullptr) << "recollect with NULL should return NULL";
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (MemoryTest, Recollect_ZeroSize)
{
  const size_t initial_size = 100;
  void        *block        = remember (initial_size, 7);
  ASSERT_NE (block, nullptr);

  const void *const new_block = recollect (block, 0);
  EXPECT_EQ (new_block, nullptr) << "recollect with zero size should return NULL";
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (MemoryTest, Forget_Success)
{
  const size_t size  = 100;
  void        *block = remember (size, 9);
  ASSERT_NE (block, nullptr);
  EXPECT_EQ (get_total_memory_usage ( ), size);

  forget (block);
  EXPECT_EQ (get_total_memory_usage ( ), 0) << "Memory should be freed and untracked";
  EXPECT_EQ (get_memory_usage_by_tag (9), 0);
}

TEST_F (MemoryTest, Forget_NullBlock)
{
  forget (nullptr);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (MemoryTest, Integration_AllocateRecollectForget)
{
  const size_t size1 = 100;
  const size_t size2 = 200;
  const size_t size3 = 150;

  // Allocate
  void *block = remember (size1, 19);
  ASSERT_NE (block, nullptr);
  EXPECT_EQ (get_total_memory_usage ( ), size1);

  // Recollect (expand)
  block = recollect (block, size2);
  ASSERT_NE (block, nullptr);
  EXPECT_EQ (get_total_memory_usage ( ), size2);

  // Recollect (shrink)
  block = recollect (block, size3);
  ASSERT_NE (block, nullptr);
  EXPECT_EQ (get_total_memory_usage ( ), size3);

  // Forget
  forget (block);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}

TEST_F (MemoryTest, Integration_MultipleTags)
{
  const size_t size = 50;

  void *block1 = remember (size, 20);
  void *block2 = remember (size, 21);
  void *block3 = remember (size, 20);  // Same as first

  ASSERT_NE (block1, nullptr);
  ASSERT_NE (block2, nullptr);
  ASSERT_NE (block3, nullptr);

  EXPECT_EQ (get_total_memory_usage ( ), size * 3);
  EXPECT_EQ (get_memory_usage_by_tag (20), size * 2);
  EXPECT_EQ (get_memory_usage_by_tag (21), size);

  forget (block1);
  EXPECT_EQ (get_memory_usage_by_tag (20), size);

  forget (block2);
  forget (block3);
  EXPECT_EQ (get_total_memory_usage ( ), 0);
}
