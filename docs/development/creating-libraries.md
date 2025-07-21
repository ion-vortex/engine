# Creating Libraries

This guide walks through creating a new library in the Ion Vortex engine, following all architectural and coding standards.

## Overview

Ion Vortex libraries are:
- **Modular**: Can be built independently or as part of the whole
- **Interface-driven**: Public API through abstract interfaces
- **Dependency-aware**: Clear layering with no circular dependencies
- **Standards-compliant**: Following the C++ Manual strictly

## Library Structure

Every library follows this structure:
```
libs/your_library/
├── include/
│   └── ion/
│       └── your_library/
│           ├── your_type.h      # Public interface
│           ├── error_codes.h         # Library-specific errors
│           └── types.h               # Public types/structs
├── src/
│   ├── your_implementation.cpp       # Implementation
│   ├── your_implementation.h         # Private header
│   └── internal_helper.cpp           # Internal utilities
├── tests/
│   ├── integration_tests.cpp         # Main tests
│   └── CMakeLists.txt
├── CMakeLists.txt                    # Build configuration
└── README.md                         # Library documentation
```

## Step-by-Step Guide

### Step 1: Plan Your Library

Before writing code, answer these questions:

1. **What problem does this library solve?**
2. **What are its dependencies?** (Check the [dependency hierarchy](../architecture/))
3. **What interfaces will it expose?**
4. **How will it handle errors?**

### Step 2: Create Directory Structure

```bash
# From ion root
mkdir -p libs/your_library/{include/ion/your_library,src,tests}
```

### Step 3: Define the Public Interface

Create `include/ion/your_library/your_type.h`:

```cpp
#pragma once

#include <ion/core/error/error.h>
#include <memory>
#include <expected>

namespace ion::your_library {

// Forward declarations
class your_type_base;

// Factory function declaration
[[OAT_NODISCARD("Please ensure that the result is valid.")]]
std::expected<std::unique_ptr<your_type_base>, std::error_code> 
make_your_type(/* parameters */);

// Pure interface
class your_type_base {
public:
    virtual ~your_type_base() = default;
    
    // Core functionality
    virtual void do_something() = 0;
    
    [[OAT_NODISCARD("Please don't forget to check the result.")]]
    virtual std::expected<int, std::error_code> compute_value() = 0;
    
    // No implementation details in interface!
};

} // namespace ion::your_library
```

### Step 4: Define Error Codes

See the C++ manual for more information, but basically you should create your own error category and then start your enum class like `enum class my_lib_errc { my_first_error = 1; }`

### Step 5: Create Implementation

Create `src/your_implementation.h`:

```cpp
#pragma once

#include <ion/your_library/your_type.h>

namespace ion::your_library::detail {

class your_type_impl final : public your_type_base {
public:
    // Factory method
    [[OAT_NODISCARD("Handle the result")]]
    static std::expected<std::unique_ptr<your_type_impl>, std::error_code> 
    create(/* parameters */);
    
    // your_type_base implementation
    void do_something() override;
    std::expected<int, , std::error_code> compute_value() override;
    
private:
    // Private constructor - use factory
    your_type_impl(/* params */);
    
    // Private members
    int state_ = 0;
};

} // namespace ion::your_library::detail
```

Create `src/your_implementation.cpp`:

```cpp
#include "your_implementation.h"
#include <ion/your_library/error_codes.h>

namespace ion::your_library::detail {

std::expected<std::unique_ptr<your_type_base>, std::error_code> 
your_type_impl::create(/* parameters */) {
    // Validate parameters
    if (/* invalid */) {
        return std::unexpected(lib_errc::invalid);
    }
    
    // Create instance (can't use make_unique with private constructor)
    auto instance = std::unique_ptr<your_type_impl>(
        new your_type_impl(/* params */)
    );
    
    // Any initialization that can fail
    if (/* init failed */) {
        return std::unexpected(lib_errc::operation_failed);
    }
    
    return instance;
}

void your_type_impl::do_something() {
    // Implementation
    state_++;
}

std::expected<int, std::error_code> your_type_impl::compute_value() {
    if (state_ < 0) {
        return std::unexpected(lib_errc::invalid_state);
    }
    
    return state_ * 2;
}

your_type_impl::your_type_impl(/* params */) {
    // Constructor logic
}

} // namespace ion::your_library::detail

// Factory function implementation
namespace ion::your_library {

std::expected<std::unique_ptr<your_type_base>, std::error_code> 
make_your_interface(/* parameters */) {
    auto result = detail::your_type_impl::create(/* params */);
    if (!result) {
        return std::unexpected(result.error());
    }
    
    // Return as interface pointer
    return std::unique_ptr<your_type_base>(std::move(result.value()));
}

} // namespace ion::your_library
```

### Step 6: Create CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.28)

# Support standalone build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(IonYourLibrary VERSION 0.1.0 LANGUAGES CXX)
    
    # Add Ion Vortex helpers
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
    include(IonHelpers)
    ion_setup_build_interface()
    
    # Find dependencies
    ion_find_dependencies(json)  # Add what you need
    
    # Build dependencies if standalone
    if(NOT TARGET ion::core)
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core core)
    endif()
    # Add other ion:: dependencies as needed
    
    # Enable testing
    option(BUILD_TESTING "Build tests" ON)
    if(BUILD_TESTING)
        enable_testing()
        find_package(Catch2 CONFIG REQUIRED)
    endif()
endif()

