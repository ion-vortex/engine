cmake_minimum_required(VERSION 3.28)
project(Oxide LANGUAGE CXX)

# ── VCPKG configuration ───────────────────────────────────────
if(NOT CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{VCPKG_ROOT})
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()
set(VCPKG_LIBRARY_LINKAGE static CACHE STRING "Link vcpkg packages statically")
# ---------------------------------------------------------------------------
# vcpkg triplet bootstrap (only if caller did not specify one)
# ---------------------------------------------------------------------------
if (NOT DEFINED VCPKG_TARGET_TRIPLET)

    # Inspect host platform -----------------------------------------------
    if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(_triplet "x64-windows-oxide-lto")
        else()
            message(FATAL_ERROR "Unsupported Windows arch")
        endif()

    elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        # CMAKE_OSX_ARCHITECTURES is only populated *after* the first
        # project() call; arm64/x86_64 fat builds need manual handling.
        if (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
            set(_triplet "arm64-osx-oxide-lto")
        else()
            message(FATAL_ERROR "Unsupported MacOS arch")
        endif()

    elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(_triplet "x64-linux-oxide-lto")

    else()
        message(FATAL_ERROR "Unsupported host platform")
    endif()

    # Push into the cache so every sub-dir sees it
    set(VCPKG_TARGET_TRIPLET ${_triplet} CACHE STRING
        "vcpkg triplet chosen automatically by top-level build script")
    message(STATUS "Auto-selected vcpkg triplet: ${VCPKG_TARGET_TRIPLET}")

endif()


# ── C++ Standard ──────────────────────────────────────────────
set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES  OFF)

# ── Build Options ─────────────────────────────────────────────
option(OXIDE_ENABLE_UNITY_BUILD "Enable unity builds for faster compilation" ON)
option(OXIDE_ENABLE_LTO         "Enable Link Time Optimization" ON)
option(OXIDE_ENABLE_MARCH_NATIVE "Use -march=native in Release" OFF)
set(CMAKE_UNITY_BUILD ${OXIDE_ENABLE_UNITY_BUILD})
set(CMAKE_PCH_ENABLE  ON)

if(OXIDE_ENABLE_LTO)
    # enable LTO/IPO only for Release and RelWithDebInfo
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE        ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)
endif()

# ──────────────────────────────────────────────────────────────────────────────
# Oxide build-settings facade
#   • Collects all compiler / linker options in one place
#   • Add with:  target_link_libraries(your_lib PRIVATE oxide::build)
# ──────────────────────────────────────────────────────────────────────────────
add_library(oxide_build INTERFACE)
add_library(oxide::build ALIAS oxide_build)

# ---------- common (all configs / all compilers) -----------------------------
target_compile_features(oxide_build INTERFACE cxx_std_23)

if (MSVC)
    # *Always-on* flags
    target_compile_options(oxide_build INTERFACE
        /W4 /WX
        /permissive-
        /sdl          # basic security checks
        /GR-          # no RTTI
        /EHsc-        # no exceptions (s = C++ EH, c = extern-"C" Θ)
    )
    target_compile_definitions(oxide_build INTERFACE _WIN32_WINNT=0x0601)

    # Debug-only
    target_compile_options(oxide_build INTERFACE
        $<$<CONFIG:Debug>:/Zi /Od>
    )

    # Release-only
    target_compile_options(oxide_build INTERFACE
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )

else()  # GCC / Clang / AppleClang
    # *Always-on*
    target_compile_options(oxide_build INTERFACE
        -Wall -Wextra -Werror
        -fno-exceptions -fno-rtti
    )

    # Debug-only (+ sanitizers)
    target_compile_options(oxide_build INTERFACE
        $<$<CONFIG:Debug>:-g3 -O0 -fsanitize=address -fsanitize=undefined>
    )
    target_link_options(oxide_build INTERFACE
        $<$<CONFIG:Debug>:-fsanitize=address -fsanitize=undefined>
    )

    # Release-only
    target_compile_options(oxide_build INTERFACE
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<AND:$<CONFIG:Release>,$<BOOL:${OXIDE_ENABLE_NATIVE}>>:-march=native>
    )

endif()

# ── Find dependencies ─────────────────────────────────────────
find_package(Stb REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(bgfx CONFIG REQUIRED)
find_package(Bullet CONFIG REQUIRED)
find_path(CGLTF_INCLUDE_DIRS "cgltf.h")
find_package(CURL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(libuv CONFIG REQUIRED)
find_package(meshoptimizer CONFIG REQUIRED)
find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h")
find_package(nlohmann_json CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)

# ── Add various builds ────────────────────────────────────────
add_subdirectory(libs)
add_subdirectory(apps)