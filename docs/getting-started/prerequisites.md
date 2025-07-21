# Prerequisites

Before building Ion Vortex, you'll need to install and configure several tools. This guide covers all platforms.

## Required Tools

### C++ Compiler

You need a C++23 capable compiler:

| Platform | Compiler | Minimum Version | Recommended |
|----------|----------|-----------------|-------------|
| Windows  | MSVC     | 19.39 (VS 2022 17.9) | Latest VS 2022 |
| Linux    | GCC      | 13.0 | 13.2+ |
| Linux    | Clang    | 16.0 | 17.0+ |
| macOS    | Apple Clang | 15.0 | Latest Xcode |

### CMake

- **Minimum**: 3.28
- **Recommended**: Latest stable (3.30+)
- **Download**: https://cmake.org/download/

Verify installation:
```bash
cmake --version
```

### vcpkg

Ion Vortex uses vcpkg for dependency management. It will be automatically cloned by our bootstrap script, but you can also install it globally.

**Option 1: Let bootstrap handle it** (Recommended)
```bash
# The bootstrap script will set up vcpkg for you
./scripts/bootstrap.sh  # Linux/macOS
./scripts/bootstrap.ps1 # Windows
```

**Option 2: Global installation**
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh  # or bootstrap-vcpkg.bat on Windows
export VCPKG_ROOT=$(pwd)  # Add to your shell profile
```

### Build Tools

| Platform | Tool | Purpose |
|----------|------|---------|
| All | Ninja | Fast builds (optional but recommended) |
| Windows | Visual Studio 2022 | Compiler and build tools |
| Linux | build-essential | Basic build tools |
| macOS | Xcode Command Line Tools | Compiler and tools |

## Platform-Specific Setup

### Windows

1. **Install Visual Studio 2022**
   - Download from: https://visualstudio.microsoft.com/
   - During installation, select:
     - "Desktop development with C++"
     - "C++ CMake tools for Windows"
     - Windows 11 SDK (or Windows 10 SDK)

2. **Install Git**
   - Download from: https://git-scm.com/download/win
   - Or via winget: `winget install Git.Git`

3. **Note**: Ninja is not required on Windows (Visual Studio generator is used)

### Linux (Ubuntu/Debian)

Run this command to install all dependencies:

```bash
sudo apt update && sudo apt install -y \
  git build-essential ninja-build pkg-config cmake \
  libssl-dev zlib1g-dev \
  autoconf libtool automake libltdl-dev
```

**Important**: The autotools packages (autoconf, libtool, automake) are required by some vcpkg dependencies.

For other distributions:
- **Fedora/RHEL**: `dnf install gcc-c++ cmake ninja-build pkg-config autoconf libtool automake`
- **Arch**: `pacman -S base-devel cmake ninja git autoconf automake libtool pkg-config`

### macOS

1. **Install Xcode Command Line Tools**
   ```bash
   xcode-select --install
   ```

2. **Install Homebrew** (if not already installed)
   ```bash
   /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   ```

3. **Install build dependencies**
   ```bash
   brew install cmake ninja pkg-config libtool autoconf automake
   ```

## Verify Your Setup

Run our verification script to ensure everything is properly installed:

```bash
# After cloning the repository
cd ion
./scripts/verify-setup.sh  # or verify-setup.ps1 on Windows
```

This script checks:
- ✅ Compiler version and C++23 support
- ✅ CMake version
- ✅ vcpkg availability
- ✅ Required system libraries
- ✅ Git configuration

## Disk Space Requirements

Ensure you have sufficient disk space:
- **Source + Build**: ~5 GB
- **vcpkg packages**: ~10 GB (cached after first build)
- **Total recommended**: 20 GB free space

## IDE Setup (Optional)

### Visual Studio Code
1. Install C++ extension pack
2. Install CMake Tools extension
3. Open the ion folder
4. Select a CMake kit when prompted

### Visual Studio 2022
1. Open Folder → Select ion directory
2. VS will detect CMakePresets.json automatically
3. Select your desired preset from the toolbar

### CLion
1. Open ion as a CMake project
2. Set CMake options to use vcpkg toolchain
3. Select desired build preset

## Network Requirements

First build requires downloading dependencies:
- vcpkg will download and build ~20 packages
- Total download: ~500 MB
- Requires HTTPS access to:
  - github.com
  - Various package sources

## Troubleshooting

### "CMake version too old"
Update CMake to 3.28+. On Ubuntu 22.04 or older, you may need to:
```bash
# Remove old version
sudo apt remove cmake
# Install via snap
sudo snap install cmake --classic
```

### "C++ compiler cannot create executables"
- **Windows**: Install Visual Studio with C++ workload
- **Linux**: Install build-essential
- **macOS**: Run `xcode-select --install`

### "vcpkg packages fail to build"
- Ensure you have all platform dependencies installed
- Check internet connectivity
- Try clearing vcpkg cache: `rm -rf vcpkg/buildtrees`

## Next Steps

Once prerequisites are installed:
1. Continue to [Building Ion](building.md)
2. Learn about [CMake Presets](building.md#understanding-presets)
3. Understand the [Build System](../development/build-system.md)