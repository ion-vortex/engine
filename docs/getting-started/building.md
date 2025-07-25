# Building Ion Vortex

This guide covers building Ion Vortex from source on all supported platforms.

## Quick Build

For the impatient (after [prerequisites](prerequisites.md) are installed):

```bash
git clone https://github.com/oat-im/ion.git
cd ion
./scripts/bootstrap.sh           # bootstrap.ps1 on Windows
cmake --preset debug-linux       # or debug-windows, debug-macos
cmake --build --preset debug-linux
```

## Step-by-Step Guide

### 1. Clone the Repository

```bash
git clone https://github.com/oat-im/ion.git
cd ion
```

### 2. Bootstrap vcpkg

Our bootstrap script sets up vcpkg and installs dependencies:

**Linux/macOS:**
```bash
./scripts/bootstrap.sh
```

**Windows:**
```powershell
.\scripts\bootstrap.ps1
```

This script:
- Clones vcpkg (if needed)
- Bootstraps vcpkg
- Installs all dependencies
- Sets up the build environment

### 3. Configure with CMake

Ion Vortex uses CMake presets for consistent configuration:

```bash
# Configure for debug build (platform-specific)
cmake --preset debug-linux    # Linux
cmake --preset debug-windows  # Windows  
cmake --preset debug-macos    # macOS

# Or configure for release build
cmake --preset release-linux    # Linux
cmake --preset release-windows  # Windows
cmake --preset release-macos    # macOS
```

### 4. Build

Build everything:
```bash
cmake --build --preset debug-linux    # or debug-windows, debug-macos
```

Build specific targets:
```bash
# Build a specific library
cmake --build --preset debug-linux --target ion_engine_interface

# Build with parallel jobs (if not automatic)
cmake --build --preset debug-linux -j 8
```

## Understanding Presets

Ion Vortex provides these CMake presets:

### Configuration Presets

| Preset | Platform | Build Type | Generator | Use Case |
|--------|----------|------------|-----------|----------|
| `debug-linux` | Linux | Debug | Ninja | Linux development |
| `debug-windows` | Windows | Debug | VS/MSBuild | Windows development |
| `debug-macos` | macOS | Debug | Ninja | macOS development |
| `release-linux` | Linux | Release | Ninja | Linux production (LTO) |
| `release-windows` | Windows | Release | VS/MSBuild | Windows production (LTO) |
| `release-macos` | macOS | Release | Ninja | macOS production (LTO) |

Note: Release builds use custom vcpkg triplets with LTO enabled.

### Build Output

Build outputs are organized by configuration and platform:
```
ion/
├── build/
│   ├── debug/
│   │   ├── Linux/
│   │   ├── Windows/
│   │   └── Darwin/
│   └── release/
│       ├── Linux/
│       ├── Windows/
│       └── Darwin/
```

## Building Individual Components

### Build a Single Library

```bash
# Configure if not already done
cmake --preset debug

# Build just ion::engine_interface
cmake --build --preset debug --target ion_engine_interface

# Build just ion::script
cmake --build --preset debug --target ion_script
```

### Build With Tests

```bash
# Configure with tests enabled
cmake --preset debug -DION_BUILD_TESTS=ON

# Build everything including tests
cmake --build --preset debug

# Run tests
ctest --preset debug
```

## Advanced Build Options

### Unity Builds

Unity builds are enabled by default for faster compilation:
```bash
# Disable if causing issues
cmake --preset debug -DION_ENABLE_UNITY_BUILD=OFF
```

## Platform-Specific Notes

### Windows

- Default generator is Visual Studio (MSBuild)
- For faster builds, use Ninja from Developer Command Prompt:
  ```powershell
  # From VS Developer Command Prompt/PowerShell
  cmake --preset debug-windows-ninja
  ```
- Debug builds include sanitizers

### Linux

- Default generator is Ninja (fast)
- Ensure all X11/Wayland dev packages are installed
- Debug builds include AddressSanitizer and UBSan

### macOS

- Supports both Intel and Apple Silicon
- Universal binaries not supported (pick architecture)
- May need to allow unsigned binaries in Security settings

## Build Artifacts

After building, find executables and libraries:

```
build/[preset]/
├── libs/
│   ├── libion_core.a          # Core library
│   ├── libion_engine_interface.a  # Engine interface library
│   ├── libion_script.a        # Script library
│   └── ...                    # Other libraries
```

## Troubleshooting

### Build Fails with "Cannot find package"

vcpkg couldn't find or build a dependency:
```bash
# Clear vcpkg binary cache
rm -rf ~/.cache/vcpkg  # Linux/macOS
rm -rf %LOCALAPPDATA%\vcpkg  # Windows

# Re-run bootstrap
./scripts/bootstrap.sh
```

### "No CMAKE_CXX_COMPILER found"

- **Windows**: Use Visual Studio generator (default) instead of Ninja
- **All platforms**: Ensure compiler is in PATH

### Out of Memory During Build

Reduce parallel jobs:
```bash
cmake --build --preset debug -j 2
```

Or disable unity builds:
```bash
cmake --preset debug -DION_ENABLE_UNITY_BUILD=OFF
```

### Linking Errors

If you get undefined symbols:
1. Ensure all dependencies built successfully
2. Check that you're not mixing Debug/Release libraries
3. Try a clean build:
   ```bash
   rm -rf build/debug
   cmake --preset debug
   cmake --build --preset debug
   ```

### "Permission Denied" on Linux/macOS

Make scripts executable:
```bash
chmod +x scripts/*.sh
```

## Cleaning Builds

Remove build artifacts:
```bash
# Clean specific configuration
rm -rf build/debug
rm -rf build/release

# Clean everything
rm -rf build/
```

## Next Steps

After successfully building:
1. Learn about the [Build System](../development/build-system.md)
2. Read the [Architecture](../architecture/) documentation
3. Explore the [API Reference](../api/)

## Tips for Faster Builds

1. **Use Ninja**: Faster than Make or MSBuild
2. **Enable ccache** (Linux/macOS):
   ```bash
   export CMAKE_CXX_COMPILER_LAUNCHER=ccache
   ```
3. **Use Unity Builds**: Enabled by default
4. **Build only what you need**: Use specific targets
5. **Use Release mode**: When you don't need debug symbols
6. **Upgrade your hardware**: More RAM and CPU cores help!