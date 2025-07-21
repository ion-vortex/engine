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
#include <ion/your_library/your_interface.h>

using namespace ion::your_library;

TEST_CASE("Feature description", "[your_library]") {
    SECTION("Success case") {
        // Arrange
        auto system = makeYourSystem();
        REQUIRE(system.has_value());
        
        // Act
        auto result = system.value()->doOperation();
        
        // Assert
        REQUIRE(result.has_value());
        REQUIRE(result.value() == expected_value);
    }
    
    SECTION("Error case") {
        // Arrange
        auto system = makeYourSystem(invalid_config);
        
        // Assert
        REQUIRE(!system.has_value());
        REQUIRE(system.error().code() == ErrorCode::InvalidConfig);
    }
}
```

### Testing Async Operations

```cpp
TEST_CASE("Async operations complete correctly", "[async]") {
    auto system = makeAsyncSystem();
    REQUIRE(system.has_value());
    
    // Start operation
    auto start_result = system.value()->startOperation();
    REQUIRE(start_result.has_value());
    
    // Simulate ticks until complete
    bool completed = false;
    for (int i = 0; i < 100 && !completed; ++i) {
        system.value()->tick(i * 1000000);  // 1ms per tick
        completed = system.value()->isComplete();
    }
    
    REQUIRE(completed);
    
    // Check result
    auto result = system.value()->getResult();
    REQUIRE(result.has_value());
}
```

### Testing State Machines

```cpp
TEST_CASE("State machine transitions", "[state]") {
    auto machine = makeStateMachine();
    REQUIRE(machine.has_value());
    
    // Initial state
    REQUIRE(machine.value()->getState() == State::Idle);
    
    // Valid transition
    auto result = machine.value()->transition(Event::Start);
    REQUIRE(result.has_value());
    REQUIRE(machine.value()->getState() == State::Running);
    
    // Invalid transition
    result = machine.value()->transition(Event::Start);
    REQUIRE(!result.has_value());
    REQUIRE(result.error().code() == ErrorCode::InvalidTransition);
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

### Mock Network

```cpp
class mock_network_client : public network_client_base {
public:
    std::expected<void, std::error_code> connect(const std::string& host, 
                                      uint16_t port) override {
        if (should_fail_connect_) {
            return std::unexpected(make_net_error(net_errc::connection_failed));
        }
        
        connected_ = true;
        connect_calls_.push_back({host, port});
        return {};
    }
    
    std::expected<void, std::error_code> send(BufferView data) override {
        if (!connected_) {
            return std::unexpected(make_net_error(net_errc::not_connected));
        }
        
        sent_data_.emplace_back(data.begin(), data.end());
        return {};
    }
    
    // Test controls
    void set_connection_failure(bool fail) { should_fail_connect_ = fail; }
    void inject_receive_data(std::vector<uint8_t> data) {
        receive_queue_.push_back(std::move(data));
    }
    
    // Test queries
    const auto& get_sent_data() const { return sent_data_; }
    bool is_connected() const { return connected_; }
    
private:
    bool connected_ = false;
    bool should_fail_connect_ = false;
    std::vector<std::pair<std::string, uint16_t>> connect_calls_;
    std::vector<std::vector<uint8_t>> sent_data_;
    std::queue<std::vector<uint8_t>> receive_queue_;
};
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
TEST_CASE("System processes valid input correctly", "[integration]")
TEST_CASE("System rejects malformed packets", "[integration]")

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
- `[graphics]` - Rendering tests
- Module names: `[core]`, `[render]`, etc.

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

### Property-Based Tests

```cpp
TEST_CASE("Serialization round-trip", "[protocol]") {
    // Test with various sizes
    for (size_t size : {0, 1, 127, 128, 1024, 65535}) {
        // Generate test data
        std::vector<uint8_t> original(size);
        std::iota(original.begin(), original.end(), 0);
        
        // Serialize
        auto serialized = serialize(original);
        REQUIRE(serialized.has_value());
        
        // Deserialize
        auto deserialized = deserialize(serialized.value());
        REQUIRE(deserialized.has_value());
        
        // Verify
        REQUIRE(deserialized.value() == original);
    }
}
```

### Fuzzing Support

```cpp
TEST_CASE("Parser handles malformed input", "[parser][fuzz]") {
    // Common malformed inputs
    const std::vector<std::vector<uint8_t>> fuzz_inputs = {
        {},                          // Empty
        {0xFF},                      // Invalid header
        {0x01, 0xFF, 0xFF},         // Length overflow
        std::vector<uint8_t>(1000, 0),  // Large input
    };
    
    for (const auto& input : fuzz_inputs) {
        auto result = parse_packet(input);
        // Should not crash, should return error
        if (!result.has_value()) {
            REQUIRE(result.error() != 0);
        }
    }
}
```

## Testing Error Paths

### Resource Exhaustion

```cpp
TEST_CASE("Handles resource exhaustion", "[resources]") {
    auto pool = make_resource_pool(3);  // Max 3 resources
    
    // Allocate all
    std::vector<std::unique_ptr<resource>> resources;
    for (int i = 0; i < 3; ++i) {
        auto r = pool->acquire();
        REQUIRE(r.has_value());
        resources.push_back(std::move(r.value()));
    }
    
    // Next allocation should fail
    auto extra = pool->acquire();
    REQUIRE(!extra.has_value());
    REQUIRE(extra.error().code() == my_lib::lib_errc::resource_exhausted);
    
    // Release one
    resources.pop_back();
    
    // Should succeed now
    auto retry = pool->acquire();
    REQUIRE(retry.has_value());
}
```

### Timeout Testing

```cpp
TEST_CASE("Operation times out", "[timeout]") {
    auto client = make_client();
    client->set_timeout(100ms);
    
    // Mock slow server
    MockServer server;
    server.set_response_delay(200ms);
    
    auto result = client->request(server);
    REQUIRE(!result.has_value());
    REQUIRE(result.error() == net_errc::timed_out);
}
```

## Performance Testing

### Benchmarking

```cpp
TEST_CASE("Benchmark serialization", "[.benchmark]") {
    // Tag with . to skip in normal runs
    const size_t iterations = 10000;
    TestData data = generate_test_data();
    
    BENCHMARK("Serialize") {
        for (size_t i = 0; i < iterations; ++i) {
            auto result = serialize(data);
            do_not_optimize(result);
        }
    };
    
    BENCHMARK("Deserialize") {
        auto serialized = serialize(data).value();
        for (size_t i = 0; i < iterations; ++i) {
            auto result = deserialize(serialized);
            do_not_optimize(result);
        }
    };
}
```

### Memory Testing

```cpp
class memory_tracker {
    size_t allocations_ = 0;
    size_t deallocations_ = 0;
    size_t current_usage_ = 0;
    
public:
    void* allocate(size_t size) {
        allocations_++;
        current_usage_ += size;
        return std::malloc(size);
    }
    
    void deallocate(void* ptr, size_t size) {
        deallocations_++;
        current_usage_ -= size;
        std::free(ptr);
    }
    
    bool has_leaks() const {
        return allocations_ != deallocations_;
    }
};

TEST_CASE("No memory leaks", "[memory]") {
    memory_tracker tracker;
    
    {
        auto system = make_system_with_allocator(&tracker);
        system->do_work();
        // System destroyed here
    }
    
    REQUIRE(!tracker.has_leaks());
    REQUIRE(tracker.current_usage_ == 0);
}
```

## Test Utilities

### Test Helpers

```cpp
// test_helpers.h
namespace test {

// Generate test data
std::vector<uint8_t> generate_random_bytes(size_t count) {
    std::vector<uint8_t> data(count);
    std::generate(data.begin(), data.end(), 
                  []() { return rand() % 256; });
    return data;
}

// Compare floating point
bool approx_equal(float a, float b, float epsilon = 0.0001f) {
    return std::abs(a - b) < epsilon;
}

// Wait for condition with timeout
template<typename Pred>
bool wait_for(Pred predicate, std::chrono::milliseconds timeout) {
    auto start = std::chrono::steady_clock::now();
    while (!predicate()) {
        if (std::chrono::steady_clock::now() - start > timeout) {
            return false;
        }
        std::this_thread::sleep_for(1ms);
    }
    return true;
}

} // namespace test
```

## CI/CD Integration

### CMake Configuration

```cmake
# tests/CMakeLists.txt
ion_add_test(
    NAME your_library_tests
    DEPENDENCIES 
        ion::your_library
        test_utilities
)

# Add test labels
set_tests_properties(your_library_tests PROPERTIES
    LABELS "unit;integration"
    TIMEOUT 30
)

# Memory check
add_test(
    NAME your_library_memcheck
    COMMAND valgrind --leak-check=full $<TARGET_FILE:your_library_tests>
)
```

### Running Tests

```bash
# Run all tests
ctest --preset debug-linux

# Run specific test
ctest --preset debug-linux -R your_library

# Run with output
ctest --preset debug-linux -V

# Run only fast tests
ctest --preset debug-linux -L "unit" -LE "slow"

# Parallel execution
ctest --preset debug-linux -j8
```

## Test Coverage

### Generating Coverage Reports

```bash
# Build with coverage
cmake --preset debug-linux -DCMAKE_CXX_FLAGS="--coverage"
cmake --build --preset debug-linux

# Run tests
ctest --preset debug-linux

# Generate report
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
lcov --list coverage.info
```

### Coverage Guidelines

Aim for:
- 90%+ line coverage for core libraries
- 80%+ branch coverage
- 100% coverage of error paths
- Focus on meaningful coverage, not numbers

## Best Practices

### DO:
- ✅ Test the public interface
- ✅ Use descriptive test names
- ✅ Keep tests independent
- ✅ Test error conditions
- ✅ Use mocks for external dependencies
- ✅ Make tests deterministic
- ✅ Keep tests fast (<100ms)

### DON'T:
- ❌ Test implementation details
- ❌ Use real network/filesystem
- ❌ Depend on test order
- ❌ Use sleep() for timing
- ❌ Share state between tests
- ❌ Write tests after debugging

## Testing Checklist

Before submitting code:

- [ ] All tests pass locally
- [ ] New features have tests
- [ ] Error paths are tested
- [ ] No flaky tests introduced
- [ ] Tests work on all platforms
- [ ] Performance hasn't regressed
- [ ] No memory leaks (valgrind clean)

## Debugging Tests

### When Tests Fail

1. **Read the error message** - Catch2 provides detailed output
2. **Run single test** - Isolate the failure
3. **Add logging** - Temporarily add debug output
4. **Check mocks** - Verify mock behavior
5. **Debugger** - Step through test execution

### Common Issues

**Flaky Tests**
- Remove timing dependencies
- Mock system calls
- Use deterministic data

**Platform Differences**
- Check endianness
- File path separators
- Line endings

**Memory Issues**
- Run with sanitizers
- Check for use-after-free
- Verify cleanup order

## Conclusion

Good tests are:
- Fast to run
- Easy to understand
- Reliable indicators of problems
- Helpful when they fail

Write tests that you'll thank yourself for in six months!