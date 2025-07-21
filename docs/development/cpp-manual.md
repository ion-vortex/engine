# 🧠 **Oat Interactive C++ Engineer Onboarding Manual (v1.0)**

# 📜 Table of Contents

*   [📰 Foreward](#-foreward)
*   [🔥 Philosophy](#-philosophy)
*   [🔧 Language Rules (L)](#-language-rules)
    *   [L.1: C++23 Standard](#l1-c23-standard)
    *   [L.2: Exceptions Banned](#l2-exceptions-banned)
    *   [L.3: Factory Functions for Stateful Objects](#l3-factory-functions-for-stateful-objects)
    *   [L.4: Ownership and error Propagation](#l4-ownership-and-error-propagation)
    *   [L.5: RTTI Off](#l5-rtti-off)
    *   [L.6: Header-Only Libraries Restricted](#l6-header-only-libraries-restricted)
*   [🏗️ Architectural Principles (A)](#️-architectural-principles)
    *   [A.1: Module Layers & Boundaries](#a1-module-layers--boundaries)
    *   [A.2: Interface Design](#a2-interface-design)
    *   [A.3: Implementation Details](#a3-implementation-details)
    *   [A.4: Reasoning-Driven APIs](#a4-reasoning-driven-apis)
*   [📁 Project Structure & Naming (PSN)](#-project-structure--naming)
    *   [PSN.1: Directory Layout](#psn1-directory-layout)
    *   [PSN.2: Namespaces](#psn2-namespaces)
    *   [PSN.3: File Naming Conventions](#psn3-file-naming-conventions)
    *   [PSN.4: One Class Per File (Mostly)](#psn4-one-class-per-file-mostly)
    *   [PSN.5: Header Guards](#psn5-header-guards)
*   [🧼 Coding Style & Hygiene (CSH)](#-coding-style--hygiene)
    *   [CSH.1: Naming Conventions (Types, Variables, Interfaces, etc)](#csh1-naming-conventions-types-variables)
    *   [CSH.2: No `using namespace`](#csh2-no-using-namespace)
    *   [CSH.3: Minimal Dependencies](#csh3-minimal-dependencies)
*   [📐 Public API Design (API)](#-public-api-design)
    *   [API.1: Minimal Entry Points](#api1-minimal-entry-points)
    *   [API.2: Construction Contract](#api2-construction-contract)
    *   [API.3: Use `struct` for POD](#api3-struct-pod)
    *   [API.4: error Codes](#api4-error-codes)
*   [⚙️ Runtime Behavior & Lifecycle (RT)](#️-runtime-behavior--lifecycle)
    *   [RT.1: Tick-Based Loops](#rt1-tick-based-loops)
    *   [RT.2: Time Representation](#rt2-time-representation)
    *   [RT.3: Allocation in Hot Paths](#rt3-allocation-in-hot-paths)
    *   [RT.4: Explicit I/O Boundaries](#rt4-explicit-io-boundaries)
*   [🔒 Memory, Ownership & Interoperability (MOI)](#-memory-ownership--interoperability)
    *   [MOI.1: Exposing Internal Buffers](#moi1-exposing-internal-buffers)
    *   [MOI.2: Explicit Object Lifetimes](#moi2-explicit-object-lifetimes)
    *   [MOI.3: STL in ABI](#moi3-stl-in-abi)
    *   [MOI.4: Mutable STL Containers in APIs](#moi4-mutable-stl-containers-in-apis)
*   [📦 Dependencies & Linking (DL)](#-dependencies--linking)
    *   [DL.1: No Dynamic Dispatch to 3rd Party](#dl1-no-dynamic-dispatch-to-3rd-party)
    *   [DL.2: Vendoring Policy](#dl2-vendoring-policy)
*   [🧱 CMake Is The Law (CM)](#-cmake-is-the-law)
    *   [CM.1: CMake Version](#cm1-cmake-version)
    *   [CM.2: FetchContent Compatibility](#cm2-fetchcontent-compatibility)
    *   [CM.3: Header Layout for CMake](#cm3-header-layout-for-cmake)
    *   [CM.4: Full Namespace Path Includes](#cm4-full-namespace-path-includes)
    *   [CM.5: Install Target Configuration](#cm5-install-target-configuration)
    *   [CM.6: `*_Config.cmake.in` Files](#cm6--_configcmakein-files)
*   [🧪 Testing, Stability & CI (TSC)](#-testing-stability--ci)
    *   [TSC.1: Integration Tests as Default](#tsc1-integration-tests-as-default)
    *   [TSC.2: Headless, Offline, Deterministic Tests](#tsc2-headless-offline-deterministic-tests)
    *   [TSC.3: Actionable Test Failures](#tsc3-actionable-test-failures)
    *   [TSC.4: Test Failure Modes](#tsc4-test-failure-modes)
    *   [TSC.5: Cross-Platform CI](#tsc5-cross-platform-ci)
*   [📢 Logging & Diagnostics (LOG)](#-logging--diagnostics)
    *   [LOG.1: `logger_base` Interface](#log1-logger-interface)
    *   [LOG.2: Structured & Prefixed Logs](#log2-structured--prefixed-logs)
    *   [LOG.3: Logging in Hot Paths](#log3-logging-in-hot-paths)
*   [📈 Telemetry / Metrics (MET)](#-telemetry--metrics)
    *   [MET.1: `metrics_base*` Injection](#met1-metrics-injection)
    *   [MET.2: Metric Naming](#met2-metric-naming)
*   [🧬 Versioning & Compatibility (VC)](#-versioning--compatibility)
    *   [VC.1: Version Constant](#vc1-version-constant)
    *   [VC.2: Backward Compatibility Handshake](#vc2-backward-compatibility-handshake)
    *   [VC.3: No API Versioning via Templates/Namespaces](#vc3-no-api-versioning-via-templatesnamespaces)
*   [💀 Dead-On-Arrival Patterns (DOA)](#-dead-on-arrival-patterns)
*   [✋ Stop Right There If... (SRT)](#-stop-right-there-if)
*   [📦 Final Word](#-final-word)

---

## 📰 Foreward
<a name="-foreward"></a>

This document is not a style guide. This is an engineering standard.

This document is about **shipping real software** in a hostile, cross-platform, multiplayer, real-time world.

Your abstractions must fail well.
Your systems must report their state clearly.
Your upgrades must never break clients unexpectedly.
Your runtime must be predictable, even under fire.

This isn't your university's C++ class.
This isn't your hobby project.
This is Oat Interactive.

Write like it.

You are building distributed systems, games, tools, and backends that run in real-time, at scale. **There is no room for confusion, compromise, or convention soup.**

If someone tells you "C++ can't do that," they're likely mistaken. If someone tells you "everyone does it this way," we strive to do better than "everyone."

You write tight, minimal, debug-friendly, cross-platform code. That's our standard.

---

## 🔥 Philosophy
<a name="-philosophy"></a>

This is not Unreal. This is not Godot. This is **not** a retrofitted game engine from 1999 duct-taped to a modern renderer.

**We use C++23 the way it was designed to be used:**

* No exceptions.
* No RTTI.
* No frameworks.
* No boost.
* No macros-as-APIs.
* No "magic" constructors that throw.
* No compromises on quality.

You write systems. You write libraries. You build fast, deterministic code that's easy to debug and easy to reason about.

> Welcome to Oat Interactive. We maintain high standards and expect excellence.

---

## 🔧 Language Rules (L)
<a name="-language-rules"></a>

### L.1. **Use C++23. No compromise.**
<a name="l1-c23-standard"></a>

If your compiler doesn't support `std::expected`, `consteval`, or `[[nodiscard]]` -- please upgrade your toolchain.

**Minimum standard: C++23.**
Anything older is a maintenance burden we choose not to accept.

---

### L.2. **Exceptions are banned from propagating. Period.**
<a name="l2-exceptions-banned"></a>

**No `throw` that escapes your module. No `try`/`catch` for flow control. No `noexcept(false)` on public API functions.**

The only permitted exception usage is:

*   In constructors that validate input (e.g. parsing a malformed IP in `addr("abc", 1234)`). These exceptions **MUST** be caught within the factory function (see L.3, API.2) and translated into an `std::expected` error before leaving the creating API boundary.
*   In third-party libraries (which we keep isolated and wrap cleanly, ensuring their exceptions are caught and translated to `std::expected` or handled internally).

All runtime code SHALL use `std::expected` for error propagation. See API.3 for more information.

```cpp
// Example of catching an internal exception in a factory
// static
[[OAT_NODISCARD("Handle this result! Failure to do so is a bug.")]]
std::expected<std::unique_ptr<my_object>, std::error_code> my_object::create(const std::string& config_data) {
    try {
        // my_object_impl constructor might throw on bad config_data
        auto impl = std::make_unique<my_object_impl>(config_data);
        return impl;
    } catch (const std::invalid_argument& e) {
        return std::unexpected(std::make_error_code(std::errc::invalid_argument));
    } catch (...) {
        return std::unexpected(std::make_error_code(std::errc::protocol_error));
    }
}

// Usage
std::expected<std::unique_ptr<my_object>, std::error_code> result = my_object::create("bad data");
if (!result) return result.error(); // Propagate error
```

This approach ensures predictable error handling throughout our codebase.

---

### L.3. **No public constructors for anything stateful.**
<a name="l3-factory-functions-for-stateful-objects"></a>

All real systems SHALL use static factory functions:

✅ Good:

```cpp
// In my_object.h
class my_object {
public:
    // Public interface methods...
    virtual ~my_object() = default;

    // Factory function
    [[OAT_NODISCARD("Handle this result! Failure to do so is a bug.")]]
    static std::expected<std::unique_ptr<my_object>, std::error_code> create(...);

protected:
    my_object() = default; // Or make constructors private/protected if my_object_impl is a friend/nested
};
```

🚫 Bad:

```cpp
session s(...);         // This approach can lead to problems if Session has complex state.
```

If you expose a public constructor, it SHALL be for pure POD/struct-only utility objects with no invariants and trivial construction (e.g., `buffer_view`, `addr`). These typically do not require a `std::unique_ptr` or `std::expected` for creation.

---

### L.4. **Use `std::unique_ptr` or `std::expected` exclusively for ownership and error propagation.**
<a name="l4-ownership-and-error-propagation"></a>

No raw `new`. No manual `delete`. No shared ownership by default.

If you pass ownership: use `std::unique_ptr<T>`.
If you return a result that might fail (including object creation): use `std::expected<T, std::error_code>`.
If you create an object that might fail: use `std::expected<std::unique_ptr<T>, std::error_code>`.

If you're writing `t* foo = new t(...)` and returning it or storing it in a raw pointer member, please reconsider your approach.

---

### L.5. **RTTI is off. Downcasting is banned.**
<a name="l5-rtti-off"></a>

No `dynamic_cast`. No `typeid`. No checking runtime types.

If your design needs polymorphism, use abstract interfaces (see A.2) with zero RTTI assumptions. For dynamic behavior, use a function pointer or a vtable (via abstract classes) as appropriate.

---

### L.6. **Header-only libraries are banned unless trivial.**
<a name="l6-header-only-libraries-restricted"></a>

If you're building real systems, separate your headers and your implementations. "Trivial" means:
*   A few simple, non-templated helper functions.
*   Basic struct/enum definitions.
*   No complex logic, significant state management, or heavy template metaprogramming that should be compiled into a library.

Headers define contracts. Source files implement them. If you're stuffing significant logic into a header file, you're exposing internals, bloating compile times, and spreading complexity.

**Keep headers clean and tight. All private implementation details belong in `src/`.**

---

## 🏗️ Architectural Principles (A)
<a name="-architectural-principles"></a>

### A.1. **Every system SHALL be split into 3 distinct layers: Interface, Implementation, and Tests.**
<a name="a1-module-layers--boundaries"></a>

These layers establish strict boundaries crucial for maintainability, testability, and clarity.

1.  **`[ Interface Layer ]`**
    *   **Location:** `include/oat/yourlib/...` (or similar public include path)
    *   **Content:** Public headers defining abstract classes (interfaces), POD-like configuration structs, public enums (like `ErrorCode`), and factory function declarations (e.g., `static std::expected<std::unique_ptr<type_base>, std::error_code> create(...)`).
    *   **Rule:** Interfaces expose **nothing private**. No implementation details, no private helper classes, no internal data structures. They define the "what," not the "how."

2.  **`[ Implementation Layer ]`**
    *   **Location:** `src/...`
    *   **Content:** Source files (`.cpp`) and private headers (`_impl.h`) containing the concrete implementations of the interfaces.
    *   **Rule:** Implementations are **invisible to consumers** of the library. Consumers only include from the public `include/` path and interact with the defined interfaces. Internal headers (`_impl.h`) are never installed and never included by public headers.

3.  **`[ Integration Tests Layer ]`**
    *   **Location:** `tests/...`
    *   **Content:** Test code that uses the public API of the library.
    *   **Rule:** Tests use the public API only, just like any other consumer. They interact with the interfaces defined in the `Interface Layer`. No "friend class" hacks to access private members of implementations. Mock dependencies by providing fake implementations of other interfaces, not by subclassing and overriding concrete implementations.

**No leaking internal state or implementation details across these walls.**

---

### A.2. **Interface Design: Abstract Classes as Contracts, Mockable by Design.**
<a name="a2-interface-design"></a>

1.  **Use Abstract Classes for Interfaces:**
    If you need polymorphism or to define a contract for a system component, use an abstract class with pure virtual functions.

    ```cpp
    // include/oat/service/protocol.h
    namespace oat::service {
        class protocol {
        public:
            virtual ~protocol() = default; // Always provide a virtual destructor for interfaces

            virtual void tick(uint64_t now_ns) = 0;

            [[OAT_NODISCARD("Handle this result! Failure to do so is a bug.")]]
            virtual std::expected<void, std::error_code> send_message(buffer_view payload) = 0;
            // ... other interface methods
        };
    } // namespace oat::service
    ```

2.  **Interfaces are Contracts, Not Base Classes with Logic:**
    If your "interface" (base class) has any non-trivial logic or member variables, it's no longer a pure interface--it's a base class for inheritance, which we discourage for complex hierarchies (see DOA). Interfaces define *what* a component does, not *how* it does parts of it.

3.  **Mockability is Paramount:**
    All interfaces SHALL be mockable for testing without resorting to reflection, complex macro magic, or overriding behavior in the *actual* implementation. Tests should be able to provide a completely separate, fake implementation of an interface to isolate the system under test.

    ✅ Good: Inject a `mock_protocol` that implements the `protocol` interface.
    🚫 Bad: Subclassing `protocol_impl` and overriding methods for a test. This is brittle and tests implementation details.

4.  **Callbacks (If Necessary) via `std::function`:**
    If an interface needs to signal events back to its owner, callbacks can be used. These SHALL use `std::function` and typically be passed into the factory function for the object implementing the interface. They should not generally be stored as mutable members that can be changed after construction, as this complicates lifetime and state reasoning.

    ```cpp
    // In configuration struct or factory function parameters:
    using on_message_received = std::function<void(buffer_view message)>;
    using on_disconnect = std::function<void(error reason)>;

    struct protocol_config {
        // ... other config ...
        on_message_received on_message_received_cb;
        on_disconnect on_disconnected_cb;
    };

    // static std::expected<std::unique_ptr<protocol>, std::error_code>
    // create_protocol(socket& transport, const protocol_config& config);
    ```

---

### A.3. **Implementation Details: Private and Encapsulated.**
<a name="a3-implementation-details"></a>

Implementations of interfaces live in the `src/` directory and are not exposed publicly.

1.  **Naming Convention:**
    A common pattern is to name the implementation class with an `Impl` suffix.
    *   Public Interface: `include/oat/net/session.h` -> `class session;`
    *   Private Implementation Header: `src/session_impl.h` -> `class session_impl : public oat::net::session { ... };`
    *   Private Implementation Source: `src/session_impl.cpp` -> `// Implementation of session_impl methods`

2.  **Private Headers:**
    Headers in `src/` (like `session_impl.h`) are for internal use by the implementation files within that library or module. They **MUST NOT** be included by any public header in `include/`. They are not part of the installation and are not visible to downstream consumers.

    ✅ Correct:
    ```cpp
    // src/session_impl.cpp
    #include "session_impl.h" // Internal header
    #include <oat/net/session.h> // Public interface it implements
    // ...
    ```

    🚫 Wrong (in a public header like `include/oat/net/session.h`):
    ```cpp
    // #include "session_impl.h" // NO! Do not expose implementation details.
    ```
    Forward declare types if necessary in public headers, but define them in `src/`.

---

### A.4. **Reasoning-Driven APIs: Clarity Above All.**
<a name="a4-reasoning-driven-apis"></a>

We don't design APIs to "feel modern" or to be overly concise at the expense of clarity. We design them to:

*   Be **debuggable**: Call chains should be clear. State changes should be traceable.
*   Be **testable**: Components should be injectable and mockable. Functionality should be verifiable in isolation.
*   Be **explicit**: No "magic" that happens behind the scenes. No hidden global state dependencies. What you see in the header is what you get.
*   Be **safe-by-default**: Minimize footguns. Make correct usage easy and incorrect usage hard (or impossible).
*   Be **predictable**: Given the same inputs and state, an operation should produce the same results (or errors). Avoid non-determinism hidden within API calls.

If a newcomer to the codebase cannot read a public header file and form a strong, accurate understanding of how to use the component and what its core behaviors and guarantees are, the API needs improvement. Ambiguity is a defect.

---

## 📁 Project Structure & Naming (PSN)
<a name="-project-structure--naming"></a>

Consistency in project layout and naming is critical for navigability, maintainability, and ensuring our CMake build system works smoothly across all libraries.

### PSN.1: **Directory Layout: Standardized for Clarity and CMake.**
<a name="psn1-directory-layout"></a>

All Oat Interactive libraries and systems SHALL adhere to the following primary directory structure at their root:

```
/your_library_name/
├── include/
│   └── oat/          # Or your top-level org namespace (e.g., pulse)
│       └── your_library_name/
│           └── public_header1.h
│           └── public_header2.h
│           └── sub_module/
│               └── public_header3.h
├── src/
│   ├── private_impl1.cpp
│   ├── private_impl1.h       # Internal header, not installed
│   ├── sub_module_impl.cpp
│   └── sub_module_impl.h     # Internal header for sub_module
├── tests/
│   ├── integration_test.cpp
│   ├── UnitTests.cpp         # For small, isolated utilities
│   └── mock_some_dependency.h # Test-specific mocks
├── CMakeLists.txt            # Root CMake file for the library
└── your_library_name_Config.cmake.in # For find_package (see CM.6)
```

*   **`include/`**: Contains *only* the public headers that form the API of your library. The subdirectory structure under `include/` (e.g., `oat/your_library_name/`) **must** match the namespace structure (see PSN.2 and CM.4).
*   **`src/`**: Contains all implementation (`.cpp`) files and any internal-only headers (`_impl.h` or similar). These files are not installed and are not directly accessible to consumers.
*   **`tests/`**: Contains all test code. Tests link against the library and use its public API.
*   **`CMakeLists.txt`**: The single CMake build script for the library, enabling it to be built standalone or incorporated via `FetchContent`.
*   **`*_Config.cmake.in`**: Template for CMake to generate a config file, allowing downstream projects to find and use your library via `find_package`.

---

### PSN.2: **Namespaces: Mirroring Directory Structure.**
<a name="psn2-namespaces"></a>

Namespaces provide logical grouping and prevent naming collisions. They SHALL mirror the public include path.

*   If a header is at `include/oat/net/session.h`, its contents should be within the `oat::net` namespace.

    ```cpp
    // File: include/oat/net/session.h
    namespace oat::net {
        class session { /* ... */ };
    } // namespace oat::net
    ```

*   ✅ Good: `pulse::net::proto::session`, `oat::core::asset::cache`
*   🚫 Bad (No single-letter namespaces): `oat::c::my_class` (unless `c` has a universally understood meaning in that context, which is rare).
*   🚫 Bad (Global namespace pollution): Avoid defining widely-used library types outside a specific namespace.

**Be explicit and descriptive with namespace components.**

---

### PSN.3: **File Naming Conventions: `snake_case` for everything.**
<a name="psn3-file-naming-conventions"></a>

This convention enhances readability and consistency.

| Element Type                      | Naming Convention | Example                     | Location Example                   |
| :-------------------------------- | :---------------- | :-------------------------- | :--------------------------------- |
| Public Header File                | `snake_case.h`    | `session_manager.h`         | `include/oat/net/session_manager.h`|
| Private Implementation Header     | `snake_case_impl.h`| `session_manager_impl.h`    | `src/session_manager_impl.h`       |
| Private Implementation Source     | `snake_case_impl.cpp`| `session_manager_impl.cpp`  | `src/session_manager_impl.cpp`     |
| Utility/Helper Header (Public)    | `snake_case.h`    | `buffer_utils.h`            | `include/oat/core/buffer_utils.h`  |
| Utility/Helper Source (Private)   | `snake_case.cpp`  | `string_helpers.cpp`        | `src/string_helpers.cpp`           |
| Test Source File                  | `PascalCaseTests.cpp` or `feature_tests.cpp` | `session_manager_tests.cpp`   | `tests/session_manager_tests.cpp`    |
| Class / Struct / Enum Class       | `PascalCase`      | `session_manager`            | (Defined inside headers/sources)   |
| Enum (C-style, if unavoidable)  | `ALL_CAPS_SNAKE`  | `MAX_CONNECTIONS`           | (Strongly prefer `enum class`)     |

*   Executable `main.cpp` files are acceptable for top-level application binaries or test executables, but not for library code.

---

### PSN.4: **One Primary Class/Interface Per File (Generally).**
<a name="psn4-one-class-per-file-mostly"></a>

*   Each major public interface (e.g., `session_manager`) or significant public class should generally reside in its own header file (e.g., `session_manager.h`).
*   Associated helper structs or enums that are tightly coupled and primarily used with that main class/interface can live in the same header.
    *   Example: `packet.h` might define multiple small packet structs that are all part of the same protocol module.
*   Avoid cramming multiple unrelated, complex classes or interfaces into a single header file. This increases coupling, compile times for unrelated changes, and makes the codebase harder to navigate and more prone to merge conflicts.

---

### PSN.5: **Header Guards: `#pragma once` is Mandatory.**
<a name="psn5-header-guards"></a>

All header files (`.h`, `_impl.h`) SHALL use `#pragma once` as the include guard mechanism.

```cpp
#pragma once

// ... rest of header file content ...
```

*   Yes, it's portable enough for our supported compilers (C++23 capable compilers all support it).
*   Yes, it's generally safer and less error-prone than traditional `#ifndef` guards.
*   This is our standard. Please use `#pragma once`.

---

## 🧼 Coding Style & Hygiene (CSH)
<a name="-coding-style--hygiene"></a>

While this document is an engineering standard, not a granular style guide, certain stylistic aspects directly impact readability, maintainability, and adherence to our core principles.

### CSH.1: **Naming Conventions – one rule: *everything* is `snake_case`**

The project now follows the **Boost / STL convention**: every identifier is lower‑snake‑case, with one small, explicit exception (ALL\_CAPS macros). No other casing schemes are permitted.

---

#### 1. **Types (classes, structs, enums, enum classes, type aliases)**

```cpp
class packet_parser;
struct channel_stats;
enum class packet_type { data, ack, keep_alive };
using user_id = std::uint64_t;
```

*Rationale*: matches Boost, Abseil, Folly, fmt, spdlog, range‑v3, etc. and eliminates “Pascal vs. snake” mental switching.

---

#### 2. **Interfaces (pure‑virtual abstract bases): `_base` suffix**

* Pure‑virtual interfaces **shall** end in `_base`.
* Example:

```cpp
class logger_base {
public:
    virtual void log(level lvl, std::string_view msg) = 0;
    virtual ~logger_base() = default;
};

class console_logger : public logger_base {
public:
    void log(level lvl, std::string_view msg) override;
};
```

*The suffix makes the role obvious without resorting to “Hungarian” `I`‑prefixes.*

---

#### 3. **Local variables and function parameters**

```cpp
void process_data(buffer_view incoming_payload) {
    auto last_recv_ns = get_current_time_ns();
    std::int32_t packet_count = 0;
    // …
}
```

---

#### 4. **Class member variables** – trailing underscore

```cpp
class user_session {
public:
    explicit user_session(user_id uid)
        : user_id_(uid), creation_time_ns_(get_current_time_ns()) {}

    user_id id() const { return user_id_; }

private:
    user_id user_id_;
    std::uint64_t creation_time_ns_;
    std::string session_token_;
};
```

---

#### 5. **Member functions**

* All member functions, including factories and important statics, use `snake_case`.

```cpp
class data_processor {
public:
    [[OAT_NODISCARD("handle this result – failure to do so is a bug.")]]
    static std::expected<std::unique_ptr<data_processor>, error>
    create(config& cfg);                         // factory, still snake_case

    void submit_data(buffer_view data);

    [[OAT_NODISCARD("handle this result – failure to do so is a bug.")]]
    bool is_processing() const;

    [[OAT_NODISCARD("handle this result – failure to do so is a bug.")]]
    std::optional<result> get_result();

private:
    void internal_process_queue();

    std::vector<buffer> queue_;
    bool is_active_;
};
```

---

#### 6. **Free (non‑member) functions**

```cpp
namespace oat::utils {
    std::uint64_t get_current_time_ns();
    std::string format_address(const addr& a);
}
```

---

#### 7. **Constants and `constexpr` variables:** `k_constant_name`

* Prefix all global, namespace‑scope, or `static` constants with `k_`.

```cpp
namespace oat::net {
    inline constexpr std::uint16_t   k_default_port       = 7777;
    inline constexpr std::size_t     k_max_packet_size    = 1200;
    inline constexpr std::string_view k_protocol_magic    = "OATP";
}
```

---

#### 8. **Macros:** `ALL_CAPS_SNAKE_CASE` (use sparingly)

```cpp
#define OAT_ENABLE_DEBUG_LOGGING 1
```

*Macros remain the only ALL\_CAPS identifiers; prefer `constexpr`, templates, or inline functions whenever possible.*

---

### CSH.2: **No `using namespace std;` or Broad `using namespace` in Headers.**
<a name="csh2-no-using-namespace"></a>

You are writing libraries and robust systems, not quick scripts. Unqualified names lead to ambiguity and namespace pollution.

*   **In Header Files (`.h`):**
    *   **Never** use `using namespace some_namespace;` at file scope or namespace scope.
    *   `using` declarations for *specific types* are acceptable if they significantly improve readability and are narrowly scoped (e.g., inside a class or function), but prefer full qualification.
        ```cpp
        // In a .h file - AVOID if possible, prefer std::vector
        // using std::vector; // Generally discouraged at namespace scope in headers

        namespace oat::data {
            // OK for very common types if restricted to this namespace, but still use with caution
            // using ByteVector = std::vector<std::byte>;

            class my_container {
                // OK within a class for a type alias
                using internal_map = std::unordered_map<int, std::string>;
                internal_map data_;
            public:
                std::vector<std::string> get_keys(); // Fully qualify std::vector
            };
        }
        ```

*   **In Source Files (`.cpp`):**
    *   `using namespace some_namespace;` can be used at function scope if it genuinely aids readability for a very localized block of code.
    *   `using SomeType = other_namespace::ActualType;` at file scope in a `.cpp` file is acceptable for commonly used types from other namespaces.
    *   Still, prefer explicit qualification (e.g., `std::cout`, `my_namespace::MyClass`) where ambiguity could arise or for less frequently used types.

**Default to explicit qualification. Import only what you use, and do so as locally as possible.**

---

### CSH.3: **Minimal Dependencies: Keep Headers Lean.**
<a name="csh3-minimal-dependencies"></a>

Strive to minimize the `#include` directives in your header files.

*   **Forward Declare When Possible:** If a header only needs to know that a type exists (e.g., for a pointer or reference member), forward declare it instead of including its full header.
    ```cpp
    // my_service.h
    #pragma once
    #include <memory> // For std::unique_ptr
    #include <oat/core/error_code.h> // For ErrorCode, error (Tagged Enum Wrapper)
    #include <oat/core/buffer_view.h> // For buffer_view

    // Forward declarations
    namespace oat::another_module { class helper_component; }
    struct config_data; // Assuming config_data is defined elsewhere

    namespace oat::my_system {
        class my_service {
        public:
            // ...
        private:
            std::unique_ptr<another_module::helper_component> helper_;
            const config_data* config_; // Only needs declaration of config_data
        };
    }
    ```
    The full definition for `helper_component` and `config_data` would be included in `my_service.cpp`.

*   **Include Only What You Use:** Don't `#include <vector>` if you're only using `std::string`. Don't pull in large, complex headers like `<algorithm>` or `<regex>` into a public API header unless absolutely essential for the *interface itself*.
*   **Rationale:** Reduces compile times, minimizes coupling, and makes it easier to understand the true dependencies of a module. We ship fast builds to fast systems.

---

## 📐 Public API Design (API)
<a name="-public-api-design"></a>

The public API is the contract your library or system provides to its consumers. It must be clear, robust, and enforce the principles of Oat Interactive engineering.

### API.1: **Minimal and Focused Entry Points: Avoid Overly Complex Signatures.**
<a name="api1-minimal-entry-points"></a>

APIs should expose narrow, well-defined entry points. Functions or methods with an excessive number of parameters are a code smell, indicating either a poorly abstracted component or a "kitchen sink" design.

1.  **Group Parameters with Config Structs:**
    If a factory function or method requires more than 3-4 parameters, group related parameters into a dedicated configuration struct. This struct should be a simple POD-like type.

    🚫 Wrong (Too many parameters):
    ```cpp
    // class network_stack;
    // static std::expected<std::unique_ptr<network_stack>, std::error_code>
    // create_network_stack(
    //     const std::string& bind_ip,
    //     uint16_t bind_port,
    //     size_t max_sessions,
    //     uint64_t session_timeout_ns,
    //     bool enable_encryption,
    //     const std::string& cert_path,
    //     logger_base* logger,
    //     metrics_base* metrics
    // );
    ```

    ✅ Right (Using a config struct):
    ```cpp
    // In network_stack_config.h (or similar)
    struct network_stack_config {
        std::string bind_ip = "0.0.0.0";
        uint16_t bind_port = 7777;
        size_t max_sessions = 1024;
        uint64_t session_timeout_ns = 30 * 1'000'000'000ULL; // 30 seconds
        bool enable_encryption = false;
        std::string cert_path; // Optional, only if encryption is enabled
        logger_base* logger = nullptr;   // Optional dependency
        metrics_base* metrics = nullptr; // Optional dependency
    };

    // In network_stack.h
    // class network_stack;
    // static std::expected<std::unique_ptr<network_stack>, std::error_code>
    // create_network_stack(const network_stack_config& config);
    ```

2.  **Rethink Architecture for Large Configs:**
    If your configuration struct itself has more than ~10-12 fields, it might indicate that the component is trying to do too much. Consider if the component can be broken down into smaller, more focused pieces with simpler configurations.

---

### API.2: **Object Construction: Use Factory Interfaces + `std::expected<std::unique_ptr<T>, std::error_code>`.**
<a name="api2-construction-contract"></a>

This is the **non-negotiable construction contract** for any non-POD, stateful, fallible object in an Oat Interactive library.

You SHALL:

* Prohibit public constructors on non-POD types.
* Use static factory methods (`create`) only on concrete classes, **never** on interfaces.
* Implement factories via interfaces (`logger_factory`, etc.) that produce `std::expected<std::unique_ptr<T>, std::error_code>`.
* Ensure factory methods catch all exceptions, including `...`, and wrap them in an `error`.

---

#### ✅ Correct: Interface with Factory Injection

```cpp
// public_interface.h
#pragma once

#include <memory>
#include <expected>
#include <oat/lib/error.h>

class logger_base {
public:
    virtual void log(std::string_view msg) = 0;
    virtual ~logger_base() = default;
};

class logger_factory {
public:
    [[OAT_NODISCARD("Handle this result! Failure to do so is a bug.")]]
    virtual std::expected<std::unique_ptr<logger_base>, std::error_code> create_logger() = 0;
    virtual ~logger_factory() = default;
};
```

---

#### ✅ Correct: Concrete Class with Private Constructor and Static `create()`

```cpp
// logger_impl.h (not exported publicly)
#pragma once

#include "public_interface.h"
#include <fstream>

class concrete_logger : public logger_base {
public:
    void log(std::string_view msg) override {
        stream_ << msg << '\n';
    }

    [[OAT_NODISCARD("Handle this result! Failure to do so is a bug.")]]
    static std::expected<std::unique_ptr<logger_base>, std::error_code> create(std::string_view path) {
        try {
            auto logger = std::unique_ptr<concrete_logger>(new concrete_logger(path));
            return std::expected<std::unique_ptr<logger_base>, std::error_code>{std::move(logger)};
        } catch (const std::exception& e) {
            return std::unexpected(std::make_error_code(std::errc::not_enough_memory));
        } catch (...) {
            return std::unexpected(std::make_error_code(std::errc::protocol_error));
        }
    }

private:
    std::ofstream stream_;

    explicit concrete_logger(std::string_view path) : stream_(std::string(path)) {
        if (!stream_) {
            throw std::runtime_error("Failed to open log file");
        }
    }

    // Explicitly disable copying
    concrete_logger(const concrete_logger&) = delete;
    concrete_logger& operator=(const concrete_logger&) = delete;
};
```

---

#### ✅ Correct: Factory Implementation That Uses the Private Concrete

```cpp
// logger_factory_impl.h
#pragma once

#include "public_interface.h"
#include "logger_impl.h"

class concrete_logger_factory : public logger_factory_base {
public:
    [[OAT_NODISCARD("Handle this result! Failure to do so is a bug.")]]
    std::expected<std::unique_ptr<logger_base>, std::error_code> create_logger() override {
        return concrete_logger::create("log.txt");
    }
};
```

---

#### 🏭 Global Factory Accessor Pattern

If you need to provide a default, globally available factory (e.g., for sockets or loggers), follow this model (or simply use C++17's inline variable model for globals):

```cpp
namespace oat::log {

namespace {
    logger_factory_base* current_factory = nullptr;
    concrete_logger_factory default_factory;
}

logger_factory_base* get_logger_factory() {
    if (!current_factory) current_factory = &default_factory;
    return current_factory;
}

logger_factory_base* set_logger_factory(logger_factory_base* new_factory) {
    logger_factory_base* previous = current_factory;
    current_factory = new_factory;
    return previous;
}

void reset_logger_factory() {
    current_factory = nullptr;
}

} // namespace oat::log
```

---

#### 🧪 What About Testing?

* For tests, you SHALL provide your own mock factories directly.
* Example:

  ```cpp
  class mock_logger : public logger_base { /* ... */ };
  class mock_logger_factory : public logger_factory_base {
  public:  
      [[OAT_NODISCARD("Handle this result! Failure to do so is a bug.")]]
      std::expected<std::unique_ptr<logger_base>, std::error_code> create_logger() override {
          return std::make_unique<mock_logger>();
      }
  };
  ```

* Don't add `set_x_factory` just so that your tests can inject a mock logger factory. Just use the `mock_logger_factory` locally.

---

#### 🧱 Requirements Summary

| Rule                                                                     | Enforcement                                          |
| ------------------------------------------------------------------------ | ---------------------------------------------------- |
| Non-POD types SHALL NOT have public constructors                         | Use `protected` or `private`                         |
| Factory interface must return `std::expected<std::unique_ptr<T>, std::error_code>` | No `new` or `make_unique` in user code               |
| Static `create()` SHALL only be declared on concrete implementations     | Never put static factories on interfaces             |
| Static `create()` MUST catch `std::exception` and `...`                  | Always return `std::unexpected<error>` on failure    |
| Friend declarations SHALL NOT be used for factories                      | Avoid coupling, expose via factory interface instead |

---

#### 🚫 Anti-Patterns

##### ❌ Static `create()` on Interface

```cpp
class logger_base {
public:
    static std::unique_ptr<logger_base> create(); // ❌ Please avoid this pattern
};
```

This design is problematic because:

* Interfaces have no business knowing their own implementations.
* You can't mock it.
* It hardwires the implementation to the consumer.
* You now have a magic static that can't be replaced or tested.

##### ❌ Public Constructors on Complex Types

```cpp
class bad_thing {
public:
    bad_thing(int val); // ❌ Not recommended unless it's a trivial POD-style value object
};
```

---

#### ✅ What This Enables

* Clean DI with no magic.
* Clear mocking boundaries.
* No chance of runtime exception leaks.
* Flexible ownership transfer via `std::unique_ptr`.
* Compile-time enforcement of object lifecycle.

---

Here's the final addition:

---

### API.3: **Explicit POD Declaration: Use `struct` for Trivial Types with Public Constructors.**
<a name="api3-struct-pod"></a>

If a type is trivial and its constructors are public and cannot throw, or is POD you SHALL:

* Declare it as a `struct`, not a `class`.

This makes the purpose of the type immediately obvious: You don't need a factory to create it because it's safe to do so with the constructors.

#### ✅ Correct Example:

```cpp
struct point {
    int x = 0;
    int y = 0;

    point() = default;
    point(int x_val, int y_val) : x(x_val), y(y_val) {}
};
```

#### ❌ Incorrect Example:

```cpp
class point { // ❌ Please use struct for POD types
public:
    point(int x, int y) : x_(x), y_(y) {}
private:
    int x_;
    int y_;
};
```

| Rule                | Enforcement      |
| ------------------- | ---------------- |
| Trivial/POD Types   | Use `struct`     |
| Complex Stateful    | Use `class`      |
| Public Constructor? | Only on `struct` |

This keeps the API surface clean and makes it immediately obvious what's a value type and what's a managed resource. No one should ever have to ask, "Can I just copy this, or does it secretly manage heap resources?"

---

### API .4 **error handling – standard-compliant, type-safe, and compiler-enforced**
<a id="api4-error-codes"/>  

All public functions that can fail **shall**

| Requirement                                                                                                                             | Rationale                                                                                                                         |
| --------------------------------------------------------------------------------------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------- |
| **Return** `std::expected<T, std::error_code>` (or `std::expected<void, std::error_code>`).                                             | The value half (`T`) remains unconstrained; the error half is the *portable* C++11 struct whose layout is ABI-stable across DLLs. |
| **Tag** every error code with a *module-local* strongly-typed `enum class`, then convert it to `std::error_code` via `make_error_code`. | Preserves type-safety at call sites while keeping one on-the-wire format.                                                         |
| **Mark** every fallible API with `OAT_NODISCARD("…descriptive message…")`.                                                              | Forces callers to handle the result.                                                                                              |

`bool`, sentinels, `nullptr`, raw integers, or `std::optional` **shall not** be used for error signalling. Exceptions remain an internal implementation detail; they do **not** cross public ABI boundaries.

---

#### 1.  Per-module error enum and category (*one page of boiler-plate*)

```cpp
// include/oat/net/net_error.h
#pragma once
#include <system_error>

namespace oat::net {

// 1. Strongly-typed enum (values start at 1 – zero means “success”)
enum class net_errc {
    connection_failed = 1,
    timeout,
    packet_too_large,
    unknown,
};

// 2. Category object – immortal, one per dynamic image
class net_category : public std::error_category {
public:
    const char*     name()    const noexcept override { return "oat.net"; }
    std::string     message(int ev) const override;
};
inline const net_category k_net_category;

// 3. make_error_code + trait – enables implicit conversion
inline std::error_code make_error_code(net_errc e) noexcept {
    return {static_cast<int>(e), k_net_category};
}
} // namespace oat::net

template<> struct std::is_error_code_enum<oat::net::net_errc> : std::true_type {};
```

```cpp
// net_error.cpp
#include "net_error.h"
namespace oat::net {
std::string net_category::message(int ev) const
{
    using E = net_errc;
    switch (static_cast<E>(ev)) {
        case E::connection_failed: return "connection failed";
        case E::timeout:           return "timeout";
        case E::packet_too_large:  return "packet too large";
        default:                   return "unknown network error";
    }
}
} // namespace oat::net
```

*Call-site safety remains intact:*

```cpp
if (auto r = connect(...); !r && r.error() == oat::net::net_errc::timeout) { … }
```

---

#### 2.  Adapting platform errors (POSIX/Win32) to module enums

```cpp
// low-level socket wrapper (Windows & POSIX variants)
static std::error_code last_os_error() noexcept {
#ifdef _WIN32
    return {static_cast<int>(::GetLastError()), std::system_category()};
#else
    return {errno, std::generic_category()};
#endif
}

static std::error_code to_net_error(std::error_code ec) noexcept {
    using E = oat::net::net_errc;

    if (ec.category() == std::generic_category()) {
        switch (ec.value()) {
            case ETIMEDOUT: return E::timeout;
            case EMSGSIZE:  return E::packet_too_large;
            default:        return E::Unknown;
        }
    }
    if (ec.category() == std::system_category()) {
        switch (ec.value()) {
            case WSAETIMEDOUT:      return E::timeout;
            case WSAEMSGSIZE:       return E::packet_too_large;
            default:                return E::Unknown;
        }
    }
    return ec;          // already oat-internal or unrecognised
}
```

*Exported API*:

```cpp
[[OAT_NODISCARD("check connect result")]]
std::expected<void, std::error_code> connect(socket s, address a)
{
    if (::connect(s.fd(), …) == 0)  return {};
    return std::unexpected(to_net_error(last_os_error()));
}
```

No caller ever sees `errno` or `DWORD`, only `oat::net::net_errc`.

---

#### 3.  Defining a descriptive, portable `nodiscard`

```cpp
// oat/attributes.h
#pragma once
#if defined(__GNUC__) || defined(__clang__)
#  define OAT_NODISCARD(msg) nodiscard(msg), gnu::warn_unused_result
#elif defined(_MSC_VER)
#  define OAT_NODISCARD(msg) nodiscard(msg)
#else
#  define OAT_NODISCARD(msg) nodiscard
#endif
```

*Usage examples*:

```cpp
OAT_NODISCARD("handle the possible timeout")
std::expected<void, std::error_code> wait_for_ready(...);

OAT_NODISCARD("did the asset actually load?")
std::expected<asset, std::error_code> load_asset(std::string_view path);
```

Each message explains *why* the caller shouldn’t ignore the value.

---

#### 4.  Standard patterns

| Pattern                            | Example                                                                                         |
| ---------------------------------- | ----------------------------------------------------------------------------------------------- |
| **Value-bearing result**           | `std::expected<Image, std::error_code> load_png(Path p);`                                       |
| **Void success**                   | `std::expected<void, std::error_code> flush();`                                                 |
| **Wrapping exceptions internally** | `cpp try { … } catch (const std::bad_alloc&) { return std::unexpected(std::errc::not_enough_memory); } ` |
| **Comparing errors**               | `if (ec == our_errc::our_error) { … }` (typed, no magic numbers)                                  |
| **Logging**                        | `log.error("flush failed: {}", ec);` (both `fmt` and `spdlog` format `error_code` natively).    |

---

#### 5.  Practices to avoid

* Returning `bool`, `nullptr`, sentinel integers, or `std::optional` as error indicators.
* Exposing platform categories (`std::generic_category`, `std::system_category`) beyond the platform layer.
* Throwing exceptions across module boundaries.
* Re-using one enum for multiple domains—create a fresh enum + category per sub-system.

---

### Why this approach is preferred

* **ABI-safe:** `std::error_code` is a trivial two-word POD; identical across compilers and DLLs.
* **Zero allocations:** no embedded `std::string` in the transport.
* **Interoperable:** STL, Boost, gRPC and many third-party libraries already consume/produce `std::error_code`.
* **Extensible:** each module owns its enum; no global registry or range reservation is required.
* **Descriptive:** `message()` delivers human-readable text on demand; not stored unless logged.
* **Enforced handling:** `OAT_NODISCARD` ensures the compiler reminds users to check every result.

Adhering to these rules yields clear, efficient, and portable error handling across all OAT C++ components.


---

## ⚙️ Runtime Behavior & Lifecycle (RT)
<a name="-runtime-behavior--lifecycle"></a>

Oat Interactive systems are designed for real-time, predictable performance. Runtime behavior must be explicit and deterministic.

### RT.1: **Tick-Based, Deterministic Loops as the Primary Update Mechanism.**
<a name="rt1-tick-based-loops"></a>

Many Oat Interactive systems, especially those dealing with networking, game logic, or time-sensitive operations, SHALL use a `tick()`-based update model.

1.  **`tick()` Method:**
    Core components expose a `tick(uint64_t current_time_ns, /* other inputs like events */)` method.
    *   This method is called repeatedly by an external loop (e.g., the main application loop, a dedicated system scheduler).
    *   Each call to `tick()` represents a single, discrete step of work for the component.
    *   The component should process pending inputs, update its internal state, and produce outputs based on the current time and inputs provided *during that specific tick*.

    ```cpp
    // class real_time_protocol {
    // public:
    //     // ... other methods ...
    //     virtual void tick(uint64_t current_time_ns) = 0;
    //     virtual void on_socket_readable() = 0; // Called by I/O layer when data is available
    //     // ...
    // };
    ```

2.  **No Internal Threads for Core Logic:**
    Individual libraries or components (like a packet parser, a session manager, a game entity system) **SHALL NOT** spawn their own threads for their primary operational logic.
    *   **Rationale:** Internal threading makes reasoning about state, determinism, and debugging significantly harder. It also complicates integration into larger systems that have their own threading models.
    *   If concurrency is needed, it is the responsibility of the top-level system or application to manage threads and dispatch `tick()` calls or events to components, potentially across multiple threads if the design explicitly supports safe concurrent access (which must be very carefully considered).

3.  **No Hidden `async` Operations:**
    Avoid asynchronous patterns (`std::async`, custom promise/future chains deeply embedded in library logic) that obscure the flow of execution or introduce hidden concurrency.
    *   If an operation is inherently asynchronous (like non-blocking I/O), its completion should typically result in an event or state change that is processed in a subsequent `tick()` call.

**The goal is: `tick()` -> Perform one quantum of work -> Return. Predictable. Testable.**

---

### RT.2: **Time Representation: `uint64_t` Nanoseconds Exclusively in APIs.**
<a name="rt2-time-representation"></a>

All time values passed into or returned from public API functions (especially for `tick()` methods, timeouts, timestamps) SHALL be represented as `uint64_t` denoting nanoseconds since an arbitrary epoch (usually `std::chrono::steady_clock` epoch).

1.  **No `std::chrono` Objects in Public APIs:**
    While `std::chrono` is excellent for internal time calculations and obtaining the current time, do not expose `std::chrono::duration` or `std::chrono::time_point` types directly in public API function signatures.
    *   **Rationale:**
        *   It forces a specific C++ standard library feature onto all consumers, including those potentially in C or other languages via FFI.
        *   It can lead to confusion about which clock or precision is implied if not rigorously documented.
        *   `uint64_t` is simple, unambiguous, and universally portable.

2.  **Obtaining Current Time:**
    The standard way to get current time for internal use (which is then converted to `uint64_t` for APIs):
    ```cpp
    #include <chrono>

    // Inside a method or function:
    auto now_steady_tp = std::chrono::steady_clock::now();
    uint64_t now_ns = static_cast<uint64_t>(now_steady_tp.time_since_epoch().count());
    // Note: std::chrono::steady_clock::period is often std::nano, so count() might be nanoseconds.
    // If not, convert explicitly:
    // uint64_t now_ns = static_cast<uint64_t>(
    //    std::chrono::duration_cast<std::chrono::nanoseconds>(
    //        now_steady_tp.time_since_epoch()
    //    ).count()
    // );
    ```
    **Always use `std::chrono::steady_clock` for measuring durations or for `tick()` timing, as it's not affected by system time changes.** `std::chrono::system_clock` can be used for wall-clock timestamps that need to be human-readable or synchronized across systems, but be aware of its properties.

3.  **Clarity in APIs:**
    Parameter names should make the unit clear: `timeout_ns`, `current_time_ns`, `event_timestamp_ns`.

---

### RT.3: **Allocation in Hot Paths: Avoid or Manage Explicitly.**
<a name="rt3-allocation-in-hot-paths"></a>

In performance-critical code paths, such as inside a `tick()` method that runs frequently, or in packet processing loops, dynamic memory allocations (`new`, `std::vector::resize` causing reallocation, `std::string` manipulations that reallocate) SHALL be avoided unless absolutely necessary and their cost understood.

1.  **Preallocate:** Allocate necessary buffers or object pools during initialization or less frequent setup phases.
2.  **Use Pools:** For frequently created/destroyed objects of the same type, use object pools to recycle memory.
3.  **Pass in Buffers:** Systems that process data (e.g., parsers, serializers) should often accept pre-allocated output buffers from the caller.
4.  **Reuse Resources:** Loggers, metrics collectors, and similar auxiliary objects should be passed in as dependencies (often via constructor or factory) and reused, not created on-the-fly in hot paths.
5.  **Profile and Justify:** If allocation in a hot path is unavoidable, it must be justified by design constraints, and its performance impact should be measured and deemed acceptable.

**The runtime owns nothing it doesn't have to. Components operate on provided data or pre-allocated resources.**

---

### RT.4: **Explicit I/O Boundaries: No Hidden Wire Activity.**
<a name="rt4-explicit-io-boundaries"></a>

Interactions with external I/O (network, disk, etc.) SHALL be explicit and visible at the API level. Avoid designs where a simple method call on an object implicitly triggers I/O operations without the caller's direct intent.

1.  **Clear Send/Receive Operations:**
    Network protocols should have clear `send_packet(buffer_view payload)` or `receive_data(mutable_buffer_view buffer)` style methods. The act of sending or attempting to receive is an explicit API call.

    ✅ Good:
    ```cpp
    // session.send_reliable(CHANNEL_ID, payload_view);
    // protocol.tick(now_ns); // tick() might internally process ACKs, retransmits, or new incoming data
    // socket.read(buffer_for_new_data); // Explicit read from socket
    ```

2.  **State Machines and I/O:**
    If a component manages a state machine that results in network I/O (e.g., sending an ACK, a keep-alive), this I/O should ideally be funneled through an explicit abstraction provided to it (e.g., a `transport` interface) or happen as a result of its `tick()` method processing events. The caller of `tick()` understands that work, potentially including I/O, will be done.

3.  **Control Over Flushing:**
    Give the caller control over when data is actually flushed to the wire, if applicable. Some systems might buffer sends until an explicit `flush()` call or until the `tick()` method processes the send queue.

**Users of your library must be able to reason about when I/O occurs. No invisible network chatter from simple state-querying method calls.**

---

## 🔒 Memory, Ownership & Interoperability (MOI)
<a name="-memory-ownership--interoperability"></a>

Correctly managing memory, defining clear ownership, and ensuring robust interoperability (especially at ABI boundaries) are paramount for stable and secure systems.

### MOI.1: **Never Expose Internal Mutable Buffers Without Surrendering Ownership or Using Safe Views.**
<a name="moi1-exposing-internal-buffers"></a>

Internal data structures, especially buffers or collections, must be protected from uncontrolled external modification.

1.  **No Direct Mutable References to Internals:**
    Do not return non-const references or raw pointers to internal containers (like `std::vector& getInternalQueue()`) if the caller is not meant to take ownership or if their modification would break invariants.

    🚫 Bad (Breaks encapsulation and invariants):
    ```cpp
    // class my_queue {
    // private:
    //     std::vector<packet> queue_;
    // public:
    //     std::vector<packet>& get_raw_queue() { return queue_; } // NO! Caller can now corrupt state.
    // };
    ```

2.  **Use Const Views for Read-Only Access:**
    If you need to provide read-only access to internal data, return a const reference, a `buffer_view` (for raw data), `std::span<const T>`, or `std::string_view`.

    ✅ Good (Read-only access):
    ```cpp
    // class my_data_store {
    // private:
    //     std::vector<item> items_;
    //     std::string internal_buffer_;
    // public:
    //     std::span<const item> get_items_view() const { return items_; }
    //     buffer_view get_internal_buffer_view() const { return buffer_view(internal_buffer_.data(), internal_buffer_.size()); }
    // };
    ```

3.  **Controlled Mutation via Methods:**
    If external entities need to modify state, they MUST go through well-defined public methods that can enforce invariants.

4.  **Surrendering Ownership via `std::unique_ptr`:**
    If a component generates data that the caller should own and manage, return it by `std::unique_ptr` (if heap-allocated) or by value (if small and copyable), often wrapped in `std::expected`.

    ```cpp
    // std::expected<std::unique_ptr<data_object>, std::error_code> create_data_object();
    // std::expected<copied_result, std::error_code> process_and_return_copy();
    ```

**Your component owns its internal state. External interaction happens through controlled interfaces.**

---

### MOI.2: **Explicit Object Lifetimes: Enforced by Factory and Ownership.**
<a name="moi2-explicit-object-lifetimes"></a>

The lifetime of objects created by your library must be clear and managed correctly, primarily through the `std::unique_ptr` returned by factory functions.

1.  **Safe to Destroy Anytime (Post-Creation):**
    Objects returned via `std::expected<std::unique_ptr<T>, std::error_code>` by a factory function (`create_x()`) MUST be safe for the caller to destroy (by letting the `std::unique_ptr` go out of scope) at any point after successful creation, without requiring additional cleanup calls on other objects *unless explicitly documented as a coupled lifetime*.
    *   Avoid designs where destroying object A in isolation causes crashes because object B (not owned by A's `unique_ptr`) still holds a raw pointer to A's internals without a proper observer/subscriber removal mechanism.

2.  **No `.init()` Calls After Construction via Factory:**
    An object obtained from a factory function (`create_x()`) MUST be in a fully usable state. It SHALL NOT require a subsequent `.init()` or `.start()` call to become functional for its basic advertised contract.
    *   Any setup that can fail or is essential for the object's initial valid state MUST happen *within* the factory function (or the constructor called by the factory) and be covered by the `std::expected` error handling.
    *   Methods like `.start()`, `.connect()`, or `.open()` are acceptable if they represent a distinct state transition *after* initial valid construction (e.g., an idle network session object calling `.connect()`).

**The factory function is the gatekeeper for valid, usable objects.** If it returns success, the object is ready.

---

### MOI.3: **STL Types in Exported ABI: Extreme Caution, Prefer Stable Views or Owned Data.**
<a name="moi3-stl-in-abi"></a>

Exposing Standard Library (STL) types directly in the Application Binary Interface (ABI) of your shared libraries (`.dll`, `.so`, `.dylib`) is fraught with peril due to potential inconsistencies between compiler versions, standard library implementations, and build settings (like `_ITERATOR_DEBUG_LEVEL` on MSVC).

1.  **Avoid `std::string`, `std::vector`, etc., in Public Function Signatures of Exported APIs if possible:**
    *   If you *must* pass string data, prefer `const char*` + `size_t` (or your `buffer_view`/`string_view`-like type that is ABI-stable) for input.
    *   For outputting string or collection data where the caller takes ownership, you might need to provide C-style allocator functions or return opaque pointers that are managed via C-style API calls if maximum ABI stability is paramount.
    *   However, within the Oat Interactive ecosystem, if all components are built with the same compiler toolchain and C++ standard, using `std::string` by value or const reference *can* be acceptable for convenience, but be aware of the tight coupling it creates.

2.  **`buffer_view` / `std::span`-like Types are Preferred for Views:**
    For passing non-owning views of data across API boundaries, use a simple, ABI-stable struct like `buffer_view { const std::byte* data; size_t size; }` or `std::span` (if C++20 `std::span` is deemed ABI-stable enough *for your toolchain and deployment environment* – C++23 `std::span` is better but check compiler support).
    *   **`std::string_view` lifetime:** Be extremely careful. A `std::string_view` is only valid as long as the underlying buffer it points to is valid. Returning a `std::string_view` that points to a temporary `std::string`'s internal buffer is a dangling pointer waiting to happen.

3.  **Full Ownership Transfer for Complex Types:**
    If an API needs to return a complex STL object (like `std::vector`) and the caller owns it, often the safest (though potentially less performant) way across a hard ABI boundary is to return it by value (if it's movable and the move is efficient) or serialize it to a C-style buffer. For internal Oat Interactive libraries built together, `std::expected<std::vector<my_pod>, std::error_code>` can be fine.

**Rule of Thumb for Shared Libraries:** The "C" subset of C++ is the most stable ABI. If you expose complex C++ types, you are implicitly tying yourself to a specific compiler toolchain and settings for ABI compatibility. For Oat Interactive internal modules built together, this is less of a concern than for libraries distributed to third parties.

---

### MOI.4: **Never Expose Mutable STL Containers Directly in Public APIs.**
<a name="moi4-mutable-stl-containers-in-apis"></a>

This is a reiteration of MOI.1 but specifically for STL containers, as they are common culprits.

🚫 Bad:
```cpp
// class thing_manager {
// public:
//     std::vector<std::string>& get_mutable_names(); // NO! Client can break invariants.
//     const std::map<int, data_object>& get_internal_map() const; // OK for const access
// };
```

If a client needs to add or remove items, provide methods:
```cpp
// class thing_manager {
// public:
//     void add_name(std::string_view name);
//     bool remove_name(std::string_view name);
//     std::vector<std::string> get_all_names() const; // Returns a copy for safety
// };
```
Returning by copy can be expensive for large collections. Alternatives include:
*   Providing iterators to a const container.
*   Using an observer pattern.
*   Designing the API so the client doesn't need direct access to the whole mutable container.

**Prioritize encapsulation and controlled state changes over direct mutable access to internal collections.**

---

## 📦 Dependencies & Linking (DL)
<a name="-dependencies--linking"></a>

Managing dependencies and ensuring robust linking behavior is crucial for creating maintainable, deployable, and hassle-free systems. Oat Interactive prioritizes minimal, well-defined dependencies and build system integration.

### DL.1: **No Dynamic Dispatch to Optional 3rd-Party Code Without Explicit Injection.**
<a name="dl1-no-dynamic-dispatch-to-3rd-party"></a>

If your library has an *optional* dependency on a third-party library (e.g., a specific logging library, a metrics backend, a compression algorithm not always needed), this dependency MUST be provided as an interface that the user can implement or inject. Your system should not attempt to `dlopen`/`LoadLibrary` and `dlsym`/`GetProcAddress` third-party symbols directly as its primary integration mechanism for *optional core functionality*.

1.  **Inject Interfaces for Optional Services:**
    Core services like logging, metrics, or specialized processing that might have different backends should be abstracted behind an interface. The user of your library provides the concrete implementation.

    ```cpp
    // Your library provides these interfaces:
    // include/oat/core/logger.h
    class logger_base {
    public:
        virtual ~logger_base() = default;
        virtual void log(log_level level, const std::string& message) = 0;
    };

    // include/oat/core/metrics.h
    class metrics_base {
    public:
        virtual ~metrics_base() = default;
        virtual void increment(const std::string& name, uint64_t value = 1) = 0;
        virtual void gauge(const std::string& name, double value) = 0;
    };

    // Your system takes these via config or factory:
    // struct my_system_config {
    //     logger_base* logger = nullptr;   // Can be null
    //     metrics_base* metrics = nullptr; // Can be null
    //     // ...
    // };
    ```

2.  **Fallback to No-Op Implementations:**
    If an optional dependency (like `logger_base*` or `metrics_base*`) is not provided (i.e., is `nullptr`), your system SHALL gracefully fall back to internal no-op (no-operation) implementations. These no-op versions do nothing, ensuring the system functions correctly without the optional feature.

    ```cpp
    // Inside your system's implementation:
    // if (config_.logger) {
    //     config_.logger->log(log_level::info, "System started.");
    // }
    // // No else needed, or use a noop_logger instance.
    ```
    Your library can even provide default `noop_logger` and `noop_metrics` implementations.

3.  **Rationale:**
    *   **Decoupling:** Your library doesn't have a hard compile-time or link-time dependency on specific third-party implementations.
    *   **Testability:** Easy to inject mock loggers/metrics for testing.
    *   **Flexibility:** Consumers can integrate with their preferred logging/metrics systems.
    *   **Avoids Linkage Hell:** Prevents issues with multiple versions of the same third-party library in a larger application.
    *   **Clearer Dependencies:** The need for a logger/metrics system is explicit in your API (via the config struct or factory parameters).

This rule primarily applies to *optional* dependencies that alter behavior or provide auxiliary services. *Required* third-party libraries are handled differently (see DL.2).

---

### DL.2: **Dependency Acquisition: CMake `FetchContent` Preferred; Header-Only Vendoring as a Last Resort.**
<a name="dl2-vendoring-policy"></a>

Oat Interactive aims for a streamlined and reproducible build process.

1.  **`FetchContent` is the Standard:**
    For required third-party C++ libraries, the primary method of integration SHALL be CMake's `FetchContent` module (or `FetchContent_MakeAvailable`).
    *   This ensures that dependencies are downloaded and built as part of your project's configure/build process, using consistent compiler settings.
    *   It avoids "DLL hell" or "ABI nightmare" scenarios by typically building dependencies from source or using pre-compiled versions compatible with your build.

    ```cmake
    # CMakeLists.txt
    include(FetchContent)

    FetchContent_Declare(
        mylib_dep_name
        GIT_REPOSITORY https://github.com/some_org/some_lib.git
        GIT_TAG        v1.2.3 # Or a specific commit hash
    )
    FetchContent_MakeAvailable(mylib_dep_name)

    # target_link_libraries(your_oat_lib PUBLIC mylib_dep_name::target)
    ```

2.  **Header-Only Libraries: Vendoring Permitted (with caveats):**
    *   Small, truly header-only libraries (e.g., stb_image, a single-header JSON parser) MAY be "vendored" – i.e., their header(s) copied directly into a `third_party/` or `extern/` subdirectory within your library's repository.
    *   **Conditions for Vendoring:**
        *   The library must be genuinely header-only (no separate `.cpp` files to compile).
        *   It should be relatively stable and not updated excessively frequently (to avoid constant manual updates).
        *   Its license must be compatible with your project.
        *   It should be isolated (e.g., `#include "third_party/json.h"`), and its symbols should ideally not pollute your library's primary namespaces if it defines its own.
    *   `FetchContent` is still often cleaner even for header-only libraries if they provide good CMake support, as it centralizes version management.

3.  **Pre-compiled Binaries: Strongly Discouraged for Dependencies:**
    Checking pre-compiled third-party binaries (`.lib`, `.dll`, `.so`, `.a`) directly into your repository is strongly discouraged. This practice is a major source of cross-platform build issues, ABI incompatibilities, and licensing headaches. `FetchContent` building from source is vastly superior.

4.  **System Package Managers (Conan, vcpkg): External to the Library's Build.**
    While developers *consuming* your Oat Interactive library might use system-wide package managers like Conan or vcpkg to satisfy dependencies (including your library itself, if packaged), your library's *own* `CMakeLists.txt` should primarily rely on `FetchContent` for its direct C++ dependencies. This makes the library self-contained and easier to integrate into diverse build environments.
    *   Your library should not *require* the user to have Conan or vcpkg installed just to build it from source using `FetchContent`.

**We have extensive experience with dependency management challenges. Please use `FetchContent` or `vcpkg` for critical, non-trivial dependencies to ensure a smooth build experience for everyone.**

---

## 🧱 CMake Is The Law (CM)
<a name="-cmake-is-the-law"></a>

CMake is the build system for all Oat Interactive C++ projects. Adherence to these CMake practices is mandatory for interoperability, maintainability, and sanity.

### CM.1: **You SHALL use CMake ≥ 3.15 (Preferably Latest Stable).**
<a name="cm1-cmake-version"></a>

If you're using a CMake version older than 3.15, please upgrade. We rely on modern CMake features for robust build logic, dependency management (`FetchContent`), and target-based property management. Aim to use a recent version (e.g., 3.20+ if possible) for access to the latest improvements.

**Check your CMake version: `cmake --version`.**

---

### CM.2: **Every Project SHALL be CMake-Fetchable via `FetchContent`.**
<a name="cm2-fetchcontent-compatibility"></a>

This is the cornerstone of our dependency management strategy. Every Oat Interactive library must be easily consumable by other Oat Interactive projects (and external users) using CMake's `FetchContent`.

This means your library's root `CMakeLists.txt` must:

1.  **Declare a Project:**
    Start with a `project()` command.
    ```cmake
    cmake_minimum_required(VERSION 3.15) # Or higher
    project(your_lib_name
            VERSION 1.0.0 # Optional but good practice
            LANGUAGES CXX)
    ```

2.  **Set C++ Standard:**
    Explicitly set the C++ standard (C++23 for Oat Interactive).
    ```cmake
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF) # Prefer explicit standard features
    ```

3.  **Define Libraries with `add_library()`:**
    Define your library targets. For shared/static libraries:
    ```cmake
    add_library(your_lib_name_target source1.cpp src/impl.cpp ...)
    # For header-only libraries that still need an interface target for usage requirements:
    # add_library(your_header_lib_target INTERFACE)
    ```

4.  **Export an Alias for Your Library Target (Crucial for Namespace Consistency):**
    To ensure consumers can link against your library using a namespaced target (e.g., `oat::your_lib`), create an ALIAS target. The alias should match your desired `find_package` and linking name.

    ```cmake
    # Primary target (actual library built)
    add_library(oat_your_lib_impl source1.cpp src/impl.cpp ...)
    # ... set properties on oat_your_lib_impl ...

    # Alias target for consumers (matches namespace)
    add_library(oat::your_lib ALIAS oat_your_lib_impl)
    ```
    Or, if your library target is already named appropriately (e.g., `oat_your_lib`):
    ```cmake
    add_library(oat_your_lib source1.cpp src/impl.cpp ...)
    # Alias for namespaced access if desired by convention, though not strictly necessary if main target is well-named
    # add_library(oat::your_lib ALIAS oat_your_lib) # If oat_your_lib is the actual target name
    ```
    The goal is for consumers to do `target_link_libraries(consumer_target PRIVATE oat::your_lib)`.

5.  **Properly Scope Include Directories:**
    Use `target_include_directories` with correct visibility (`PUBLIC`, `PRIVATE`, `INTERFACE`).
    *   `PUBLIC`: For include directories needed by both your library and its consumers. (Typically `include/` path).
    *   `INTERFACE`: For include directories needed *only* by consumers (e.g., for an interface library).
    *   `PRIVATE`: For include directories needed *only* by your library's internals (e.g., `src/` if it contains internal headers used by multiple `.cpp` files in your lib).

    ```cmake
    target_include_directories(oat_your_lib_impl # Or your main library target
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include> # Path after installation
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src # If you have internal headers in src/
    )
    ```

You don't need to write complex `FindYourLib.cmake` modules if your library is well-behaved with `FetchContent` and exports an alias.

---

### CM.3: **Header Layout SHALL Adhere to `include/your_org/your_lib/...`.**
<a name="cm3-header-layout-for-cmake"></a>

The public headers of your library must reside in a namespaced subdirectory within your `include` directory.

*   **Source Tree:** `your_project_root/include/oat/your_lib/header.h`
*   **CMake `target_include_directories`:** This allows consumers to use `#include <oat/your_lib/header.h>`.

    ```cmake
    # In your library's CMakeLists.txt
    target_include_directories(oat_your_lib_impl PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}> # Makes 'include' available for <oat/your_lib/header.h>
        # If installing, you'd point to the 'include' dir in the install prefix
        $<INSTALL_INTERFACE:.> # Assuming headers are installed to <prefix>/include
    )
    # This makes consumers use #include <oat/your_lib/header.h>
    # because target_include_directories adds the path *up to but not including* the final component
    # specified (i.e. `${CMAKE_CURRENT_SOURCE_DIR}/include` is added to include paths).
    ```
    The key is that `${CMAKE_CURRENT_SOURCE_DIR}/include` (for build tree) or `include` (for install tree, relative to `CMAKE_INSTALL_PREFIX`) is the path added to the include search paths, allowing the subsequent `oat/your_lib/header.h` to be resolved.

**Clear boundary between installed headers and internal headers is essential for proper CMake setup.**

---

### CM.4: **Headers SHALL Be Included With Full Namespace Path.**
<a name="cm4-full-namespace-path-includes"></a>

When including headers from *other* Oat Interactive libraries (or well-behaved third-party libraries), always use the full namespaced path.

✅ Good (within `my_consumer_lib.cpp`):
```cpp
#include <oat/network/session.h>    // Including a header from oat::network library
#include <oat/core/logger.h>        // Including a header from oat::core library
// #include <some_external_lib/public_api.h>
```

🚫 Bad:
```cpp
// Assuming current library is different from 'network' or 'core'
// #include "session.h"       // Ambiguous, relies on flat include paths
// #include "core/logger.h"   // Relative path, might break if project structure changes
```

This requires that libraries correctly set up their `target_include_directories` (see CM.2, CM.3) to export the path *above* their namespaced directory.

**Please include through the full, unambiguous namespace path to ensure build consistency across projects.**

---

### CM.5: **Install Targets SHALL Be Explicitly Configured for Libraries.**
<a name="cm5-install-target-configuration"></a>

If your library is intended to be installable (e.g., for system-wide use or packaging, beyond just `FetchContent`), you must configure install rules.

```cmake
# Install the library target (binaries)
install(TARGETS oat_your_lib_impl # Your actual library target
    EXPORT oat_your_lib_ExportSet # Name of the export set
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin # For DLLs on Windows
)

# Install public headers
install(DIRECTORY include/oat/your_lib/  # The specific public header directory
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/oat/your_lib # Ensures namespaced path on install
    COMPONENT dev
    FILES_MATCHING PATTERN "*.h"
)
# Or more generally if 'include/' contains 'oat/'
# install(DIRECTORY include/
#     DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#     COMPONENT dev
#     FILES_MATCHING PATTERN "*.h" # Might need more specific patterns
# )


# Generate and install the export set file
install(EXPORT oat_your_lib_ExportSet
    FILE        oat_your_lib_Targets.cmake # Name of the generated targets file
    NAMESPACE   oat:: # The namespace for imported targets (e.g., oat::your_lib)
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/oat_your_lib # Path for CMake support files
    COMPONENT   dev
)
```
*   Use `GNUInstallDirs` for standard paths (`CMAKE_INSTALL_LIBDIR`, `CMAKE_INSTALL_INCLUDEDIR`, etc.).
*   Don't just slap `install(DIRECTORY include/)`. Define your targets cleanly and export them properly using `install(EXPORT ...)` so consumers can use `find_package`.

**You're building libraries for consumption, not just throwing object files into a system directory.**

---

### CM.6: **All Projects SHALL Write and Install a `*_Config.cmake.in` and `*_ConfigVersion.cmake.in` File.**
<a name="cm6--_configcmakein-files"></a>

This is essential for robust `find_package(YourLibName CONFIG)` support, allowing downstream projects to easily find and use your installed library.

1.  **`your_lib_name_Config.cmake.in` (template):**
    ```cmake
    @PACKAGE_INIT@

    # Include the targets file generated by install(EXPORT ...)
    include("${CMAKE_CURRENT_LIST_DIR}/@TARGETS_EXPORT_NAME@.cmake")
    # The line above assumes TARGETS_EXPORT_NAME is set to your export set name, e.g., oat_your_lib_Targets

    # Check for dependencies (if any)
    # find_dependency(SomeOtherLib REQUIRED)

    check_required_components("@PROJECT_NAME@")
    ```

2.  **`your_lib_name_ConfigVersion.cmake.in` (template):**
    ```cmake
    set(PACKAGE_VERSION "@PACKAGE_VERSION@")

    if(PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION)
        set(PACKAGE_VERSION_COMPATIBLE FALSE)
    else()
        set(PACKAGE_VERSION_COMPATIBLE TRUE)
        if(PACKAGE_FIND_VERSION_EXACT)
            if(PACKAGE_VERSION VERSION_EQUAL PACKAGE_FIND_VERSION)
                set(PACKAGE_VERSION_EXACT TRUE)
            endif()
        endif()
    endif()
    ```

3.  **CMake code to configure and install these:**
    ```cmake
    # In your root CMakeLists.txt
    include(CMakePackageConfigHelpers)

    # Configure the ConfigVersion.cmake file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}" # Make sure PROJECT_VERSION is set
        COMPATIBILITY AnyNewerVersion # Or ExactVersion, SameMajorVersion
    )

    # Define the name of the export set for targets
    set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets") # e.g., oat_your_libTargets

    # Configure the Config.cmake file
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}Config.cmake.in" # Your template input
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"    # Generated output
        INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}" # Install path
        PATH_VARS # List any path vars that need to be made relative/absolute
            # CMAKE_INSTALL_PREFIX # Example
        NO_SET_AND_CHECK_MACRO # Recommended
        NO_CHECK_REQUIRED_COMPONENTS_MACRO # Recommended
    )

    # Install the generated config files
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
        COMPONENT dev
    )

    # (Make sure your install(EXPORT ...) from CM.5 uses the same NAMESPACE
    # and installs its ${TARGETS_EXPORT_NAME}.cmake file to the same directory)
    # install(EXPORT ${TARGETS_EXPORT_NAME}
    #     FILE        ${TARGETS_EXPORT_NAME}.cmake
    #     NAMESPACE   oat:: # Or your chosen namespace
    #     DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
    #     COMPONENT   dev
    # )
    ```

**Please ensure proper CMake config files are written for your library to enable standalone integration.**

---

## 🧪 Testing, Stability & CI (TSC)
<a name="-testing-stability--ci"></a>

Software that isn't tested isn't shippable. Software that only works on your machine needs improvement. At Oat Interactive, testing is not an afterthought; it's integral to the engineering process, ensuring stability, correctness, and reliability across all supported platforms.

### TSC.1: **Integration Tests are the Default for Systems and Libraries.**
<a name="tsc1-integration-tests-as-default"></a>

While unit tests have their place for small, isolated utility functions or classes with no external dependencies, the primary focus for Oat Interactive systems and libraries SHALL be on integration tests.

1.  **Black-Box Testing via Public APIs:**
    Integration tests interact with your library or system through its public API, just like any other consumer. They should not rely on internal implementation details, "friend" access, or private headers.
    *   **File Location:** Typically `tests/integration_test.cpp` or `tests/my_feature_integration_test.cpp`.
    *   **Focus:** Verifying that components work together correctly and that the public contract of the library is upheld.

2.  **Unit Tests for Isolated Logic:**
    Use unit tests for:
    *   Pure algorithms (e.g., a complex data transformation function).
    *   Utility classes with minimal state and no external dependencies (e.g., a custom string formatter, a bit manipulation helper).
    *   Testing specific edge cases of a well-encapsulated piece of logic that is hard to trigger through the public API of a larger system.

**If you only wrote unit tests for your "manager" class's individual methods by mocking all its collaborators, you haven't fully tested your system--you've tested your mocking skills.** True confidence comes from testing the integrated whole through its defined contract.

---

### TSC.2: **Tests SHALL Run Headless, Offline, and Deterministically.**
<a name="tsc2-headless-offline-deterministic-tests"></a>

Test environments must be controlled and reproducible.

1.  **Headless:** Tests MUST NOT require a graphical environment, display server, or windowing system to run. All operations should be command-line driven and verifiable through exit codes, logs, or output files.
2.  **Offline:** Tests MUST pass without requiring internet access. Any external resources (e.g., test data files, configurations) should be part of the test suite or generated locally. If testing network components, use loopback interfaces or mock network layers; do not rely on external servers.
3.  **Deterministic:** Given the same input and test environment, a test SHALL produce the same result every time. Avoid randomness in tests unless the randomness itself is part of the test (e.g., testing a PRNG) and is carefully controlled (e.g., by seeding with a fixed value). Time-dependent tests must mock or control the passage of time.

**Tests should be robust, automated, and reliable. They should not depend on external factors beyond your control.**

---

### TSC.3: **All Test Failures SHALL Be Actionable and Diagnosable.**
<a name="tsc3-actionable-test-failures"></a>

A failing test that provides no useful information wastes valuable debugging time.

1.  **Clear Assertions:** Use your testing framework's assertion macros to check conditions. Assertion messages should be clear about what was expected and what was received.
    ```cpp
    // Example with a hypothetical test framework
    ASSERT_EQUALS(actual_value, expected_value, "Packet sequence number mismatch");
    ASSERT_TRUE(session->is_connected(), "Session failed to connect within timeout");
    ```
2.  **Dump Relevant State:** On failure, tests should output relevant state information that helps diagnose the problem. This might include:
    *   The specific input data that caused the failure.
    *   Key internal state variables (accessed via public getters if appropriate, or logged by the component itself under test).
    *   Relevant log messages from the component under test (ensure your `logger_base` interface can be directed to a test-specific buffer).
3.  **No "Segfault at line 999" as the Only Output:** While crashes are bugs to be fixed, the test itself should aim to catch error conditions *before* a crash, or the environment should provide a clear stack trace. Relying on a segfault as the sole indicator of failure is insufficient.

**Make your tests help you (and others) find and fix bugs quickly.**

---

### TSC.4: **You SHALL Test for Failure Modes and Edge Cases.**
<a name="tsc4-test-failure-modes"></a>

Testing only the "happy path" (where everything works perfectly) is insufficient for real-world software. Robust systems are defined by how well they handle adversity.

Your tests MUST cover:

*   **Invalid Inputs:** Null pointers, empty buffers, out-of-range values, malformed data.
*   **error Conditions:** How does the system behave when an operation fails? Are correct `std::error_code`s returned?
*   **Resource Exhaustion (Simulated):** How does the system react if it can't allocate memory (if testable), or if a dependent service reports an error?
*   **Timeouts:** For time-sensitive operations, do timeouts work correctly?
*   **Network Issues (for network code):**
    *   Packet loss (simulated).
    *   Packet duplication.
    *   Packet reordering.
    *   Connection drops.
*   **Boundary Conditions:** Zero items, one item, max items, etc.

**Your system's reliability is proven through comprehensive testing of both success and failure scenarios.**

---

### TSC.5: **Continuous Integration (CI) SHALL Build and Run Tests on All Supported Platforms.**

<a name="tsc5-cross-platform-ci"></a>

Oat Interactive software is cross‑platform. “Works on my machine” is not an acceptable standard.

1. **Supported Platforms:** CI pipelines MUST compile and run all tests on, at a minimum:
   * Linux (a common distribution, e.g., Ubuntu LTS)
   * macOS (latest or N‑1 version)
   * Windows (typically MSVC compiler)

2. **Automated Execution:** Tests MUST be automatically executed by the CI system on every proposed change (e.g., pull/merge request) and on merges to main branches.
3. **CI Configuration as Code:** The CI pipeline configuration (e.g., `.gitlab-ci.yml`, GitHub Actions workflow files) is part of the repository and treated as code.
4. **Failing Build Blocks the Change:** A CI build that fails (either compilation or tests) MUST prevent the change from being merged until fixed.

**A library without a CI configuration that successfully builds and tests on all target platforms SHALL not be considered ready for release.**

---

## 📢 Logging & Diagnostics (LOG)
<a name="-logging--diagnostics"></a>

Effective logging is indispensable for debugging, monitoring, and understanding the runtime behavior of systems--especially distributed and real‑time ones. At Oat Interactive, logging is a first‑class concern.

### LOG.1: **All Logs SHALL Go Through a `logger_base` Interface.**
<a name="log1-logger-interface"></a>

Direct use of `std::cout`, `printf`, `spdlog::log()`, or any other concrete logging mechanism within library or system code is strictly forbidden. All logging activity MUST be routed through a `logger_base` interface provided to the component.

1.  **The `logger_base` Interface:**
    Define a standard `logger_base` interface.
    ```cpp
    // include/oat/core/logger.h (or similar central location)
    #pragma once
    #include <string> // Or std::string_view if lifetimes are managed carefully
    #include <source_location> // C++20 for richer log metadata

    namespace oat::core {

        enum class log_level {
            trace,    // Highly detailed, for deep debugging
            debug,    // Developer-centric information
            info,     // General operational information
            warn,     // Potentially problematic situations
            error,    // Errors that the component handled or is reporting
            critical  // Severe errors that may lead to shutdown
        };

        // Optional: Helper to convert log_level to string
        // const char* to_string(log_level level);

        class logger_base {
        public:
            virtual ~logger_base() = default;

            // Basic logging method
            virtual void log(log_level level, const std::string& message) = 0;

            // Enriched logging method (preferred)
            virtual void log(log_level level,
                             const std::string& message,
                             const std::source_location& location = std::source_location::current()) {
                // Default implementation can simply call the basic one,
                // or concrete loggers can use the extra source_location info.
                // Example: format message with file:line from location
                std::string full_message = "[" + std::string(location.file_name()) + ":" +
                                           std::to_string(location.line()) + "] " + message;
                this->log(level, full_message); // Calls the pure virtual basic log
            }

            // Convenience methods (optional, can be implemented in terms of the above)
            // void trace(const std::string& message, const std::source_location& loc = ...){ log(log_level::trace, message, loc); }
            // void debug(const std::string& message, const std::source_location& loc = ...){ log(log_level::debug, message, loc); }
            // void info(const std::string& message, const std::source_location& loc = ...){ log(log_level::info, message, loc); }
            // void warn(const std::string& message, const std::source_location& loc = ...){ log(log_level::warn, message, loc); }
            // void error(const std::string& message, const std::source_location& loc = ...){ log(log_level::error, message, loc); }
            // void critical(const std::string& message, const std::source_location& loc = ...){ log(log_level::critical, message, loc); }

            virtual bool is_enabled(log_level level) const { // Optional: for performance critical paths
                 return true; // Default: always enabled, concrete loggers can be smarter
            }
        };

        // Provide a No-Op logger_base for convenience
        class noop_logger : public logger_base {
        public:
            void log(log_level /*level*/, const std::string& /*message*/) override {}
            void log(log_level /*level*/, const std::string& /*message*/, const std::source_location& /*location*/) override {}
            bool is_enabled(log_level /*level*/) const override { return false; }
        };

    } // namespace oat::core
    ```

2. **Injection:**
    Systems receive a `logger_base*` (typically non-owning) via their factory function / configuration struct.
    ```cpp
    // struct my_system_config {
    //     oat::core::logger_base* logger = nullptr; // Default to no logging if not provided
    //     // ...
    // };
    ```
   If `logger` is `nullptr`, the system SHOULD either use a static `noop_logger` instance or perform `nullptr` checks before every log call. A preferred approach is for the factory to instantiate a `noop_logger` if none is provided.

3. **Rationale:**
   * **Decoupling:** Libraries are not tied to a specific logging implementation.
   * **Controllability:** The application controls log output, formatting, and destinations.
   * **Testability:** Easy to inject a mock/test logger to verify logging behavior.

**Directly writing plain text such as `"something went wrong"` to standard output without using an appropriate `log_level` enum results in logs that are difficult to parse, filter, or maintain.**

---

### LOG.2: **Logs SHALL Be Structured and Contextual.**
<a name="log2-structured--prefixed-logs"></a>

Log messages MUST be informative and easy to parse, both for humans and machines.

1. **Prefix with Component/Subsystem:**
   Every log message originating from a specific component SHOULD clearly indicate its source. This can be achieved by the component itself prefixing its messages, or by the `logger_base` implementation if it supports contextual logging.

    ✅ Good (Component adds context):
    ```cpp
    // In session_manager.cpp
    // if (logger_ && logger_->is_enabled(log_level::warn)) {
    //    logger_->warn("[session_manager] Session " + session_id_ + " timed out.");
    // }
    ```
2. **Include Relevant Contextual Data:**
   Log messages SHOULD include key data points that help understand the event. Don’t just say “Error occurred”; specify *what* error, in *what* context.

    ✅ Good:
    ```cpp
    // logger_->error("[packet_parser] Failed to parse packet type " + std::to_string(type_val) +
    //               " from " + sender_addr.toString() + ": invalid length field.");
    // logger_->info("[reliable_channel:" + channel_id_ + "] Retrying SEQ=" + std::to_string(seq_num) +
    //              " (attempt " + std::to_string(retry_count) + ")");
    ```

    🚫 Bad (Unhelpful):
    ```cpp
    // logger_->error("An error happened.");
    // logger_->warn("Something bad in packet processing");
    ```

3. **Structured Logging (If Supported by Implementation):**
   While the basic interface uses strings, concrete `logger_base` implementations might support structured logging (key‑value pairs). If so, prefer it for easier machine parsing. The interface itself remains simple.

**Logs are critical instrumentation for operations, troubleshooting, and automated analysis; ensure every entry provides meaningful information.**

---

### LOG.3: **No Uncontrolled Logging in Hot Paths: Use Sampling or Throttling.**
<a name="log3-logging-in-hot-paths"></a>

Excessive logging in performance‑critical code sections (e.g., per packet, per tick in a high‑frequency loop) can severely degrade performance and produce overwhelming log volume.

1. **Check Log Level Before Formatting:**
    If formatting a log message is expensive, check if the current log level is enabled before doing the work.
    ```cpp
    // if (logger_ && logger_->is_enabled(log_level::debug)) {
    //     std::string complex_debug_info = format_lots_of_data(); // Expensive
    //     logger_->debug("[MyComponent] State: " + complex_debug_info);
    // }
    ```

2. **Sample or Throttle Frequent Events:**
    For events that occur very frequently:
    *   **Log once per N events:** E.g., "Processed 1000 packets."
    *   **Throttle by time:** E.g., Log a summary or a specific instance of an event no more than once every X seconds.
    *   Log only on significant *changes* in state, not every observation of the same state.

    ✅ Good (Throttling example):
    ```cpp
    // uint64_t last_queue_full_log_ns_ = 0;
    // const uint64_t LOG_THROTTLE_INTERVAL_NS = 5 * 1'000'000'000ULL; // 5 seconds

    // void handle_queue_full() {
    //     if (logger_ && logger_->is_enabled(log_level::warn)) {
    //         uint64_t now_ns = get_current_time_ns();
    //         if (now_ns - last_queue_full_log_ns_ > LOG_THROTTLE_INTERVAL_NS) {
    //             logger_->warn("[data_queue] Queue is full. Dropping data.");
    //             last_queue_full_log_ns_ = now_ns;
    //         }
    //     }
    //     // ... drop data ...
    // }
    ```

3. **`trace` and `debug` Levels for Verbosity:**
   Use `log_level::trace` and `log_level::debug` for highly verbose output. These levels are typically disabled in production but can be enabled for detailed diagnostics.

**Logging is a diagnostic tool, not a denial‑of‑service vector against your own application or logging infrastructure.**

---

## 📈 Telemetry / metrics_base (MET)
<a name="-telemetry--metrics"></a>

While logs provide detailed, event‑specific information, metrics offer a higher‑level, aggregated view of a system’s health and performance over time. Oat Interactive systems SHOULD expose key operational metrics.

### MET.1: **Every System SHALL Support `metrics_base*` Injection for Key Indicators.**
<a name="met1-metrics-injection"></a>

Similar to logging, components SHOULD NOT be hard‑coded to a specific metrics backend. Instead, they SHOULD accept a `metrics_base` interface.

1. **The `metrics_base` Interface:**
   Define a standard `metrics_base` interface.
    ```cpp
    // include/oat/core/metrics.h (or similar central location)
    #pragma once
    #include <string>
    #include <cstdint>
    #include <vector> // For tags/labels

    namespace oat::core {

        // Optional: A simple struct for key-value tags/labels
        struct metric_tag {
            std::string key;
            std::string value;
        };
        using metric_tags = std::vector<metric_tag>;


        class metrics_base {
        public:
            virtual ~metrics_base() = default;

            // Increment a counter
            virtual void increment(const std::string& name,
                                   const metric_tags& tags = {},
                                   uint64_t value = 1) = 0;

            // Set a gauge (a value that can go up or down)
            virtual void gauge(const std::string& name,
                               const metric_tags& tags = {},
                               double value) = 0;

            // Record a timing/histogram observation (often in milliseconds or nanoseconds)
            // The concrete implementation would handle aggregation (e.g., Prometheus histograms/summaries)
            virtual void timing(const std::string& name,
                                const metric_tags& tags = {},
                                uint64_t duration_ns) = 0;
        };

        // Provide a No-Op metrics_base implementation
        class noop_metrics : public metrics_base {
        public:
            void increment(const std::string& /*name*/, const metric_tags& /*tags*/, uint64_t /*value*/) override {}
            void gauge(const std::string& /*name*/, const metric_tags& /*tags*/, double /*value*/) override {}
            void timing(const std::string& /*name*/, const metric_tags& /*tags*/, uint64_t /*duration_ns*/) override {}
        };

    } // namespace oat::core
    ```

2. **Injection:**
   Systems receive a `metrics_base*` (typically non‑owning) via their factory function / configuration struct.
    ```cpp
    // struct my_system_config {
    //     oat::core::metrics_base* metrics = nullptr; // Default to no metrics if not provided
    //     // ...
    // };
    ```

   If `metrics` is `nullptr`, the system SHOULD use a static `noop_metrics` instance or perform `nullptr` checks.

3. **What to Instrument:**
   Focus on key performance indicators (KPIs) and operational health:
    *   **Counts:**
        *   Packets sent/received (per type, per channel)
        *   Messages processed
        *   Connections accepted/rejected/dropped
        *   Errors encountered (per type)
        *   Retries (e.g., ACK retries)
    *   **Gauges:**
        *   Active session count
        *   Queue sizes/depths
        *   Buffer utilization
        *   Resource handles in use
    *   **Timings/Histograms:**
        *   Request/response latency
        *   Processing time for significant operations
        *   Time spent waiting for I/O

**Use metrics judiciously. They should convey the system’s operational state, not overwhelm dashboards with unnecessary data.**

---

### MET.2: **Metric Names SHALL Be Stable, ASCII‑Only, `snake_case`, and Hierarchical.**
<a name="met2-metric-naming"></a>

Consistency in metric naming is crucial for dashboards, alerting, and long‑term analysis. Adhere to conventions often found in systems like Prometheus.

1. **Naming Convention:**
    *   **`snake_case`:** Use lowercase letters with underscores to separate words.
    *   **Application/Library Prefix:** Start with a prefix indicating the application or library (e.g., `oat_myapp_` or `pulse_net_`).
    *   **Logical Hierarchy:** Follow with components and the specific thing being measured.
    *   **Units (for non-obvious types):** If the unit isn't obvious (e.g., not a simple count), include it in the name (e.g., `_bytes`, `_seconds`, `_ns`). Counters are typically unitless counts of events.

    ✅ Good Examples:
    ```
    oat_myapp_http_requests_total         // Counter: total HTTP requests
    oat_myapp_sessions_active             // Gauge:   currently active sessions
    oat_myapp_database_query_duration_seconds // Histogram/Summary: query latency in seconds
    pulse_net_packets_sent_total{type="data",channel="reliable"} // Counter with tags
    pulse_net_packets_received_bytes_total // Counter: total bytes received
    oat_core_task_queue_depth             // Gauge: current depth of a task queue
    ```

2. **Stability:**
   Once a metric name (and its type) is established and used in dashboards or alerts, treat it as part of the system’s API. If a metric must change fundamentally, add a new one and deprecate the old.

3. **ASCII‑Only:**
   Metric names and tag keys/values SHALL use ASCII characters for maximum compatibility.

4. **Tags/Labels for Dimensionality (If Supported):**
   Use tags/labels instead of creating many similar metric names.

    ✅ Good (with tags):
    `increment("packets_sent_total", {{"channel_type", "reliable"}, {"message_type", "foo"}});`
    `increment("packets_sent_total", {{"channel_type", "unreliable"}, {"message_type", "bar"}});`

    This allows flexible querying and aggregation (e.g., total packets sent regardless of channel, or total for a specific channel).

**Clear, consistent, and meaningful metrics greatly simplify troubleshooting and capacity planning; avoid introducing metrics that do not provide actionable insight.**

---

## 🧬 Versioning & Compatibility (VC)
<a name="-versioning--compatibility"></a>

Managing versions and ensuring compatibility (or clear incompatibility) is essential for robust, maintainable software.

### VC.1: **Use an Explicit Version Constant for Protocols and File Formats.**

<a name="vc1-version-constant"></a>

Any data format that is serialized to disk, transmitted over a network, or otherwise persisted or exchanged externally SHALL include an explicit version marker.

1.  **Define a Version Constant:**
    Use an `inline constexpr` integer (usually `uint8_t` or `uint16_t` if many versions are anticipated) for the version.

    ✅ Good:
    ```cpp
    // include/oat/your_protocol/protocol_constants.h
    namespace oat::your_protocol {
        inline constexpr uint8_t k_protocol_version = 2; // Current version is 2
        // inline constexpr uint8_t k_min_supported_client_version = 1; // If applicable
    }

    // In your packet structure:
    struct HandshakePacket {
        uint8_t version; // Will be set to k_protocol_version by sender
        // ... other handshake data
    };

    struct FileHeader {
        uint32_t magic_number;
        uint16_t format_version; // Set to k_data_format_version
        // ... other metadata
    };
    ```

2. **Include in Handshakes/Headers:**
    *   For network protocols, the version number MUST be part of the initial handshake sequence.
    *   For file formats, it MUST be in the file header.

3.  **Check on Read/Receive:**
    When reading a file or receiving an initial handshake:
    *   Check the received version number.
    *   Decide if your current code can understand/support this version.
    *   Reject, adapt, or warn based on the version mismatch.

**Omitting `k_protocol_version` in the initial release can lead to ambiguous or incompatible Version 1 behavior and could require disruptive changes later.**

---

### VC.2: **New Features Affecting Wire Format or Behavior SHALL Negotiate Backward/Forward Compatibility via Handshake or Explicit Versioning.**
<a name="vc2-backward-compatibility-handshake"></a>

When introducing changes that alter the wire format, add capabilities, or change fundamental behaviors, these changes MUST be managed gracefully.

1.  **Handshake for Feature Negotiation (Preferred for Network Protocols):**
    During the initial connection handshake, peers can exchange their versions and a set of supported feature flags.
    *   **Example:** Client connects, sends `hello_packet { version = 2, feature_flags = 0b00000001 /* supports_compression */ }`.
    *   Server receives, checks version. If `version == 2`, it checks its own `feature_flags`. If server also supports compression, they can agree to use it. If client `version == 1` (which didn't have compression), server communicates without it.
    *   This allows incremental feature rollout without breaking older clients/servers immediately.

2.  **Strict Versioning for File Formats:**
    For file formats, the reader typically checks the `format_version`.
    *   If `read_version < current_version`: The reader might try to read it in a backward-compatible way (if possible and designed for) or migrate its data.
    *   If `read_version > current_version`: The reader should typically report an error ("file is from a newer version, please upgrade software") or attempt a partial read of known fields if the format allows.
    *   If `read_version` is unsupported (too old): Report an error.

3.  **Adding New Optional Fields/Messages:**
    *   If adding new *optional* fields to existing messages in a TLV (Type-Length-Value) or similar extensible format, older clients/servers can often ignore the unknown fields.
    *   If adding entirely new message types, ensure older peers can safely ignore or report an "unknown message type" error without crashing. This often requires a message header with `message_type` and `message_length`.

**If a protocol or file format must change without backward‑compatible negotiation, YOU SHALL provide a detailed migration guide, communicate the change clearly, and be prepared to address any resulting issues. Wherever feasible, prefer non‑breaking changes.**

---

### VC.3: **No Versioned Namespaces, No Complex SFINAE/Template API Versioning.**
<a name="vc3-no-api-versioning-via-templatesnamespaces"></a>

API versioning for C++ libraries should be explicit and manageable, not hidden behind layers of template magic or convoluted preprocessor directives.

1.  **Explicit New Interfaces for Major Breaking API Changes:**
    If you need to make significant, backward-incompatible changes to a C++ library's public API:
    *   Create a new set of interfaces, potentially in a new namespace (e.g., `oat::your_lib::v2::my_interface`).
    *   The old version (`oat::your_lib::my_interface` or `oat::your_lib::v1::my_interface`) can be maintained for a deprecation period or kept if long-term support is needed.
    *   Provide clear documentation on how to migrate from v1 to v2.

    ```cpp
    // Old: include/oat/your_lib/v1/service.h
    namespace oat::your_lib::v1 {
        class service { /* ... */ };
    }

    // New: include/oat/your_lib/v2/service.h
    namespace oat::your_lib::v2 {
        class service { /* new, improved, incompatible API */ };
    }
    ```

2.  **Avoid `#ifdef` for API Versioning in Headers:**
    Using preprocessor directives like `#ifdef USE_API_V2` to switch between API contracts within the *same header files* leads to configuration hell, makes code hard to read, and breaks the "One Definition Rule" if not handled with extreme care across translation units.

3.  **Avoid Template Metaprogramming / SFINAE for API Versioning:**
    While powerful, using complex template techniques (like SFINAE) to subtly change API behavior or signatures based on template parameters or type traits for versioning purposes makes the API opaque and difficult to reason about for users. Keep API contracts clear and explicit.

**Versioning is best handled explicitly at runtime (for protocols/formats) or through clearly versioned APIs (for C++ interfaces). Compile‑time techniques that obscure version differences increase maintenance risk. If it is a Version 2 API, name it accordingly.**

---

## 💀 Dead‑On‑Arrival Patterns (DOA)
<a name="-dead-on-arrival-patterns"></a>

These patterns are fundamentally misaligned with Oat Interactive’s engineering philosophy. If you rely heavily on any of them, reassess your design.

1.  **`std::shared_ptr` as the Default Choice for Ownership.**
    *   **Problem:** `std::shared_ptr` introduces overhead (atomic reference counting), obscures ownership, and can easily lead to reference cycles if not managed carefully. It makes reasoning about object lifetimes complex.
    *   **Oat Interactive Way:** Default to `std::unique_ptr` for clear, single ownership. Use `std::shared_ptr` *only* when shared ownership is an explicit, unavoidable design requirement, and justify its use thoroughly. Raw non-owning pointers (`T*`) or references (`T&`) are used for observation or borrowing, with lifetimes managed by `std::unique_ptr` owners.

2.  **Returning `std::string_view` from `std::string::c_str()` (or similar temporary-to-view conversions).**
    *   **Problem:** Creates dangling views if the underlying `std::string` (or other buffer owner) is temporary, goes out of scope, or is modified. This is a common source of memory corruption bugs.
        ```cpp
        // 🚫 DOA Example:
        std::string_view get_name_view() {
            std::string name = "Temporary Name";
            return name; // DANGLING! 'name' is destroyed, view points to garbage.
        }
        std::string_view get_substr_view(const std::string& s) {
            return s.substr(0, 4); // substr() can return a new temporary std::string
                                   // whose data is then viewed, also DANGLING.
        }
        ```
    *   **Oat Interactive Way:** Ensure `std::string_view` (or `buffer_view`) always points to data with a guaranteed lifetime that outlives the view. Return strings by value (`std::string`), by const reference to a stable member, or use owned buffer types if data needs to be passed around.

3.  **Virtual Destructors in Non-Polymorphic Base Classes (or Classes Not Intended for Polymorphic Deletion).**
    *   **Problem:** Adds unnecessary vtable overhead if the class is never intended to be derived from and deleted via a base class pointer. Conversely, *forgetting* a virtual destructor in a class *intended* to be a polymorphic base is a bug.
    *   **Oat Interactive Way:**
        *   Public base classes meant to be interfaces (deleted via base pointer) **MUST** have a `virtual ~base() = default;` (See A.2).
        *   Classes not intended for polymorphic deletion (e.g., concrete utility classes, PODs) should not have virtual destructors. Mark them `final` if they shouldn't be derived from at all.

4.  **Deep Inheritance Chains (Longer than 1-2 Levels for Implementation Inheritance).**
    *   **Problem:** Deep inheritance hierarchies ("is-a" relationships for implementation sharing) lead to brittle, tightly coupled designs (the "fragile base class problem"). They are hard to understand, modify, and test.
    *   **Oat Interactive Way:**
        *   Prefer composition over implementation inheritance. Build complex objects by assembling smaller, focused components.
        *   Interface inheritance (deriving from abstract classes with pure virtuals) is fine and encouraged (A.2), but this is about *contract*, not *implementation* sharing.
        *   If you have `class d : public c; class c : public b; class b : public a;`, your design is likely flawed.

5.  **Singleton Pattern (Classic Global Access Version).**
    *   **Problem:** Singletons introduce global state, make dependency tracking difficult, hinder testability (hard to mock/replace), and often lead to issues with initialization order and lifetime management in complex systems.
    *   **Oat Interactive Way:** If you need a single, shared instance of a service, make it an explicit dependency. Create it once at the application's composition root and pass it (as a pointer or reference, often via an interface) to the components that need it (see DL.1, LOG.1, MET.1). This is called "Dependency Injection."

6.  **Returning error Codes (e.g., `int` or `bool`) and Output Parameters for Results.**
    *   **Problem:** C-style error handling with output parameters is error-prone (caller might forget to check the return code), makes function signatures clunky, and doesn't compose well.
        ```cpp
        // 🚫 DOA Example:
        // bool get_data(int id, my_data* out_data, error* out_error);
        ```
    *   **Oat Interactive Way:** Use `std::expected<T, std::error_code>` to return either a value `T` or an `error`. This forces the caller to explicitly check for and handle the error case. (See L.2, L.4, API.2, API.3).

---

## ✋ Stop Right There If...
<a name="-stop-right-there-if"></a>

This is a quick checklist. If you find yourself doing any of these, pause, take a step back, and reconsider your approach. You might be veering off the Oat Interactive path.

*   **You're writing templates for code that isn't genuinely generic across multiple, unrelated types.**
    *   *Why stop?* Templates add compile-time overhead and complexity. Only use them when the generic algorithm or data structure truly applies to a family of types that cannot be reasonably handled by runtime polymorphism or simple function overloading. Don't use templates just to avoid writing a function twice for `int` and `float` if that's the only use case.

*   **You're wrapping everything in a "manager" class (e.g., `session_manager`, `network_manager`, `ui_manager`) because you don't have a clearer name or defined responsibility for the component.**
    *   *Why stop?* "manager" is often a sign of a god object or a class with poorly defined, overly broad responsibilities. Try to break down "managers" into smaller, more focused components with clear, single responsibilities.

*   **You're hiding I/O (network, disk) behind lambda callbacks passed deep into a system, instead of using explicit I/O calls or a proper event dispatch/`tick()` mechanism.**
    *   *Why stop?* Obscures when and how I/O occurs, making it hard to debug, test, and reason about threading or blocking behavior. I/O should be explicit (RT.4) or handled through a well-defined event loop/`tick()` process.

*   **You're using `std::any`, `std::variant` with many alternatives, or `void*` to pass generic data through core system interfaces without strong type safety or clear contracts.**
    *   *Why stop?* These can be type-safety escape hatches. While `std::variant` is useful for sum types, overly complex variants or reliance on `std::any`/`void*` often indicate a design that avoids defining clear data contracts between components.

*   **Your public header files `#include` private implementation headers (e.g., `_impl.h` files from `src/`).**
    *   *Why stop?* This breaks encapsulation, exposes internal details, and increases coupling and compile times for consumers (A.1, A.3, L.6).

*   **You're manually calling `new` and `delete` outside of what `std::make_unique` (or a custom allocator within an encapsulated data structure) would handle.**
    *   *Why stop?* Prone to memory leaks and lifetime errors. `std::unique_ptr` (and `std::make_unique`) should manage almost all dynamic memory (L.4).

*   **You find yourself needing to `dynamic_cast` frequently.**
    *   *Why stop?* Often a sign that your polymorphic design is flawed, or you're trying to break type safety. Well-designed interfaces shouldn't require consumers to know the concrete derived types (L.5).

*   **Your CMake file looks like a sequence of `include_directories()` and `link_libraries()` without using `target_include_directories()` and `target_link_libraries()`.**
    *   *Why stop?* This is "old-style" CMake that leads to difficult-to-manage dependencies and properties that don't propagate correctly. Modern CMake is target-centric (CM.2, CM.4).

---

## 📦 Final Word
<a name="-final-word"></a>

This document goes beyond style guidance; it is intended to ensure reliability and maintainability for systems such as multiplayer networking stacks, real‑time engines, and simulation backends.

**The following common justifications are insufficient in this context:**

* “It worked on my machine.”
* “Another engine does it this way.”
* “I prefer exceptions.”

Please review these rules carefully; they are mandatory. Non‑compliance introduces technical debt that the team will later need to resolve.

To recap:

* If your code throws exceptions across a boundary, it’s wrong.
* If your header pulls in half the STL for no reason, it’s wrong.
* If your object construction isn’t wrapped in a factory returning `std::expected`, it’s wrong.
* If your test isn’t integration‑first and deterministic, it’s wrong.
* If your API doesn’t make side effects explicit, it’s wrong.
* If your memory ownership model isn’t obvious from the signature, it’s wrong.
* If your CMake setup can’t build, install, and be consumed via `FetchContent`, it’s wrong.

Prioritize correctness and clarity over unnecessary complexity.

**As Oat Interactive C++ engineers, we are expected to uphold these standards and deliver accordingly.**
