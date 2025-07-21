# Getting Started with Ion Vortex

Welcome to Ion Vortex! This guide will help you get up and running with the framework, from initial setup to building and integrating its libraries.

## Quick Start

If you're experienced with C++ and just want to dive in:

```bash
# Clone and setup
git clone https://github.com/oat-im/ion.git
cd ion
./scripts/bootstrap.sh  # or bootstrap.ps1 on Windows

# Build and run
cmake --preset debug-linux    # or debug-windows, debug-macos
cmake --build --preset debug-linux
./build/debug/Linux/libs/engine_interface/tests
```

## Detailed Guides

### 📋 [Prerequisites](prerequisites.md)
Everything you need to install before building Ion Vortex:
- Compiler requirements (C++23)
- CMake 3.28+
- vcpkg setup
- Platform-specific dependencies

### 🔨 [Building Ion Vortex](building.md)
Step-by-step build instructions:
- Using CMake presets
- Building individual libraries
- Debug vs Release builds
- Troubleshooting common issues

## Choose Your Path

After completing the basics, explore based on your interests:

- **Library Integration** → See [API Reference](../api/)
- **Engine Development** → See [Architecture docs](../architecture/)
- **Contributing** → See [Contributing guide](../contributing/)

## Platform Notes

### Windows
- Visual Studio 2022 or later required
- Use Visual Studio generator (not Ninja) for best results
- Run from any terminal (no Developer Command Prompt needed)

### Linux
- GCC 13+ or Clang 16+
- Install X11/Wayland development packages
- Ninja generator recommended

### macOS
- Xcode 14+ or Command Line Tools
- Apple Silicon (M1/M2) and Intel supported
- Homebrew for dependency management

## Getting Help

- **Build Issues**: Check [Building Guide](building.md#troubleshooting)
- **API Questions**: See [API Reference](../api/)
- **Architecture**: Read [Architecture docs](../architecture/)
- **Community**: Join our Discord/Forums (if applicable)

## Next Steps

1. ✅ Complete [Prerequisites](prerequisites.md)
2. ✅ Follow [Building Guide](building.md)
3. 🎯 Pick a library and start coding!

Remember: Ion Vortex follows strict C++ standards. Review the [C++ Manual](../development/cpp-manual.md) to understand our coding requirements.