# Oat Interactive C++ Standards (Summary)

## Core Rules

### Language (L)
- **C++23 only** - No exceptions (`-fno-exceptions`), no RTTI (`-fno-rtti`)
- **Factory functions only** - No public constructors on complex types
- **std::expected<T, Error>** - All fallible operations, never bool/nullptr
- **std::unique_ptr** - For ownership transfer

### Architecture (A)
- **3 layers**: Interface (`include/`), Implementation (`src/`), Tests
- **Abstract interfaces** - Pure virtual classes, mockable by design
- **No implementation leaks** - Private headers stay in `src/`

### API Design
- **struct for POD** - Simple value types with public constructors
- **class for complex** - Stateful objects with private constructors + factories
- **[[nodiscard, gnu::warn_unused_result]]
** - On all functions that return errors

### Error Handling
```cpp
enum class ErrorCode { Ok, InvalidArgument, ... };
struct Error { ErrorCode code; std::string msg; };

[[nodiscard("Handle this result!"), gnu::warn_unused_result]]
std::expected<std::unique_ptr<IFoo>, Error> create_foo();
```

### Factory Pattern
```cpp
// Interface (public header)
class ILogger {
public:
    virtual ~ILogger() = default;
    virtual void log(std::string_view msg) = 0;
};

[[nodiscard, gnu::warn_unused_result]]
 std::expected<std::unique_ptr<ILogger>, Error> 
create_logger(const Config& cfg);

// Implementation (private)
class LoggerImpl : public ILogger {
private:
    LoggerImpl(...);  // Private constructor
public:
    static std::expected<std::unique_ptr<ILogger>, Error> Create(...);
};
```

## Project Structure
```
include/company/module/  # Public APIs only
src/                     # All implementations  
tests/                   # Integration tests using public APIs
```

## Naming
- **Types**: `PascalCase`
- **Variables/functions**: `snake_case`  
- **Members**: `snake_case_` (trailing underscore)
- **Member functions**: `snake_case` (non-static), `CamelCase` (public static)
- **Interfaces**: `IClassName`
- **Implementations**: `ClassNameImpl`

## CMake
- **File sets** for headers (CMake 3.23+)
- **Version requirements** in find_package
- **No vendoring** - Use vcpkg/package managers

## Testing
- **Integration tests** - Use public APIs only
- **Mock interfaces** - Not implementations
- **Deterministic** - No network/filesystem dependencies

## Memory
- **Stack preferred** - RAII everything
- **No raw new/delete** - Use smart pointers
- **std::span** - For non-owning views

## Anti-Patterns ❌
- Public constructors on complex types
- Static Create() on interfaces  
- Exceptions for control flow
- `using namespace` anywhere
- Friend functions for factories
- Implementation inheritance for logic sharing
- Raw pointers for ownership

## Enforcement
- All factory functions catch exceptions → return Error
- [[nodiscard, gnu::warn_unused_result]]
 on all fallible operations
- Static asserts for struct sizes
- No dynamic_cast (RTTI disabled)