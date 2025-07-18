cmake_minimum_required(VERSION 3.28)

# Allow building as standalone project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(IonCore VERSION 0.1.0 LANGUAGES CXX)
    
    # Set up module path for helpers
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
    
    # Load helpers and set up build interface
    include(IonHelpers)
    include(IonExportHeader)
    ion_setup_build_interface()
    
    # Find external dependencies that core needs
    ion_find_dependencies(json toml glm)
    
    # Enable testing if this is the main project
    option(BUILD_TESTING "Build tests" ON)
    if(BUILD_TESTING)
        enable_testing()
        find_package(Catch2 CONFIG REQUIRED)
    endif()
endif()

# Register external dependencies BEFORE creating the library
ion_add_external_dependency(core
    glm::glm
    nlohmann_json::nlohmann_json
    tomlplusplus::tomlplusplus
)

# Create library - sources are auto-discovered
ion_add_library(
    NAME core
    DEPENDENCIES ion::build  # core has no internal deps
)

# Generate export header
ion_generate_export_header(ion_core)

# Link the external dependencies
target_link_libraries(ion_core PUBLIC
    glm::glm
    nlohmann_json::nlohmann_json
    tomlplusplus::tomlplusplus
)

# ── Tests ─────────────────────────────────────────────────────
if(BUILD_TESTING OR ION_BUILD_TESTS)
    add_subdirectory(tests)
endif()