#!/usr/bin/env bash
# Verify development environment setup for Ion Vortex - Linux/macOS
set -euo pipefail

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Counters
ERRORS=0
WARNINGS=0

# Helper functions
check_pass() {
    echo -e "${GREEN}✓${NC} $1"
}

check_fail() {
    echo -e "${RED}✗${NC} $1"
    ((ERRORS++))
}

check_warn() {
    echo -e "${YELLOW}⚠${NC} $1"
    ((WARNINGS++))
}

header() {
    echo -e "\n${BLUE}=== $1 ===${NC}"
}

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
    PRESET_DEBUG="debug-linux"
    PRESET_RELEASE="release-linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
    PRESET_DEBUG="debug-macos"
    PRESET_RELEASE="release-macos"
else
    check_fail "Unsupported OS: $OSTYPE"
    exit 1
fi

echo "Ion Vortex Development Environment Verification"
echo "==============================================="
echo "OS: $OS"
echo ""

# Check Git
header "Version Control"
if command -v git &> /dev/null; then
    GIT_VERSION=$(git --version | cut -d' ' -f3)
    check_pass "Git $GIT_VERSION"
    
    # Check if we're in a git repo
    if git rev-parse --git-dir &> /dev/null; then
        BRANCH=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "unknown")
        REMOTE=$(git config --get remote.origin.url 2>/dev/null || echo "none")
        check_pass "In git repository (branch: $BRANCH)"
        if [[ "$REMOTE" == *"git.oat.im/oat/ion"* ]]; then
            check_pass "Correct remote: $REMOTE"
        else
            check_warn "Unexpected remote: $REMOTE"
        fi
    else
        check_warn "Not in a git repository"
    fi
else
    check_fail "Git not found"
fi

# Check build tools
header "Build Tools"

# CMake
if command -v cmake &> /dev/null; then
    CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
    CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d. -f1)
    CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d. -f2)
    
    if [ "$CMAKE_MAJOR" -gt 3 ] || ([ "$CMAKE_MAJOR" -eq 3 ] && [ "$CMAKE_MINOR" -ge 28 ]); then
        check_pass "CMake $CMAKE_VERSION"
    else
        check_fail "CMake $CMAKE_VERSION (need 3.28+)"
    fi
else
    check_fail "CMake not found"
fi

# Ninja (required on Linux/macOS)
if command -v ninja &> /dev/null; then
    NINJA_VERSION=$(ninja --version)
    check_pass "Ninja $NINJA_VERSION"
else
    check_fail "Ninja not found (required on $OS)"
fi

# Check C++ compiler
header "C++ Compiler"

if command -v clang++ &> /dev/null; then
    CLANG_VERSION=$(clang++ --version | head -n1)
    # Extract version number
    CLANG_VER_NUM=$(echo "$CLANG_VERSION" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1 | cut -d. -f1)
    
    if [ -n "$CLANG_VER_NUM" ] && [ "$CLANG_VER_NUM" -ge 16 ]; then
        check_pass "Clang++ $CLANG_VERSION"
        CXX_FOUND=true
    else
        check_warn "Clang++ $CLANG_VERSION (may be too old, need 16+)"
        CXX_FOUND=true
    fi
elif command -v g++ &> /dev/null; then
    GCC_VERSION=$(g++ --version | head -n1)
    GCC_VER_NUM=$(echo "$GCC_VERSION" | grep -oE '[0-9]+' | head -1)
    
    if [ -n "$GCC_VER_NUM" ] && [ "$GCC_VER_NUM" -ge 13 ]; then
        check_pass "G++ $GCC_VERSION"
        CXX_FOUND=true
    else
        check_warn "G++ $GCC_VERSION (may be too old, need 13+)"
        CXX_FOUND=true
    fi
else
    check_fail "No C++ compiler found (need clang++ 16+ or g++ 13+)"
    CXX_FOUND=false
fi

# Test C++23 support
if [ "$CXX_FOUND" = true ]; then
    echo '#include <expected>' > /tmp/ion_cpp23_test.cpp
    echo 'int main() { std::expected<int,int> e{42}; return e.value(); }' >> /tmp/ion_cpp23_test.cpp
    
    if $CXX_COMPILER -std=c++23 -c /tmp/ion_cpp23_test.cpp -o /tmp/ion_cpp23_test.o &> /dev/null; then
        check_pass "C++23 support verified"
    else
        check_fail "C++23 support not available"
    fi
    rm -f /tmp/ion_cpp23_test.cpp /tmp/ion_cpp23_test.o
fi

# Check vcpkg
header "Package Manager"

