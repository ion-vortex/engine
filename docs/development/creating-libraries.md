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
│           ├── your_interface.h      # Public interface
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

Create `include/ion/your_library/your_interface.h`:

```cpp
#pragma once

#include <ion/core/error/error.h>
#include <memory>
#include <expected>

namespace ion::your_library {

// Forward declarations
class IYourInterface;

// Factory function declaration
[[nodiscard("Handle the result"), gnu::warn_unused_result]]
std::expected<std::unique_ptr<IYourInterface>, core::Error> 
makeYourInterface(/* parameters */);

// Pure interface
class IYourInterface {
public:
    virtual ~IYourInterface() = default;
    
    // Core functionality
    virtual void doSomething() = 0;
    
    [[nodiscard("Check the result"), gnu::warn_unused_result]]
    virtual std::expected<int, core::Error> computeValue() = 0;
    
    // No implementation details in interface!
};

} // namespace ion::your_library
```

### Step 4: Define Error Codes

Create `include/ion/your_library/error_codes.h`:

**Important**: Check `ion/core/error/error.h` for reserved error code ranges. Each library has an assigned range:
- 1000-1999: render
- 2000-2999: net
- 3000-3999: asset
- 4000-4999: audio
- 5000-5999: crypto
- 6000-6999: fs
- 7000-7999: physics
- 8000-8999: protocol
- 9000-9999: ui
- 10000+: Applications

```cpp
#pragma once

#include <ion/core/error/error.h>

namespace ion::your_library {

enum class ErrorCode : std::uint32_t {
    // Mirror core sentinels
    Ok      = 0x00000000,
    Unknown = 0xFFFFFFFF,
    
    // Library-specific errors (use your assigned range)
    // Example: If assigned 3000-3999
    NotInitialized      = 3001,
    InvalidParameter    = 3002,
    ResourceUnavailable = 3003,
    OperationFailed     = 3004,
    InvalidState        = 3005,
    // ... more as needed
};

// Convert error code to string
constexpr std::string_view to_string(ErrorCode code) noexcept {
    switch (code) {
        case ErrorCode::Ok:                  return "Ok";
        case ErrorCode::Unknown:             return "Unknown error";
        case ErrorCode::NotInitialized:      return "Not initialized";
        case ErrorCode::InvalidParameter:    return "Invalid parameter";
        case ErrorCode::ResourceUnavailable: return "Resource unavailable";
        case ErrorCode::OperationFailed:     return "Operation failed";
        case ErrorCode::InvalidState:        return "Invalid state";
        default:
            // Try core error codes
            return ion::core::to_string(static_cast<ion::core::ErrorCode>(code));
    }
}

} // namespace ion::your_library
```

### Step 5: Create Implementation

Create `src/your_implementation.h`:

