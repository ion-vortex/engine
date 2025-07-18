#!/usr/bin/env bash
# Bootstrap script for Ion Vortex - Linux/macOS
set -euo pipefail

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Helper functions
log_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
}

log_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Detect OS
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macOS"
else
    log_error "Unsupported OS: $OSTYPE"
    exit 1
fi

log_info "Detected OS: $OS"

# Check for required tools
log_info "Checking prerequisites..."

# Check CMake
if ! command -v cmake &> /dev/null; then
    log_error "CMake not found. Please install CMake 3.28 or later."
    exit 1
fi

CMAKE_VERSION=$(cmake --version | head -n1 | cut -d' ' -f3)
CMAKE_MAJOR=$(echo $CMAKE_VERSION | cut -d. -f1)
CMAKE_MINOR=$(echo $CMAKE_VERSION | cut -d. -f2)

if [ "$CMAKE_MAJOR" -lt 3 ] || ([ "$CMAKE_MAJOR" -eq 3 ] && [ "$CMAKE_MINOR" -lt 28 ]); then
    log_error "CMake version $CMAKE_VERSION is too old. Need 3.28 or later."
    exit 1
fi
log_info "Found CMake $CMAKE_VERSION ✓"

# Check Ninja (required on Linux/macOS)
if ! command -v ninja &> /dev/null; then
    log_error "Ninja not found. Please install ninja-build."
    if [[ "$OS" == "Linux" ]]; then
        log_info "Try: sudo apt install ninja-build"
    elif [[ "$OS" == "macOS" ]]; then
        log_info "Try: brew install ninja"
    fi
    exit 1
fi
log_info "Found Ninja ✓"

# Check for C++ compiler
if command -v clang++ &> /dev/null; then
    CXX_COMPILER="clang++"
    CXX_VERSION=$($CXX_COMPILER --version | head -n1)
elif command -v g++ &> /dev/null; then
    CXX_COMPILER="g++"
    CXX_VERSION=$($CXX_COMPILER --version | head -n1)
else
    log_error "No C++ compiler found. Please install clang++ or g++."
    exit 1
fi
log_info "Found C++ compiler: $CXX_VERSION ✓"

# Check for autotools (required for some vcpkg packages)
log_info "Checking for build dependencies..."

MISSING_TOOLS=()

if ! command -v autoconf &> /dev/null; then
    MISSING_TOOLS+=("autoconf")
fi

if ! command -v automake &> /dev/null; then
    MISSING_TOOLS+=("automake")
fi

if ! command -v libtool &> /dev/null; then
    MISSING_TOOLS+=("libtool")
fi

if ! command -v aclocal &> /dev/null; then
    # aclocal comes with automake
    if [[ ! " ${MISSING_TOOLS[@]} " =~ " automake " ]]; then
        MISSING_TOOLS+=("automake")
    fi
fi

if ! command -v pkg-config &> /dev/null; then
    MISSING_TOOLS+=("pkg-config")
fi

if [ ${#MISSING_TOOLS[@]} -gt 0 ]; then
    log_error "Missing required tools: ${MISSING_TOOLS[*]}"
    log_info "These are needed by some dependencies (e.g., libsodium)"
    
    if [[ "$OS" == "Linux" ]]; then
        log_info "Install with: sudo apt install ${MISSING_TOOLS[*]}"
    elif [[ "$OS" == "macOS" ]]; then
        log_info "Install with: brew install ${MISSING_TOOLS[*]}"
    fi
    exit 1
else
    log_info "Found autotools and pkg-config ✓"
fi

# Check/setup vcpkg
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"

# Check if VCPKG_ROOT is set and valid
if [ -n "${VCPKG_ROOT:-}" ] && [ -f "$VCPKG_ROOT/vcpkg" ]; then
    log_info "Using existing vcpkg at: $VCPKG_ROOT"
else
    # Use local vcpkg in project
    VCPKG_ROOT="$PROJECT_ROOT/vcpkg-instance"
    
    if [ ! -d "$VCPKG_ROOT" ]; then
        log_info "Cloning vcpkg..."
        git clone https://github.com/Microsoft/vcpkg.git "$VCPKG_ROOT"
    fi
    
    if [ ! -f "$VCPKG_ROOT/vcpkg" ]; then
        log_info "Bootstrapping vcpkg..."
        cd "$VCPKG_ROOT"
        ./bootstrap-vcpkg.sh
        cd "$PROJECT_ROOT"
    fi
    
    export VCPKG_ROOT
    log_info "vcpkg ready at: $VCPKG_ROOT"
fi

# Install dependencies via vcpkg
log_info "Installing dependencies via vcpkg..."
log_info "This may take 10-30 minutes on first run..."

cd "$PROJECT_ROOT"

# Create a temporary cmake file to trigger vcpkg
TEMP_CMAKE_DIR="$PROJECT_ROOT/build/bootstrap-temp"
mkdir -p "$TEMP_CMAKE_DIR"

# Configure with appropriate preset based on OS
if [[ "$OS" == "Linux" ]]; then
    PRESET="debug-linux"
elif [[ "$OS" == "macOS" ]]; then
    PRESET="debug-macos"
fi

log_info "Running CMake configure to install vcpkg dependencies..."
cmake -S . -B "$TEMP_CMAKE_DIR" --preset "$PRESET" 2>&1 | while IFS= read -r line; do
    # Filter out the most verbose vcpkg output but keep important stuff
    if [[ "$line" =~ "error"|"Error"|"ERROR"|"warning"|"Warning"|"failed"|"Failed" ]] || 
       [[ "$line" =~ "Building"|"Installing"|"Detecting compiler" ]] ||
       [[ "$line" =~ ^--.*$ ]]; then
        echo "$line"
    fi
done

# Check if configure succeeded
if [ ${PIPESTATUS[0]} -ne 0 ]; then
    log_error "CMake configuration failed. Check the output above."
    exit 1
fi

# Clean up temp directory
rm -rf "$TEMP_CMAKE_DIR"

log_info ""
log_info "Bootstrap complete! ✓"
log_info ""
log_info "Next steps:"
log_info "  1. Configure: cmake --preset $PRESET"
log_info "  2. Build:     cmake --build --preset $PRESET"
log_info "  3. Run:       ./build/debug/$OS/apps/client/client"
log_info ""
log_info "For release builds, use: release-${OS,,}"