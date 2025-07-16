# Oxide Build System Documentation

## Overview

The Oxide project uses a modern CMake-based build system designed to support both monolithic builds (building everything together) and modular builds (building individual libraries separately). The system enforces strict dependency rules from the C++ Manual while providing flexibility for developers.

## Table of Contents

1. [Architecture](#architecture)
2. [Directory Structure](#directory-structure)
3. [Build System Components](#build-system-components)
4. [Building Oxide](#building-oxide)
5. [Library Development](#library-development)
6. [Application Development](#application-development)
7. [Testing](#testing)
8. [Installation and Packaging](#installation-and-packaging)
9. [Troubleshooting](#troubleshooting)

## Architecture

### Design Principles

1. **Static Linking Only**: All libraries are built as static archives to enable Link-Time Optimization (LTO)
2. **Transitive Dependencies**: The build system automatically handles transitive dependencies
3. **No Direct External Dependencies**: Applications and tests only link against `oxide::` targets
4. **Automatic Source Discovery**: Sources are discovered automatically based on standard directory layout
5. **Modern CMake**: Target-based design using CMake 3.28+ features

### Dependency Rules

Libraries are organized in layers with strict dependency rules:

```
┌─────────────────────────────────────────────────────┐
│                    apps/*                           │
├─────────────────────────────────────────────────────┤
│  ui          │  render       │  protocol            │
├──────────────┼───────────────┼─────────────────────┤
│  audio       │  physics      │  asset      │crypto │
├──────────────┴───────────────┴─────────────┴───────┤
│                      core                           │
└─────────────────────────────────────────────────────┘
```

- **core**: No dependencies (bottom layer)
- **crypto**: Depends on core only
- **asset**: Depends on core, crypto
- **physics**: Depends on core only
- **audio**: Depends on core only
- **protocol**: Depends on core, crypto
- **render**: Depends on core, physics (read-only)
- **ui**: Depends on core, render

## Directory Structure

```
oxide/
├── cmake/
│   ├── OxideHelpers.cmake      # Build system utilities
│   └── OxideConfig.cmake.in    # Package config template
├── libs/
│   ├── core/
│   │   ├── include/
│   │   │   └── oxide/
│   │   │       └── core/       # Public headers
│   │   ├── src/                # Implementation files
│   │   ├── tests/              # Unit/integration tests
│   │   └── CMakeLists.txt
│   └── [other libraries...]
├── apps/
│   ├── client/
│   │   ├── src/                # Application sources
│   │   └── CMakeLists.txt
│   └── [other apps...]
├── CMakeLists.txt              # Top-level build file
├── CMakePresets.json           # Build presets
└── vcpkg.json                  # Dependencies manifest
```

## Build System Components

### OxideHelpers.cmake

This file contains the core build system utilities:

#### `oxide_add_library()`

Creates a static library with standard Oxide settings:

```cmake
oxide_add_library(
    NAME core
    DEPENDENCIES oxide::build    # Other oxide:: libraries only
)
```

Features:
- Automatically discovers sources in `src/` 
- Automatically discovers public headers in `include/oxide/${NAME}/`
- Sets up include directories correctly
- Handles installation and export

#### `oxide_add_external_dependency()`

Registers external dependencies for a library:

```cmake
oxide_add_external_dependency(core
    glm::glm
    nlohmann_json::nlohmann_json
)
```

Must be called BEFORE `oxide_add_library()`.

#### `oxide_add_application()`

Creates an executable with all transitive dependencies:

```cmake
oxide_add_application(
    NAME client
    DEPENDENCIES oxide::core oxide::render oxide::ui
)
```

Features:
- Automatically discovers sources in `src/`
- Collects ALL transitive dependencies (internal and external)
- Ensures static linking of everything

#### `oxide_add_test()`

Creates a test executable:

```cmake
oxide_add_test(
    NAME core_tests
    DEPENDENCIES oxide::core
)
```

### CMakePresets.json

Defines standard build configurations:

- **debug-windows**: Debug build using Visual Studio generator
- **release-windows**: Release build using Visual Studio generator
- **debug-linux**: Debug build using Ninja
- **release-linux**: Release build using Ninja
- **debug-macos**: Debug build using Ninja
- **release-macos**: Release build using Ninja

## Building Oxide

### Prerequisites

1. CMake 3.28 or later
2. C++23 capable compiler (MSVC 19.39+, GCC 13+, Clang 16+)
3. vcpkg (for dependency management)
4. Ninja (Linux/macOS) or Visual Studio 2022 (Windows)

### Full Build

```bash
# Configure
cmake --preset release-windows    # or release-linux, release-macos

# Build
cmake --build --preset release-windows

# Install (optional)
cmake --install build/release-windows --prefix /path/to/install
```

### Building Individual Libraries

Libraries can be built standalone:

```bash
cd libs/render
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

The build system will automatically build required dependencies.

## Library Development

### Creating a New Library

1. Create directory structure:
   ```
   libs/mylib/
   ├── include/
   │   └── oxide/
   │       └── mylib/
   │           └── my_interface.h
   ├── src/
   │   └── my_implementation.cpp
   ├── tests/
   │   └── test_mylib.cpp
   └── CMakeLists.txt
   ```

2. Write `CMakeLists.txt`:
   ```cmake
   cmake_minimum_required(VERSION 3.28)

   # Support standalone builds
   if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
       project(OxideMyLib VERSION 0.1.0 LANGUAGES CXX)
       
       list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
       include(OxideHelpers)
       oxide_setup_build_interface()
       
       # Find external dependencies
       oxide_find_dependencies(json)
   endif()

   # Register external dependencies
   oxide_add_external_dependency(mylib
       nlohmann_json::nlohmann_json
   )

   # Create library
   oxide_add_library(
       NAME mylib
       DEPENDENCIES 
           oxide::build
           oxide::core  # Internal dependencies
   )

   # Link external dependencies
   target_link_libraries(oxide_mylib PUBLIC
       nlohmann_json::nlohmann_json
   )

   # Tests
   if(BUILD_TESTING OR OXIDE_BUILD_TESTS)
       add_subdirectory(tests)
   endif()
   ```

3. Update `libs/CMakeLists.txt` to add your library in dependency order

### Header Organization

Public headers must follow the namespace structure:
- File: `include/oxide/mylib/foo.h`
- Namespace: `oxide::mylib`
- Include: `#include <oxide/mylib/foo.h>`

### External Dependencies

External dependencies are handled in two steps:

1. Register them with `oxide_add_external_dependency()` BEFORE creating the library
2. Link them with `target_link_libraries()` AFTER creating the library

This two-step process allows the build system to track all transitive external dependencies.

## Application Development

### Creating an Application

1. Create directory structure:
   ```
   apps/myapp/
   ├── src/
   │   └── main.cpp
   └── CMakeLists.txt
   ```

2. Write `CMakeLists.txt`:
   ```cmake
   cmake_minimum_required(VERSION 3.28)

   if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
       project(OxideMyApp VERSION 0.1.0 LANGUAGES CXX)
       
       list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
       include(OxideHelpers)
       oxide_setup_build_interface()
       
       # Build required libraries
       add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../libs libs)
   endif()

   # Create executable - only specify oxide:: dependencies
   oxide_add_application(
       NAME myapp
       DEPENDENCIES
           oxide::core
           oxide::render
   )
   ```

### Important Notes

- Applications should NEVER directly link external libraries
- All external dependencies come transitively through oxide:: libraries
- The build system automatically handles all transitive dependencies

## Testing

### Writing Tests

1. Create test file in `libs/mylib/tests/`:
   ```cpp
   #include <catch2/catch_test_macros.hpp>
   #include <oxide/mylib/my_interface.h>

   TEST_CASE("MyLib basic functionality", "[mylib]") {
       // Test code
   }
   ```

2. Create `tests/CMakeLists.txt`:
   ```cmake
   oxide_add_test(
       NAME mylib_tests
       DEPENDENCIES oxide::mylib
   )
   ```

### Running Tests

```bash
# Build with tests enabled
cmake --preset debug-windows -DOXIDE_BUILD_TESTS=ON
cmake --build --preset debug-windows

# Run tests
ctest --preset debug-windows
# or
./build/debug-windows/libs/mylib/tests/mylib_tests
```

## Installation and Packaging

### Installing Oxide

```bash
cmake --build --preset release-windows
cmake --install build/release-windows --prefix /path/to/install
```

This installs:
- Static libraries to `lib/`
- Headers to `include/oxide/`
- CMake config files to `lib/cmake/oxide/`

### Using Installed Oxide

In another project:

```cmake
find_package(Oxide REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE
    oxide::core
    oxide::render
)
```

With components:

```cmake
find_package(Oxide REQUIRED COMPONENTS core render ui)
```

## Troubleshooting

### Common Issues

#### "No CMAKE_CXX_COMPILER could be found" on Windows

This typically happens when using Ninja generator without proper environment setup.

**Solution**: Use Visual Studio generator (default in our presets) or run from a Visual Studio Developer Command Prompt.

#### Missing Dependencies

If you get errors about missing targets when building standalone libraries:

**Solution**: Make sure to add required libraries to your standalone build:
```cmake
if(NOT TARGET oxide::core)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../core core)
endif()
```

#### Circular Dependencies

The build system enforces the dependency hierarchy. If you get errors about circular dependencies:

**Solution**: Review the [dependency rules](#dependency-rules) and refactor your code to follow the layered architecture.

### Build System Debugging

Enable verbose output:
```bash
cmake --build . --verbose
```

Check what's included in a target:
```cmake
get_target_property(deps oxide_mylib INTERFACE_LINK_LIBRARIES)
message(STATUS "Dependencies: ${deps}")
```

### Platform-Specific Notes

#### Windows
- Default generator is Visual Studio 2022
- Ninja requires Developer Command Prompt
- Use forward slashes in paths even on Windows

#### Linux/macOS
- Default generator is Ninja
- Make sure to install all system dependencies (see README.md)
- Use `CMAKE_BUILD_TYPE` to set configuration

## Advanced Topics

### Custom Build Configurations

Add to CMakePresets.json:
```json
{
  "name": "custom-config",
  "inherits": "base",
  "cacheVariables": {
    "CMAKE_BUILD_TYPE": "RelWithDebInfo",
    "OXIDE_ENABLE_MARCH_NATIVE": "ON"
  }
}
```

### Cross-Compilation

Use vcpkg triplets and CMake toolchain files:
```bash
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake \
  -DVCPKG_TARGET_TRIPLET=arm64-linux \
  -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=/path/to/arm-toolchain.cmake
```

### IDE Integration

The build system generates `compile_commands.json` for IDE integration:
- VS Code: Works automatically with CMake Tools extension
- CLion: Import as CMake project
- Visual Studio: Open folder or use CMake support

## Appendix: Build System Files Reference

- `cmake/OxideHelpers.cmake`: Core build utilities
- `cmake/OxideConfig.cmake.in`: Package config template
- `CMakePresets.json`: Build configuration presets
- `vcpkg.json`: External dependency manifest
- `vcpkg/triplets/`: Custom vcpkg triplets for static LTO builds