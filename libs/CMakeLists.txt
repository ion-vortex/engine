# libs/CMakeLists.txt
# Add all library subdirectories

# Order matters! Dependencies must come first
add_subdirectory(core)      # No dependencies

# disabling for now
# add_subdirectory(crypto)    # Depends on core

# disabling for now
# add_subdirectory(asset)     # Depends on core, crypto

# disabling for now
# add_subdirectory(physics)   # Depends on core

# disabling for now
# add_subdirectory(audio)     # Depends on core

# disabling for now
# add_subdirectory(protocol)  # Depends on core, crypto

# disabling for now
# add_subdirectory(render)    # Depends on core, physics (read-only)

# disabling for now
# add_subdirectory(ui)        # Depends on core, render