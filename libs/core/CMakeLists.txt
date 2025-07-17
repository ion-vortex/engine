cmake_minimum_required(VERSION 3.28)

# Allow building as standalone project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(OxideCore VERSION 0.1.0 LANGUAGES CXX)
    
    # Set up module path for helpers
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
    
    # Load helpers and set up build interface
    include(OxideHelpers)
    include(OxideExportHeader)
    oxide_setup_build_interface()
    
    # Find external dependencies that core needs
    oxide_find_dependencies(json toml glm)
    
    # Enable testing if this is the main project
    option(BUILD_TESTING "Build tests" ON)
    if(BUILD_TESTING)
        enable_testing()
        find_package(Catch2 CONFIG REQUIRED)
    endif()
endif()

# Register external dependencies BEFORE creating the library
oxide_add_external_dependency(core
    glm::glm
    nlohmann_json::nlohmann_json
    tomlplusplus::tomlplusplus
)

# Create library - sources are auto-discovered
oxide_add_library(
    NAME core
    DEPENDENCIES oxide::build  # core has no internal deps
)

# Generate export header
oxide_generate_export_header(oxide_core)

# Link the external dependencies
target_link_libraries(oxide_core PUBLIC
    glm::glm
    nlohmann_json::nlohmann_json
    tomlplusplus::tomlplusplus
)

# ── Tests ─────────────────────────────────────────────────────
if(BUILD_TESTING OR OXIDE_BUILD_TESTS)
    add_subdirectory(tests)
endif()