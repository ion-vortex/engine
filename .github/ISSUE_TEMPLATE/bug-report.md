---
name: Bug Report
about: Create a report to help us improve
title: "[BUG] Brief description of the issue"
labels: bug
assignees: raygoe

---

## Description

**Brief Summary:**
A clear and concise description of what the bug is.

**Expected Behavior:**
A clear description of what you expected to happen.

**Actual Behavior:**
A clear description of what actually happened.

**Have you tried:**
- [ ] Running `./scripts/bootstrap.sh` (Linux, macOS) or `./scripts/bootstrap.ps1` (Windows)
- [ ] Running `./scripts/verify-setup.sh` (Linux, macOS) or `./scripts/verify-setup.ps1` (Windows)

## Environment

**Build Configuration:**
- [ ] Debug preset (`debug`)
- [ ] Release preset (`release`)

**Platform:**
- [ ] Windows (specify version: _____________)
- [ ] Linux (specify distro: _____________)
- [ ] macOS (specify version: _____________)

**Compiler:**
- [ ] MSVC 2022
- [ ] GCC (version: _______)
- [ ] Clang (version: _____)
- [ ] Other: _________________

**CMake Version:** (run `cmake --version`)

**vcpkg Version:** (run `vcpkg version`)

## Component/Module Affected

**Which Ion Vortex module is affected?** (check all that apply)
- [ ] `ion::core` (logging, error handling, lifecycle)
- [ ] `ion::render` (SDL3, rendering backend)
- [ ] `ion::ui` (ImGui integration)
- [ ] `ion::physics` (SAT collision detection)
- [ ] `ion::protocol` (networking protocol)
- [ ] `ion::asset` (asset loading)
- [ ] `ion::audio` (SDL3 audio)
- [ ] `ion::crypto` (cryptographic utilities)
- [ ] `apps/client` (SDL3 + ImGui client)
- [ ] `apps/zoned` (headless zone server)
- [ ] `apps/unid` (zone router/login)
- [ ] Build system (CMake, vcpkg)
- [ ] Documentation
- [ ] Unknown/Multiple modules

## Steps to Reproduce

1. 
2. 
3. 
4. 

**Minimal reproduction case:** (if possible, provide the smallest code example that reproduces the issue)

```cpp
// Paste minimal code here
```

## Error Information

**Compilation Errors:** (if applicable)
```
Paste compilation errors here
```

**Runtime Errors/Crashes:** (if applicable)
```
Paste stack trace, error messages, or crash logs here
```

**Assertion Failures:** (if applicable)
```
Paste assertion failure messages here
```

## Additional Context

**Logs:** (if available, paste relevant log output or attach it)
```
Paste log output here
```