if [ -n "${VCPKG_ROOT:-}" ] && [ -f "$VCPKG_ROOT/vcpkg" ]; then
    check_pass "vcpkg found at: $VCPKG_ROOT"
    
    # Check if it's executable
    if [ -x "$VCPKG_ROOT/vcpkg" ]; then
        check_pass "vcpkg is executable"
    else
        check_fail "vcpkg is not executable"
    fi
else
    SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
    LOCAL_VCPKG="$PROJECT_ROOT/vcpkg-instance"
    
    if [ -f "$LOCAL_VCPKG/vcpkg" ]; then
        check_warn "vcpkg found locally at: $LOCAL_VCPKG (VCPKG_ROOT not set)"
        echo "    Run: export VCPKG_ROOT=$LOCAL_VCPKG"
    else
        check_fail "vcpkg not found (run bootstrap.sh first)"
    fi
fi

# Platform-specific checks
header "Platform-Specific Requirements"

if [[ "$OS" == "Linux" ]]; then
    # Check for X11/OpenGL development packages
    MISSING_PKGS=()
    
    # Check for common header files
    if ! [ -f /usr/include/X11/Xlib.h ] && ! [ -f /usr/local/include/X11/Xlib.h ]; then
        MISSING_PKGS+=("libx11-dev")
    fi
    
    if ! [ -f /usr/include/GL/gl.h ] && ! [ -f /usr/local/include/GL/gl.h ]; then
        MISSING_PKGS+=("libgl1-mesa-dev")
    fi
    
    if [ ${#MISSING_PKGS[@]} -eq 0 ]; then
        check_pass "X11/OpenGL development headers found"
    else
        check_fail "Missing packages: ${MISSING_PKGS[*]}"
        echo "    Run: sudo apt install ${MISSING_PKGS[*]}"
    fi
    
    # Check for pkg-config
    if command -v pkg-config &> /dev/null; then
        check_pass "pkg-config found"
    else
        check_fail "pkg-config not found"
    fi
    
elif [[ "$OS" == "macOS" ]]; then
    # Check for Xcode command line tools
    if xcode-select -p &> /dev/null; then
        XCODE_PATH=$(xcode-select -p)
        check_pass "Xcode Command Line Tools: $XCODE_PATH"
    else
        check_fail "Xcode Command Line Tools not installed"
        echo "    Run: xcode-select --install"
    fi
    
    # Check for Homebrew (optional but recommended)
    if command -v brew &> /dev/null; then
        check_pass "Homebrew found"
    else
        check_warn "Homebrew not found (optional but recommended)"
    fi
fi

# Check disk space
header "System Resources"

AVAILABLE_SPACE=$(df -BG . | awk 'NR==2 {print $4}' | sed 's/G//')
if [ "$AVAILABLE_SPACE" -ge 20 ]; then
    check_pass "Disk space: ${AVAILABLE_SPACE}GB available"
elif [ "$AVAILABLE_SPACE" -ge 10 ]; then
    check_warn "Disk space: ${AVAILABLE_SPACE}GB available (20GB recommended)"
else
    check_fail "Disk space: ${AVAILABLE_SPACE}GB available (20GB required)"
fi

# Check if we can build
header "Build Environment Test"

if [ -f CMakeLists.txt ]; then
    check_pass "CMakeLists.txt found"
    
    # Check if CMakePresets.json exists
    if [ -f CMakePresets.json ]; then
        check_pass "CMakePresets.json found"
        
        # Try to list presets
        if cmake --list-presets &> /dev/null; then
            check_pass "CMake presets accessible"
        else
            check_warn "Cannot list CMake presets"
        fi
    else
        check_fail "CMakePresets.json not found"
    fi
else
    check_fail "Not in Ion Vortex project root (CMakeLists.txt not found)"
fi

# Summary
echo ""
echo "==============================================="
if [ $ERRORS -eq 0 ]; then
    if [ $WARNINGS -eq 0 ]; then
        echo -e "${GREEN}All checks passed!${NC}"
        echo ""
        echo "Your environment is ready. Next steps:"
        echo "  1. Run: ./scripts/bootstrap.sh"
        echo "  2. Run: cmake --preset $PRESET_DEBUG"
        echo "  3. Run: cmake --build --preset $PRESET_DEBUG"
    else
        echo -e "${YELLOW}Setup complete with $WARNINGS warnings${NC}"
        echo ""
        echo "Your environment should work, but check the warnings above."
    fi
    exit 0
else
    echo -e "${RED}Setup incomplete: $ERRORS errors, $WARNINGS warnings${NC}"
    echo ""
    echo "Please fix the errors above before proceeding."
    exit 1
fi