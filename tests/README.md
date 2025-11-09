# Testing

This directory contains the test suite for the library using Google Test framework.

## Running Tests

### Building and Running Tests

```bash
# Build with tests enabled
cmake -DDEMENTIA_BUILD_TESTS=ON ..
make

# Run tests using CTest (recommended)
ctest --verbose

# Or run tests directly with Google Test options
./test_dementia --gtest_verbose
./test_dementia --gtest_filter="*Basic*"  # Run only tests matching pattern
```

### Test Options

The project supports several CMake options for testing:

| Option | Default | Description |
|--------|---------|-------------|
| `DEMENTIA_BUILD_TESTS` | `ON` (standalone) | Build test programs |
| `DEMENTIA_ENABLE_COVERAGE` | `OFF` | Enable code coverage (requires Debug build) |

### Google Test Options

Google Test provides many command-line options for test execution:

```bash
# Run specific test suites
./test_dementia --gtest_filter="UdementiaBasicTest.*"

# Run tests with detailed output
./test_dementia --gtest_verbose

# Run tests in random order
./test_dementia --gtest_shuffle

# Stop on first failure
./test_dementia --gtest_break_on_failure

# List all available tests
./test_dementia --gtest_list_tests
```

### Code Coverage

When `DEMENTIA_ENABLE_COVERAGE=ON` and build type is `Debug`, code coverage is automatically enabled for tests.

## Test Structure

- `test_main.c` - Main test file with Google Test test cases
- Add new test files as needed
