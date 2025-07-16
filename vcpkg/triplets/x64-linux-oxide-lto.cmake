# x64-linux-oxide-lto.cmake
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)

# Release flags
set(VCPKG_C_FLAGS_RELEASE    "-O3 -flto")
set(VCPKG_CXX_FLAGS_RELEASE  "-O3 -flto")
set(VCPKG_LINKER_FLAGS_RELEASE "-flto")

# Debug flags – keep them normal (sanitizers don’t mix with LTO)
set(VCPKG_C_FLAGS_DEBUG    "-g")
set(VCPKG_CXX_FLAGS_DEBUG  "-g")

# System Configuration
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
