# arm64-osx-oxide-lto.cmake
set(VCPKG_TARGET_ARCHITECTURE arm64)
set(VCPKG_LIBRARY_LINKAGE      static)
set(VCPKG_CRT_LINKAGE          static)

# ---- Release --------------------------------------------------------------
set(VCPKG_C_FLAGS_RELEASE      "-O3 -flto")
set(VCPKG_CXX_FLAGS_RELEASE    "-O3 -flto")
set(VCPKG_LINKER_FLAGS_RELEASE "-flto")

# ---- Debug ---------------------------------------------------------------
set(VCPKG_C_FLAGS_DEBUG        "-g")
set(VCPKG_CXX_FLAGS_DEBUG      "-g")

# System Configuration
set(VCPKG_CMAKE_SYSTEM_NAME Darwin)