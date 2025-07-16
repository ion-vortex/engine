# libs/CMakeLists.txt
# Add all library subdirectories

# Order matters! Dependencies must come first
add_subdirectory(core)      # No dependencies
add_subdirectory(crypto)    # Depends on core
add_subdirectory(asset)     # Depends on core, crypto
add_subdirectory(physics)   # Depends on core
add_subdirectory(audio)     # Depends on core
add_subdirectory(protocol)  # Depends on core, crypto
add_subdirectory(render)    # Depends on core, physics (read-only)
add_subdirectory(ui)        # Depends on core, render