```cpp
#pragma once

#include <ion/your_library/your_interface.h>

namespace ion::your_library::detail {

class YourImplementation final : public IYourInterface {
public:
    // Factory method
    [[nodiscard("Handle the result"), gnu::warn_unused_result]]
    static std::expected<std::unique_ptr<YourImplementation>, core::Error> 
    Create(/* parameters */);
    
    // IYourInterface implementation
    void doSomething() override;
    std::expected<int, core::Error> computeValue() override;
    
private:
    // Private constructor - use factory
    YourImplementation(/* params */);
    
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

std::expected<std::unique_ptr<YourImplementation>, core::Error> 
YourImplementation::Create(/* parameters */) {
    // Validate parameters
    if (/* invalid */) {
        return std::unexpected(core::Error(
            ErrorCode::InvalidParameter,
            "Description of what's wrong"
        ));
    }
    
    // Create instance (can't use make_unique with private constructor)
    auto instance = std::unique_ptr<YourImplementation>(
        new YourImplementation(/* params */)
    );
    
    // Any initialization that can fail
    if (/* init failed */) {
        return std::unexpected(core::Error(
            ErrorCode::OperationFailed,
            "Initialization failed"
        ));
    }
    
    return instance;
}

void YourImplementation::doSomething() {
    // Implementation
    state_++;
}

std::expected<int, core::Error> YourImplementation::computeValue() {
    if (state_ < 0) {
        return std::unexpected(core::Error(
            ErrorCode::InvalidState,
            "State is negative"
        ));
    }
    
    return state_ * 2;
}

YourImplementation::YourImplementation(/* params */) {
    // Constructor logic
}

} // namespace ion::your_library::detail

// Factory function implementation
namespace ion::your_library {

std::expected<std::unique_ptr<IYourInterface>, core::Error> 
makeYourInterface(/* parameters */) {
    auto result = detail::YourImplementation::Create(/* params */);
    if (!result) {
        return std::unexpected(result.error());
    }
    
    // Return as interface pointer
    return std::unique_ptr<IYourInterface>(std::move(result.value()));
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
#include <ion/your_library/your_interface.h>

using namespace ion::your_library;

TEST_CASE("YourInterface basic operations", "[your_library]") {
    SECTION("Creation succeeds with valid parameters") {
        auto result = makeYourInterface(/* valid params */);
        REQUIRE(result.has_value());
        REQUIRE(result.value() != nullptr);
    }
    
    SECTION("Creation fails with invalid parameters") {
        auto result = makeYourInterface(/* invalid params */);
        REQUIRE(!result.has_value());
        REQUIRE(result.error().code() == ErrorCode::InvalidParameter);
    }
    
    SECTION("Operations work correctly") {
        auto result = makeYourInterface();
        REQUIRE(result.has_value());
        
        auto& interface = *result.value();
        
        // Test operations
        interface.doSomething();
        
        auto value_result = interface.computeValue();
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
#include <ion/your_library/your_interface.h>

auto result = ion::your_library::makeYourInterface();
if (!result) {
    // Handle error
    return result.error();
}

auto interface = std::move(result.value());
interface->doSomething();
```

## API Reference

### IYourInterface

Main interface for...

#### Methods

- `doSomething()` - Description
- `computeValue()` - Returns computed value or error

### Error Codes

- `InvalidParameter` - When...
- `OperationFailed` - When...

## Dependencies

- `ion::core` - For error handling
- External: none

## Implementation Notes

Any important implementation details...
```

## Best Practices

### DO:
- ✅ Keep interfaces minimal and focused
- ✅ Hide all implementation details
- ✅ Use `std::expected` for all fallible operations
- ✅ Mark functions `[[nodiscard, gnu::warn_unused_result]]
` when appropriate
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
struct Config {
    ILogger* logger = nullptr;  // Optional
    IMetrics* metrics = nullptr;  // Optional
};

class Implementation {
    void doWork() {
        if (logger_) {
            logger_->log("Doing work");
        }
        // Work happens regardless
    }
};
```

### Resource Management

```cpp
class ResourceImpl : public IResource {
    static std::expected<std::unique_ptr<ResourceImpl>, Error> Create() {
        // Acquire resource
        auto handle = acquireResource();
        if (!handle) {
            return std::unexpected(Error(ErrorCode::ResourceUnavailable));
        }
        
        return std::unique_ptr<ResourceImpl>(
            new ResourceImpl(std::move(handle))
        );
    }
    
    ~ResourceImpl() {
        // Automatic cleanup
    }
};
```

### Async Operations

```cpp
class AsyncImpl : public IAsync {
    std::expected<void, Error> startOperation() override {
        // Validate state
        if (busy_) {
            return std::unexpected(Error(ErrorCode::Busy));
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
    auto system = makeSystem();
    auto result = system->process(input);
    REQUIRE(result.value() == expected);
}
```

### Mock Dependencies
```cpp
class MockLogger : public ILogger {
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
    auto result = makeSystem(invalid_config);
    REQUIRE(!result.has_value());
    REQUIRE(result.error().code() == ErrorCode::InvalidConfig);
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