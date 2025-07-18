cmake_minimum_required(VERSION 3.28)

# ── Project declaration ───────────────────────────────────────
project(Ion
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "Lightweight real-time space-sim framework"
)

# ── Module path for our helpers ───────────────────────────────
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ── Build Options ─────────────────────────────────────────────
option(ION_BUILD_APPS         "Build applications" ON)
option(ION_BUILD_TESTS        "Build tests" OFF)
option(ION_ENABLE_UNITY_BUILD "Enable unity builds for faster compilation" ON)
option(ION_ENABLE_LTO         "Enable Link Time Optimization" ON)
option(ION_ENABLE_MARCH_NATIVE "Use -march=native in Release" OFF)
option(ION_USE_PRIVATE_ASSETS "Use proprietary asset packs" OFF)

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
set(CMAKE_UNITY_BUILD ${ION_ENABLE_UNITY_BUILD})
set(CMAKE_PCH_ENABLE  ON)

# ── Load our helper functions ─────────────────────────────────
include(IonHelpers)
include(IonExportHeader)

# ── Set up the build environment for shared libraries ─────────
ion_setup_build_environment()

# ── Set up the global build interface ─────────────────────────
ion_setup_build_interface()

# ── Find all external dependencies upfront ────────────────────
ion_find_dependencies(
    stb assimp bgfx bullet cgltf curl glfw glm imgui 
    sodium libuv meshoptimizer miniaudio json toml zstd
)

# ── Enable testing if requested ───────────────────────────────
if(ION_BUILD_TESTS)
    enable_testing()
    find_package(Catch2 CONFIG REQUIRED)
endif()

# ── Add subdirectories ────────────────────────────────────────
add_subdirectory(libs)

if(ION_BUILD_APPS)
    # disabling for now
    # add_subdirectory(apps)
endif()

# ── Installation and packaging ────────────────────────────────
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Export targets
install(EXPORT IonTargets
    FILE IonTargets.cmake
    NAMESPACE ion::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ion
)

# ── Create package config file ────────────────────────────────
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/IonConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/IonConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ion
)

# ── Create version file ───────────────────────────────────────
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/IonConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

# ── Install config files ──────────────────────────────────────
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/IonConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/IonConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ion
)