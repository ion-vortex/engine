# Generate export headers for shared libraries
function(oxide_generate_export_header TARGET)
    get_target_property(_name ${TARGET} NAME)
    string(REPLACE "oxide_" "" _module ${_name})
    string(TOUPPER ${_module} _module_upper)
    
    set(_export_header "${CMAKE_CURRENT_BINARY_DIR}/include/oxide/${_module}/export.h")

    # Create directory structure
    get_filename_component(_export_header_dir ${_export_header} DIRECTORY)
    file(MAKE_DIRECTORY ${_export_header_dir})
    
    file(WRITE ${_export_header} 
"#pragma once

// Auto-generated export header for ${_module}

#ifdef OXIDE_SHARED_LIBS
    #ifdef _WIN32
        #ifdef OXIDE_${_module_upper}_EXPORTS
            #define OXIDE_${_module_upper}_API __declspec(dllexport)
        #else
            #define OXIDE_${_module_upper}_API __declspec(dllimport)
        #endif
    #else
        #ifdef OXIDE_${_module_upper}_EXPORTS
            #define OXIDE_${_module_upper}_API __attribute__((visibility(\"default\")))
        #else
            #define OXIDE_${_module_upper}_API
        #endif
    #endif
#else
    #define OXIDE_${_module_upper}_API
#endif

// Export macros for templates and inline functions
#define OXIDE_${_module_upper}_TEMPLATE_API
")
    
    target_sources(${TARGET} PRIVATE ${_export_header})
    target_include_directories(${TARGET} PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    
    # Install the generated export header
    install(FILES ${_export_header}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/oxide/${_module}
    )
endfunction()