cmake_minimum_required(VERSION 3.28)

# ── Project declaration ───────────────────────────────────────
project(Oxide
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Lightweight real-time space-sim framework"
)

# ── Module path for our helpers ───────────────────────────────
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ── Build Options ─────────────────────────────────────────────
option(OXIDE_BUILD_APPS         "Build applications" ON)
option(OXIDE_BUILD_TESTS        "Build tests" OFF)
option(OXIDE_ENABLE_UNITY_BUILD "Enable unity builds for faster compilation" ON)
option(OXIDE_ENABLE_LTO         "Enable Link Time Optimization" ON)
option(OXIDE_ENABLE_MARCH_NATIVE "Use -march=native in Release" OFF)
option(OXIDE_USE_PRIVATE_ASSETS "Use proprietary asset packs" OFF)

# ── VCPKG configuration ───────────────────────────────────────
if(NOT CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()
set(VCPKG_LIBRARY_LINKAGE static CACHE STRING "Link vcpkg packages statically")

# ── C++ Standard ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES  OFF)

# ── Global build settings ─────────────────────────────────────
set(CMAKE_UNITY_BUILD ${OXIDE_ENABLE_UNITY_BUILD})
set(CMAKE_PCH_ENABLE  ON)

if(OXIDE_ENABLE_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
endif()

# ── Load our helper functions ─────────────────────────────────
include(OxideHelpers)

# ── Set up the global build interface ─────────────────────────
oxide_setup_build_interface()

# ── Find all external dependencies upfront ────────────────────
oxide_find_dependencies(
    stb assimp bgfx bullet cgltf curl glfw glm imgui 
    sodium libuv meshoptimizer miniaudio json toml zstd
)

# ── Enable testing if requested ───────────────────────────────
if(OXIDE_BUILD_TESTS)
    enable_testing()
    find_package(Catch2 CONFIG REQUIRED)
endif()

# ── Add subdirectories ────────────────────────────────────────
add_subdirectory(libs)

if(OXIDE_BUILD_APPS)
    add_subdirectory(apps)
endif()

# ── Installation and packaging ────────────────────────────────
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Export targets
install(EXPORT OxideTargets
    FILE OxideTargets.cmake
    NAMESPACE oxide::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/oxide
)

# Create package config file
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OxideConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/OxideConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/oxide
)

# Create version file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/OxideConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# Install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/OxideConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/OxideConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/oxide
)