# cmake/OxideHelpers.cmake
# Shared CMake utilities for Oxide project

include_guard(GLOBAL)

# ──────────────────────────────────────────────────────────────────────────────
# oxide_add_library
#   Creates a library target with standard Oxide settings
#   Automatically discovers sources based on standard layout:
#     - src/**/*.cpp for sources
#     - include/oxide/${NAME}/**/*.h for public headers
#   
# Usage:
#   oxide_add_library(
#     NAME core
#     DEPENDENCIES oxide::build  # Other oxide:: libraries only
#   )
# ──────────────────────────────────────────────────────────────────────────────
function(oxide_add_library)
    cmake_parse_arguments(ARG
        ""  # options
        "NAME"  # one-value keywords
        "DEPENDENCIES"  # multi-value keywords
        ${ARGN}
    )

    if(NOT ARG_NAME)
        message(FATAL_ERROR "oxide_add_library: NAME is required")
    endif()

    # Automatically discover sources
    file(GLOB_RECURSE _sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    )
    
    file(GLOB_RECURSE _public_headers CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/include/oxide/${ARG_NAME}/*.h"
    )

    if(NOT _sources)
        message(FATAL_ERROR "No sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src/")
    endif()

    # Create the library target
    set(_target_name "oxide_${ARG_NAME}")
    add_library(${_target_name} STATIC ${_sources})
    add_library(oxide::${ARG_NAME} ALIAS ${_target_name})

    # Set up include directories
    target_include_directories(${_target_name}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
    )

    # Link dependencies - PUBLIC so they propagate
    if(ARG_DEPENDENCIES)
        target_link_libraries(${_target_name} PUBLIC ${ARG_DEPENDENCIES})
    endif()

    # Always link oxide::build for compiler settings
    if(NOT "oxide::build" IN_LIST ARG_DEPENDENCIES)
        target_link_libraries(${_target_name} PRIVATE oxide::build)
    endif()

    # Set public headers for installation
    set_target_properties(${_target_name} PROPERTIES
        PUBLIC_HEADER "${_public_headers}"
    )

    # Enable unity build if requested
    if(CMAKE_UNITY_BUILD)
        set_target_properties(${_target_name} PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE 16
        )
    endif()

    # Store this library's external dependencies for later retrieval
    # This is needed because we need to collect ALL transitive deps for executables
    get_property(_ext_deps GLOBAL PROPERTY OXIDE_${ARG_NAME}_EXTERNAL_DEPS)
    set_property(GLOBAL PROPERTY OXIDE_${ARG_NAME}_EXTERNAL_DEPS "${_ext_deps}")

    # Export for find_package support
    install(TARGETS ${_target_name}
        EXPORT OxideTargets
        ARCHIVE DESTINATION lib
        PUBLIC_HEADER DESTINATION include/oxide/${ARG_NAME}
    )
endfunction()

# ──────────────────────────────────────────────────────────────────────────────
# oxide_add_external_dependency
#   Registers an external dependency for a library
#   This must be called BEFORE oxide_add_library
#   
# Usage:
#   oxide_add_external_dependency(core glm::glm nlohmann_json::nlohmann_json)
# ──────────────────────────────────────────────────────────────────────────────
function(oxide_add_external_dependency lib_name)
    # Store the external deps in a global property
    set_property(GLOBAL APPEND PROPERTY OXIDE_${lib_name}_EXTERNAL_DEPS ${ARGN})
endfunction()

# ──────────────────────────────────────────────────────────────────────────────
# oxide_collect_all_dependencies
#   Recursively collects all transitive dependencies (internal and external)
#   Used internally by oxide_add_application and oxide_add_test
# ──────────────────────────────────────────────────────────────────────────────
function(oxide_collect_all_dependencies out_var)
    set(_all_deps)
    set(_all_external_deps)
    set(_processed_libs)
    set(_libs_to_process ${ARGN})

    while(_libs_to_process)
        list(POP_FRONT _libs_to_process _current_lib)
        
        # Skip if already processed
        if(_current_lib IN_LIST _processed_libs)
            continue()
        endif()
        
        list(APPEND _processed_libs ${_current_lib})
        
        # Only process oxide:: libraries
        if(_current_lib MATCHES "^oxide::")
            list(APPEND _all_deps ${_current_lib})
            
            # Get the library name without oxide:: prefix
            string(REPLACE "oxide::" "" _lib_name ${_current_lib})
            
            # Get this library's external dependencies
            get_property(_ext_deps GLOBAL PROPERTY OXIDE_${_lib_name}_EXTERNAL_DEPS)
            if(_ext_deps)
                list(APPEND _all_external_deps ${_ext_deps})
            endif()
            
            # Get this library's dependencies
            if(TARGET oxide_${_lib_name})
                get_target_property(_lib_deps oxide_${_lib_name} INTERFACE_LINK_LIBRARIES)
                if(_lib_deps)
                    foreach(_dep ${_lib_deps})
                        if(_dep MATCHES "^oxide::" AND NOT _dep IN_LIST _processed_libs)
                            list(APPEND _libs_to_process ${_dep})
                        endif()
                    endforeach()
                endif()
            endif()
        endif()
    endwhile()

    # Remove duplicates
    list(REMOVE_DUPLICATES _all_deps)
    if(_all_external_deps)
        list(REMOVE_DUPLICATES _all_external_deps)
    endif()

    # Return both internal and external deps
    set(${out_var} ${_all_deps} ${_all_external_deps} PARENT_SCOPE)
endfunction()

# ──────────────────────────────────────────────────────────────────────────────
# oxide_add_application
#   Creates an executable target with standard Oxide settings
#   Automatically discovers sources in src/
#   
# Usage:
#   oxide_add_application(
#     NAME client
#     DEPENDENCIES oxide::core oxide::render oxide::ui
#   )
# ──────────────────────────────────────────────────────────────────────────────
function(oxide_add_application)
    cmake_parse_arguments(ARG
        ""
        "NAME"
        "DEPENDENCIES"
        ${ARGN}
    )

    if(NOT ARG_NAME)
        message(FATAL_ERROR "oxide_add_application: NAME is required")
    endif()

    # Automatically discover sources
    file(GLOB_RECURSE _sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    )

    if(NOT _sources)
        message(FATAL_ERROR "No sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src/")
    endif()

    # Create the executable
    add_executable(${ARG_NAME} ${_sources})

    # Collect ALL transitive dependencies
    oxide_collect_all_dependencies(_all_deps ${ARG_DEPENDENCIES})

    # Link all dependencies
    target_link_libraries(${ARG_NAME}
        PRIVATE
            oxide::build
            ${_all_deps}
    )

    # Enable unity build if requested
    if(CMAKE_UNITY_BUILD)
        set_target_properties(${ARG_NAME} PROPERTIES
            UNITY_BUILD ON
            UNITY_BUILD_BATCH_SIZE 16
        )
    endif()

    # Install the executable
    install(TARGETS ${ARG_NAME}
        RUNTIME DESTINATION bin
    )
endfunction()

# ──────────────────────────────────────────────────────────────────────────────
# oxide_add_test
#   Creates a test executable with Catch2 support
#   Automatically discovers test sources
#   
# Usage:
#   oxide_add_test(
#     NAME core_tests
#     DEPENDENCIES oxide::core
#   )
# ──────────────────────────────────────────────────────────────────────────────
function(oxide_add_test)
    cmake_parse_arguments(ARG
        ""
        "NAME"
        "DEPENDENCIES"
        ${ARGN}
    )

    if(NOT ARG_NAME)
        message(FATAL_ERROR "oxide_add_test: NAME is required")
    endif()

    # Ensure Catch2 is available
    find_package(Catch2 CONFIG REQUIRED)

    # Automatically discover test sources
    file(GLOB_RECURSE _sources CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    )

    if(NOT _sources)
        message(FATAL_ERROR "No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}/")
    endif()

    # Create test executable
    add_executable(${ARG_NAME} ${_sources})

    # Collect ALL transitive dependencies
    oxide_collect_all_dependencies(_all_deps ${ARG_DEPENDENCIES})

    # Link test framework and all dependencies
    target_link_libraries(${ARG_NAME}
        PRIVATE
            oxide::build
            ${_all_deps}
            Catch2::Catch2WithMain
    )

    # Add to CTest
    add_test(NAME ${ARG_NAME} COMMAND ${ARG_NAME})
endfunction()

# ──────────────────────────────────────────────────────────────────────────────
# oxide_find_dependencies
#   Wrapper to find common external dependencies with consistent error handling
# ──────────────────────────────────────────────────────────────────────────────
macro(oxide_find_dependencies)
    foreach(_dep ${ARGN})
        if(_dep STREQUAL "assimp")
            find_package(assimp CONFIG REQUIRED)
        elseif(_dep STREQUAL "bgfx")
            find_package(bgfx CONFIG REQUIRED)
        elseif(_dep STREQUAL "bullet")
            find_package(Bullet CONFIG REQUIRED)
        elseif(_dep STREQUAL "cgltf")
            find_path(CGLTF_INCLUDE_DIRS "cgltf.h" REQUIRED)
        elseif(_dep STREQUAL "curl")
            find_package(CURL REQUIRED)
        elseif(_dep STREQUAL "glfw")
            find_package(glfw3 CONFIG REQUIRED)
        elseif(_dep STREQUAL "glm")
            find_package(glm CONFIG REQUIRED)
        elseif(_dep STREQUAL "imgui")
            find_package(imgui CONFIG REQUIRED)
        elseif(_dep STREQUAL "sodium")
            find_package(unofficial-sodium CONFIG REQUIRED)
        elseif(_dep STREQUAL "libuv")
            find_package(libuv CONFIG REQUIRED)
        elseif(_dep STREQUAL "meshoptimizer")
            find_package(meshoptimizer CONFIG REQUIRED)
        elseif(_dep STREQUAL "miniaudio")
            find_path(MINIAUDIO_INCLUDE_DIRS "miniaudio.h" REQUIRED)
        elseif(_dep STREQUAL "json")
            find_package(nlohmann_json CONFIG REQUIRED)
        elseif(_dep STREQUAL "toml")
            find_package(tomlplusplus CONFIG REQUIRED)
        elseif(_dep STREQUAL "zstd")
            find_package(zstd CONFIG REQUIRED)
        elseif(_dep STREQUAL "stb")
            find_package(Stb REQUIRED)
        else()
            message(FATAL_ERROR "Unknown dependency: ${_dep}")
        endif()
    endforeach()
endmacro()

# ──────────────────────────────────────────────────────────────────────────────
# oxide_setup_build_interface
#   Creates the oxide::build interface target if not already present
# ──────────────────────────────────────────────────────────────────────────────
function(oxide_setup_build_interface)
    if(TARGET oxide::build)
        return()
    endif()

    add_library(oxide_build INTERFACE)
    add_library(oxide::build ALIAS oxide_build)

    # C++ standard
    target_compile_features(oxide_build INTERFACE cxx_std_23)

    # Compiler-specific flags
    if(MSVC)
        target_compile_options(oxide_build INTERFACE
            /W4 /WX
            /permissive-
            /sdl
        )
        target_compile_definitions(oxide_build INTERFACE _WIN32_WINNT=0x0601)

        target_compile_options(oxide_build INTERFACE
            $<$<CONFIG:Debug>:/Zi /Od>
            $<$<CONFIG:Release>:/O2 /DNDEBUG>
        )
    else()
        target_compile_options(oxide_build INTERFACE
            -Wall -Wextra -Werror
        )

        target_compile_options(oxide_build INTERFACE
            $<$<CONFIG:Debug>:-g3 -O0 -fsanitize=address -fsanitize=undefined>
        )
        target_link_options(oxide_build INTERFACE
            $<$<CONFIG:Debug>:-fsanitize=address -fsanitize=undefined>
        )

        target_compile_options(oxide_build INTERFACE
            $<$<CONFIG:Release>:-O3 -DNDEBUG>
            $<$<AND:$<CONFIG:Release>,$<BOOL:${OXIDE_ENABLE_MARCH_NATIVE}>>:-march=native>
        )
    endif()

    # install build-interface so that each library's public dependencies are satisfied
    install(TARGETS oxide_build
        EXPORT OxideTargets
        INCLUDES DESTINATION include
    )
endfunction()