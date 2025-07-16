# Development Guide

This section contains everything you need for day-to-day development on the Oxide project.

## Overview

Oxide follows strict engineering standards to ensure code quality, maintainability, and performance. This guide covers:
- Coding standards and conventions
- How to work with the build system
- Creating new components
- Testing practices

## Core Documents

### 📖 [C++ Manual](cpp-manual.md)
The complete Oxide/Oat Interactive C++ engineering standard. This is **required reading** for all contributors. It covers:
- Language rules (C++23, no exceptions, no RTTI)
- Architectural principles
- Memory management
- API design patterns
- What NOT to do

### 📋 [C++ Manual - Quick Reference](cpp-manual-short.md)
A condensed version of the manual for quick lookups. Keep this handy while coding.

### 🔧 [Build System](build-system.md)
Comprehensive guide to the CMake-based build system:
- How the modular build works
- CMake helpers and macros
- Dependencies management
- Installation and packaging

### 📚 [Creating Libraries](creating-libraries.md)
Step-by-step guide for adding new libraries:
- Directory structure
- CMake configuration
- Interface design
- Integration with existing libraries

### 📱 [Creating Applications](creating-apps.md)
How to create new applications using Oxide:
- Application structure
- Using oxide libraries
- Platform considerations
- Packaging

### 🧪 [Testing](testing.md)
Testing philosophy and practices:
- Writing integration tests
- Unit testing guidelines
- Mocking interfaces
- CI/CD integration

## Development Workflow

### Daily Development Cycle

1. **Update your branch**
   ```bash
   git pull --rebase origin main
   ```

2. **Build and test**
   ```bash
   cmake --preset debug-linux  # or your platform
   cmake --build --preset debug-linux
   ctest --preset debug-linux
   ```

3. **Make changes**
   - Follow the C++ Manual strictly
   - Write tests for new functionality
   - Update documentation

4. **Validate changes**
   ```bash
   # Format check (if we have one)
   # Lint check
   # Run tests
   ctest --preset debug-linux
   ```

## Key Principles

### 1. **No Exceptions Policy**
```cpp
// ❌ Wrong
throw std::runtime_error("Failed");

// ✅ Correct
return std::unexpected(Error(ErrorCode::Failed, "Operation failed"));
```

### 2. **Factory Pattern for Objects**
```cpp
// ❌ Wrong
auto obj = new MyClass(params);

// ✅ Correct
auto result = MyClass::Create(params);
if (!result) return result.error();
auto obj = std::move(result.value());
```

### 3. **Interface-Based Design**
```cpp
// Public interface
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void render() = 0;
};

// Hidden implementation
class RendererImpl : public IRenderer {
    void render() override { /* ... */ }
};
```

### 4. **Explicit Error Handling**
Every fallible operation returns `std::expected<T, Error>` and is marked `[[nodiscard]]`.

## Common Tasks

### Adding a New Feature

1. **Design the interface** in `include/oxide/module/`
2. **Implement** in `src/`
3. **Write tests** in `tests/`
4. **Update documentation**
5. **Submit PR** with clear description

### Debugging a Crash

1. **Build with debug symbols**
   ```bash
   cmake --preset debug-linux
   ```

2. **Run with debugger**
   ```bash
   gdb ./build/debug/Linux/apps/client/client
   ```

3. **Check logs** - All components use structured logging

### Performance Optimization

1. **Profile first** - Don't optimize blindly
2. **Measure impact** - Benchmark before/after
3. **Document changes** - Explain optimizations
4. **Test thoroughly** - Ensure correctness

## Build Configurations

### Debug Build
- Full debug symbols
- Assertions enabled
- Sanitizers active (ASan, UBSan)
- No optimizations

### Release Build
- Full optimizations
- LTO enabled
- Stripped binaries
- Still includes error checking

### Custom Configurations
See [Build System](build-system.md#custom-configurations) for creating specialized builds.

## IDE Setup

### VS Code
```json
{
    "cmake.configureOnOpen": true,
    "cmake.preferredGenerators": ["Ninja"],
    "C_Cpp.default.cppStandard": "c++23"
}
```

### CLion
- Open as CMake project
- Select appropriate preset
- Enable C++23 support

### Visual Studio
- Open folder
- CMake integration automatic
- Select preset from toolbar

## Troubleshooting

### "Cannot find oxide headers"
- Check `CMAKE_PREFIX_PATH`
- Ensure dependencies built
- Verify include paths

### "Undefined reference"
- Check library link order
- Verify all dependencies listed
- Ensure consistent Debug/Release

### "C++23 features not available"
- Update compiler
- Check CMake C++ standard setting
- Verify compiler flags

## Resources

- [Architecture Overview](../architecture/)
- [Getting Started Guide](../getting-started/)
- [API Reference](../api/)
- [Contributing Guidelines](../contributing/)

## Getting Help

- Check existing documentation
- Search closed issues/PRs
- Ask in development chat
- Create detailed bug report

Remember: **The C++ Manual is law.** When in doubt, refer to it.