# Register external dependencies FIRST
ion_add_external_dependency(your_library
    # List external deps like nlohmann_json::nlohmann_json
)

# Create library (sources auto-discovered)
ion_add_library(
    NAME your_library
    DEPENDENCIES 
        ion::build
        ion::core      # Add ion:: dependencies
)

# Link external dependencies
target_link_libraries(ion_your_library PUBLIC
    # External deps here
)

# Tests
if(BUILD_TESTING OR ION_BUILD_TESTS)
    add_subdirectory(tests)
endif()
```

### Step 7: Write Tests

Create `tests/integration_tests.cpp`:

```cpp
#include <catch2/catch_test_macros.hpp>
#include <ion/your_library/your_type.h>

using namespace ion::your_library;

TEST_CASE("YourInterface basic operations", "[your_library]") {
    SECTION("Creation succeeds with valid parameters") {
        auto result = make_your_interface(/* valid params */);
        REQUIRE(result.has_value());
        REQUIRE(result.value() != nullptr);
    }
    
    SECTION("Creation fails with invalid parameters") {
        auto result = make_your_interface(/* invalid params */);
        REQUIRE(!result.has_value());
        REQUIRE(result.error().code() == ErrorCode::invalid_parameter);
    }
    
    SECTION("Operations work correctly") {
        auto result = make_your_interface();
        REQUIRE(result.has_value());
        
        auto& interface = *result.value();
        
        // Test operations
        interface.do_something();
        
        auto value_result = interface.compute_value();
        REQUIRE(value_result.has_value());
        REQUIRE(value_result.value() == 2);
    }
}
```

Create `tests/CMakeLists.txt`:

```cmake
ion_add_test(
    NAME your_library_tests
    DEPENDENCIES ion::your_library
)
```

### Step 8: Add to Build System

Edit `libs/CMakeLists.txt` and add your library in dependency order:

```cmake
add_subdirectory(core)
# ... other libraries ...
add_subdirectory(your_library)  # After its dependencies
```

### Step 9: Document Your Library

Create `libs/your_library/README.md`:

```markdown
# Your Library

Brief description of what this library does.

## Usage

```cpp
#include <ion/your_library/your_type.h>

auto result = ion::your_library::make_your_interface();
if (!result) {
    // Handle error
    return result.error();
}

auto interface = std::move(result.value());
interface->do_something();
```

## API Reference

### your_type_base

Main interface for...

#### Methods

- `do_something()` - Description
- `compute_value()` - Returns computed value or error

### Error Codes (`oat::my_library::errc`)

- `invalid_parameter` - When...
- `operation_failed` - When...

## Dependencies

- `ion::core` - For error handling
- External: none

## Implementation Notes

Any important implementation details...

## Best Practices

### DO:
- ✅ Keep interfaces minimal and focused
- ✅ Hide all implementation details
- ✅ Use `std::expected` for all fallible operations
- ✅ Mark functions `[[OAT_NODISCARD("...")]]` when appropriate
- ✅ Follow the dependency hierarchy strictly
- ✅ Write comprehensive tests
- ✅ Document the public API

### DON'T:
- ❌ Expose implementation headers
- ❌ Use exceptions
- ❌ Create circular dependencies
- ❌ Put implementation in headers (unless template)
- ❌ Use raw `new`/`delete`
- ❌ Forget error handling

## Common Patterns

### Optional Dependencies

```cpp
struct config {
    logger_base* logger = nullptr;  // Optional
    metrics_base* metrics = nullptr;  // Optional
};

class implementation {
    void do_work() {
        if (logger_) {
            logger_->log("Doing work");
        }
        // Work happens regardless
    }
};
```

### Resource Management

```cpp
class resource_impl : public resource_base {
    static std::expected<std::unique_ptr<resource_impl>, std::error_code> create() {
        // Acquire resource
        auto handle = acquire_resource();
        if (!handle) {
            return std::unexpected(make_mylib_error(lib_errc::handle_invalid));
        }
        
        return std::unique_ptr<resource_impl>(
            new resource_impl(std::move(handle))
        );
    }
    
    ~resource_impl() {
        // Automatic cleanup
    }
};
```

### Async Operations

```cpp
class async_impl : public async_base {
    std::expected<void, std::error_code> start_operation() override {
        // Validate state
        if (busy_) {
            return std::unexpected(make_mylib_error(lib_errc::bad_state));
        }
        
        // Start async work
        future_ = std::async(std::launch::async, [this] {
            // Work happens here
        });
        
        return {};
    }
};
```

## Testing Strategies

### Integration Tests (Primary)
Test through public interface:
```cpp
TEST_CASE("Feature works end-to-end") {
    auto system = make_system();
    auto result = system->process(input);
    REQUIRE(result.value() == expected);
}
```

### Mock Dependencies
```cpp
class mock_logger : public logger_base {
    void log(std::string_view msg) override {
        messages_.push_back(std::string(msg));
    }
    
    std::vector<std::string> messages_;
};
```

### Error Cases
Always test error paths:
```cpp
TEST_CASE("Handles errors gracefully") {
    auto result = make_system(invalid_config);
    REQUIRE(!result.has_value());
    REQUIRE(result.error() == mylib::lib_errc::invalid_config);
}
```

## Next Steps

After creating your library:
1. Run tests: `ctest --preset debug-linux`
2. Build standalone: `cd libs/your_library && cmake -B build`
3. Use in apps: Link with `ion::your_library`
4. Document in API reference
5. Add examples if complex

Remember: Your library is only as good as its interface. Design it carefully!