cmake_minimum_required(VERSION 3.28)
project(oxide_core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Unity builds and PCH
set(CMAKE_UNITY_BUILD ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 16)
set(CMAKE_PCH_ENABLE ON)

# Compiler-specific settings
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    message(STATUS "Configuring for Clang")
    target_compile_options(oxide_core PRIVATE -Weverything -Wno-c++98-compat -Wno-padded)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Configuring for GCC")
    target_compile_options(oxide_core PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Configuring for MSVC")
    target_compile_options(oxide_core PRIVATE /W4 /WX)
endif()

# Collect interface headers and implementation sources
file(GLOB_RECURSE CORE_HEADERS "include/oxide/core/**/*.h")
file(GLOB_RECURSE CORE_SOURCES "src/**/*.cpp")

add_library(oxide_core STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(oxide_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS oxide_core EXPORT oxide_coreConfig
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)