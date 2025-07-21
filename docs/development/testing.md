# Testing Guide

This document covers testing practices for Ion Vortex, emphasizing integration testing while explaining when and how to use unit tests.

## Testing Philosophy

Ion Vortex follows these testing principles:

1. **Integration tests are primary** - Test through public interfaces
2. **No friend classes** - Tests use the same API as users
3. **Mock interfaces, not implementations** - Create test doubles of interfaces
4. **Test error paths** - Failures are as important as success
5. **Deterministic tests** - No randomness, network calls, or timing dependencies
6. **Fast feedback** - Tests must run quickly

## Test Structure

```
libs/your_library/tests/
├── integration_tests.cpp     # Primary tests through public API
├── mock_dependencies.h       # Mock implementations of interfaces
├── test_helpers.cpp         # Shared test utilities
├── unit_tests.cpp           # Unit tests for isolated components
└── CMakeLists.txt
```

## Writing Integration Tests

### Basic Test Structure

```cpp
#include <catch2/catch_test_macros.hpp>
#include <ion/engine_interface/engine_interface.h>

using namespace ion::engine_interface;

TEST_CASE("Engine interface operations", "[engine_interface]") {
    SECTION("Initialization") {
        // Arrange
        auto engine = make_engine_interface();
        REQUIRE(engine.has_value());

        // Act
        auto result = engine.value()->initialize();

        // Assert
        REQUIRE(result.has_value());
    }

    SECTION("Error handling") {
        // Arrange
        auto engine = make_engine_interface(invalid_config);

        // Assert
        REQUIRE(!engine.has_value());
        REQUIRE(engine.error() == core_errc::invalid_config);
    }
}
```

### Testing Async Operations

```cpp
TEST_CASE("Async script execution", "[script]") {
    auto script_engine = make_script_engine();
    REQUIRE(script_engine.has_value());

    // Start script execution
    auto start_result = script_engine.value()->execute_script("test_script");
    REQUIRE(start_result.has_value());

    // Simulate ticks until complete
    bool completed = false;
    for (int i = 0; i < 100 && !completed; ++i) {
        script_engine.value()->tick(i * 1000000);  // 1ms per tick
        completed = script_engine.value()->is_execution_complete();
    }

    REQUIRE(completed);

    // Check result
    auto result = script_engine.value()->get_execution_result();
    REQUIRE(result.has_value());
}
```

### Testing State Machines

```cpp
TEST_CASE("state machine transitions", "[state]") {
    auto machine = make_state_machine();
    REQUIRE(machine.has_value());

    // Initial state
    REQUIRE(machine.value()->get_state() == state::idle);

    // Valid transition
    auto result = machine.value()->transition(event::start);
    REQUIRE(result.has_value());
    REQUIRE(machine.value()->get_state() == state::running);

    // Invalid transition
    result = machine.value()->transition(event::start);
    REQUIRE(!result.has_value());
    REQUIRE(result.error().code() == core_errc::InvalidTransition);
}
```

## Creating Mocks

### Mock Interfaces

```cpp
// mock_dependencies.h
#pragma once

#include <ion/core/logging/logger.h>
#include <vector>
#include <string>

namespace test {

class mock_logger : public ion::core::logger_base {
public:
    void log(ion::core::log_level level, 
             const std::string& message) override {
        entries_.push_back({level, message});
    }

    // Test helpers
    bool has_message(const std::string& msg) const {
        return std::any_of(entries_.begin(), entries_.end(),
            [&](const auto& e) { return e.message == msg; });
    }

    size_t message_count() const { return entries_.size(); }

    void clear() { entries_.clear(); }

private:
    struct log_entry {
        ion::core::log_level level;
        std::string message;
    };
    std::vector<log_entry> entries_;
};

} // namespace test
```

## Unit Testing Guidelines

Unit tests are appropriate for:
- Pure functions with no dependencies
- Algorithm implementations
- Data structures
- Utility classes

### Example Unit Test

```cpp
// unit_tests.cpp
#include <catch2/catch_test_macros.hpp>
#include "bit_packer.h"  // Internal utility

TEST_CASE("BitPacker packs values correctly", "[unit][bitpacker]") {
    BitPacker packer;

    SECTION("Pack single bit") {
        packer.write_bit(true);
        auto data = packer.get_data();

        REQUIRE(data.size() == 1);
        REQUIRE(data[0] == 0x80);  // 10000000
    }

    SECTION("Pack multiple values") {
        packer.write_bits(0x5, 3);   // 101
        packer.write_bits(0xA, 4);   // 1010
        packer.write_bit(true);      // 1

        auto data = packer.get_data();
        REQUIRE(data.size() == 1);
        REQUIRE(data[0] == 0xAB);    // 10101011
    }
}
```

## Test Organization

### Naming Conventions

```cpp
// Integration tests - describe behavior
TEST_CASE("Engine processes valid input correctly", "[integration]")
TEST_CASE("Engine rejects malformed packets", "[integration]")

// Unit tests - describe specific functionality
TEST_CASE("CRC32 calculates correct checksum", "[unit]")
TEST_CASE("RingBuffer wraps at capacity", "[unit]")

// Use sections for variations
TEST_CASE("Connection handling", "[network]") {
    SECTION("Successful connection") { }
    SECTION("Connection timeout") { }
    SECTION("Connection refused") { }
}
```

### Test Tags

Use consistent tags for filtering:
- `[integration]` - Integration tests
- `[unit]` - Unit tests
- `[slow]` - Tests that take >100ms
- `[network]` - Network-related tests
- Module names: `[core]`, `[engine_interface]`, `[script]`, etc.

## Testing Patterns

### Table-Driven Tests

```cpp
TEST_CASE("Error code to string conversion", "[core]") {
    struct test_case {
        std::error_code code;
        std::string_view expected;
    };

    const test_case cases[] = {
        {ion::core::core_errc::success, "Success"},
        {ion::core::core_errc::invalid_parameter, "Invalid parameter"},
        {ion::core::core_errc::not_found, "Not found"},
        {ion::core::core_errc::timeout, "Operation timed out"},
    };

    for (const auto& tc : cases) {
        REQUIRE(core_errc_to_string(tc.code) == tc.expected);
    }
}
```

## CI/CD Integration

### CMake Configuration

```cmake
# tests/CMakeLists.txt
ion_add_test(
    NAME engine_interface_tests
    DEPENDENCIES 
        ion::engine_interface
        test_utilities
)

# Add test labels
set_tests_properties(engine_interface_tests PROPERTIES
    LABELS "unit;integration"
    TIMEOUT 30
)
```

### Running Tests

```bash
# Run all tests
ctest --preset debug-linux

# Run specific test
ctest --preset debug-linux -R engine_interface

# Run with output
ctest --preset debug-linux -V

# Run only fast tests
ctest --preset debug-linux -L "unit" -LE "slow"

# Parallel execution
ctest --preset debug-linux -j8
```