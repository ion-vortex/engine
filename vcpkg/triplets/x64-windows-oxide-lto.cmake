# x64-windows-oxide-lto.cmake
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE         static)   # /MT or /MTd
set(VCPKG_LIBRARY_LINKAGE     static)

# ---- Release --------------------------------------------------------------
# /GL enables “Whole Program Optimisation” → LTCG object files
set(VCPKG_C_FLAGS_RELEASE      "/O2 /GL")
set(VCPKG_CXX_FLAGS_RELEASE    "/O2 /GL")
set(VCPKG_LINKER_FLAGS_RELEASE "/LTCG")

# ---- Debug ---------------------------------------------------------------
set(VCPKG_C_FLAGS_DEBUG        "/Zi /Od")
set(VCPKG_CXX_FLAGS_DEBUG      "/Zi /Od")