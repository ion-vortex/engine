# Creating applications

This guide covers creating applications that use the Ion Vortex engine, from simple tools to full games.

## Overview

Ion Vortex applications are:
- **Standalone executables** that link Ion Vortex libraries
- **Platform-aware** with proper handling for Windows, Linux, macOS
- **Resource-managed** using RAII and smart pointers
- **Error-handling** with `std::expected` throughout

## application Structure

```
apps/your_app/
├── src/
│   ├── main.cpp              # Entry point
│   ├── application.cpp       # Main app class
│   ├── application.h         # App class header
│   └── systems/              # App-specific systems
│       ├── game_logic.cpp
│       └── game_logic.h
├── assets/                   # App-specific assets
│   ├── shaders/
│   ├── models/
│   └── config/
├── CMakeLists.txt
└── README.md
```

## Step-by-Step Guide

### Step 1: Plan Your application

Consider:
1. **What type of app?** (Game, tool, server, etc.)
2. **Which Ion Vortex libraries are needed?**
3. **Platform requirements?**
4. **Asset requirements?**

### Step 2: create Directory

```bash
mkdir -p apps/your_app/src
cd apps/your_app
```

### Step 3: Write CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.28)

# Support standalone build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(YourApp VERSION 0.1.0 LANGUAGES CXX)
    
    # Add Ion Vortex helpers
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
    include(IonHelpers)
    ion_setup_build_interface()
    
    # Build all required libraries
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../libs libs)
endif()

# create application - dependencies auto-resolved
ion_add_application(
    NAME your_app
    DEPENDENCIES
        ion::core
        ion::render
        ion::ui
        ion::audio
        ion::protocol
        # Add what you need
)

# Platform-specific settings
if(WIN32)
    set_target_properties(your_app PROPERTIES
        WIN32_EXECUTABLE TRUE  # No console window
    )
    
    # Copy assets on Windows
    add_custom_command(TARGET your_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:your_app>/assets
    )
elseif(APPLE)
    set_target_properties(your_app PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.oatinteractive.yourapp"
        MACOSX_BUNDLE_BUNDLE_NAME "Your App"
    )
    
    # Copy assets into bundle
    set_source_files_properties(
        assets/
        PROPERTIES MACOSX_PACKAGE_LOCATION Resources
    )
endif()

# Copy assets for Linux/development
if(NOT WIN32 AND NOT APPLE)
    add_custom_command(TARGET your_app POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:your_app>/assets
    )
endif()
```

### Step 4: Create `application` Class

`src/application.h`:

```cpp
#pragma once

#include <ion/core/error/error.h>
#include <ion/core/logging/logger.h>
#include <ion/render/backend/render_backend.h>
#include <ion/ui/ui_system.h>
#include <ion/audio/audio_engine.h>
#include <ion/platform/window.h>

#include <memory>
#include <expected>
#include <chrono>

namespace your_app {

class application {
public:
    // Configuration
    struct config {
        std::string title = "Your App";
        int width = 1280;
        int height = 720;
        bool fullscreen = false;
        bool vsync = true;
        ion::core::logger_base* logger = nullptr;
    };
    
    // Factory
    [[ION_NODISCARD("Please make sure to handle the creation failure.")]]
    static std::expected<std::unique_ptr<application>, std::error_code> 
    create(const config& config);
    
    // Lifecycle
    void run();
    void shutdown();
    
    ~application();
    
private:
    application(const config& config);
    
    [[ION_NODISCARD("Please ensure to handle init failure.")]]
    std::expected<void, std::error_code> initialize();
    
    void update(float delta_time);
    void render();
    void handle_input();
    
    // Core systems (all through interfaces)
    config config_;
    std::unique_ptr<ion::platform::window_base> window_;
    std::unique_ptr<ion::render::render_backend_base> renderer_;
    std::unique_ptr<ion::ui::ui_system_base> ui_;
    std::unique_ptr<ion::audio::audio_engine_base> audio_;
    
    // State
    bool running_ = true;
    std::chrono::steady_clock::time_point last_frame_time_;
    
    // Prevent copying
    application(const application&) = delete;
    application& operator=(const application&) = delete;
};

} // namespace your_app
```

`src/application.cpp`:

```cpp
#include "application.h"

namespace your_app {

std::expected<std::unique_ptr<application>, std::error_code>
application::create(const config& config) {
    // create instance
    auto app = std::unique_ptr<application>(new application(config));
    
    // Initialize
    if (auto result = app->initialize(); !result) {
        return std::unexpected(result.error());
    }
    
    return app;
}

application::application(const config& config) 
    : config_(config) {
}

application::~application() {
    shutdown();
}

std::expected<void, std::error_code> application::initialize() {
    using namespace ion::core;
    
    // create window through ion platform system
    ion::platform::window_config window_config;
    window_config.title = config_.title;
    window_config.width = config_.width;
    window_config.height = config_.height;
    window_config.fullscreen = config_.fullscreen;
    
    auto window_result = ion::platform::make_window(window_config);
    if (!window_result) {
        return std::unexpected(window_result.error());
    }
    window_ = std::move(window_result.value());
    
    // Initialize renderer
    auto render_result = ion::render::make_render_backend(window_.get());
    if (!render_result) {
        return std::unexpected(render_result.error());
    }
    renderer_ = std::move(render_result.value());
    
    // Initialize UI
    auto ui_result = ion::ui::make_ui_system(window_.get(), renderer_.get());
    if (!ui_result) {
        return std::unexpected(ui_result.error());
    }
    ui_ = std::move(ui_result.value());
    
    // Initialize audio
    auto audio_result = ion::audio::make_audio_engine();
    if (!audio_result) {
        if (config_.logger) {
            config_.logger->log(ion::core::log_level::warn,
                "Audio initialization failed: " + 
                std::string(audio_result.error().what())
            );
        }
        // Audio is optional, continue without it
    } else {
        audio_ = std::move(audio_result.value());
    }
    
    // Setup window callbacks
    window_->set_close_callbacks([this]() {
        running_ = false;
    });
    
    last_frame_time_ = std::chrono::steady_clock::now();
    
    return {};
}

void application::run() {
    while (running_ && !window_->should_close()) {
        // Calculate delta time
        auto now = std::chrono::steady_clock::now();
        float delta_time = std::chrono::duration<float>(now - last_frame_time_).count();
        last_frame_time_ = now;
        
        // Poll events
        window_->poll_events();
        
        // Update
        handle_input();
        update(delta_time);
        
        // Render
        render();
    }
}

void application::update(float delta_time) {
    // Update game logic
    
    // Update audio
    if (audio_) {
        audio_->update();
    }
}

void application::render() {
    if (auto& fb = renderer_->frame(); fb.has_value())
    {
        // Clear
        renderer_->clear(0x2a2a2aff);  // Dark gray
        
        // Render game objects
        // ...
        
        // UI
        if (auto& frame = _ui->frame(); frame.has_value()) {
            if (auto& menu_bar = _ui->menu_bar(); menu_bar.has_value()) {
                if (auto& menu = _ui->menu("File"); menu.has_value()) {
                    if (auto& item = _ui->menu_item("Exit"); item.has_value()) {
                        running_ = false;
                    } // item cleans up automatically when it's out of scope.
                } // ... etc
            }
        }
        
        // Debug window
        if (auto& window = _ui->window("Debug"); window.has_value()) {
            ui_->text("FPS: %.1f", ui_->get_framerate());
        } // window cleans up automatically when it's out of scope.
        
        ui_->render();
    } // renderer automatically swaps the buffers, etc, when fb goes out of scope.
}

void application::handle_input() {
    // Handle keyboard through window interface
    if (window_->is_key_pressed(ion::input::Key::Escape)) {
        running_ = false;
    }
    
    // Handle gamepad through input system
    // ...
}

void application::shutdown() {
    // Clean shutdown in reverse order
    audio_.reset();
    ui_.reset();
    renderer_.reset();
    window_.reset();
}

} // namespace your_app
```

### Step 5: create Main Entry Point

`src/main.cpp`:

```cpp
#include "application.h"
#include <ion/core/logging/logger.h>

#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    // create logger
    auto logger = ion::core::make_console_logger();
    
    // Parse command line arguments
    your_app::application::config config;
    config.logger = logger.get();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--fullscreen" || arg == "-f") {
            config.fullscreen = true;
        } else if (arg == "--windowed" || arg == "-w") {
            config.fullscreen = false;
        } else if (arg == "--resolution" && i + 1 < argc) {
            // Parse resolution like "1920x1080"
            std::string res = argv[++i];
            auto x_pos = res.find('x');
            if (x_pos != std::string::npos) {
                config.width = std::stoi(res.substr(0, x_pos));
                config.height = std::stoi(res.substr(x_pos + 1));
            }
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  --fullscreen, -f     Start in fullscreen mode\n"
                      << "  --windowed, -w       Start in windowed mode\n"
                      << "  --resolution WxH     Set resolution (e.g., 1920x1080)\n"
                      << "  --help, -h           Show this help\n";
            return 0;
        }
    }
    
    // create and run application
    logger->log(ion::core::log_level::info, "Starting application...");
    
    auto app_result = your_app::application::create(config);
    if (!app_result) {
        logger->log(ion::core::log_level::error,
            "Failed to create application: " + 
            std::string(app_result.error().what())
        );
        return 1;
    }
    
    try {
        app_result.value()->run();
    } catch (const std::exception& e) {
        logger->log(ion::core::log_level::critical,
            "Unhandled exception: " + std::string(e.what())
        );
        return 2;
    }
    
    logger->log(ion::core::log_level::info, "application terminated normally");
    return 0;
}
```

### Step 6: Add to Build System

Edit `apps/CMakeLists.txt`:

```cmake
add_subdirectory(client)
add_subdirectory(your_app)  # Add this
add_subdirectory(unid)
add_subdirectory(zoned)
```

## application Types

### Game Client

Full game with rendering, audio, networking:

```cpp
class game_client : public application {
    // Game-specific systems
    std::unique_ptr<game_world_base> world_;
    std::unique_ptr<network_client_base> network_;
    std::unique_ptr<input_manager_base> input_;
    
    void update(float dt) override {
        network_->receive();
        input_->process();
        world_->simulate(dt);
        network_->send();
    }
};
```

### Dedicated Server

Headless server without rendering:

```cpp
class game_server {
    std::unique_ptr<network_server_base> network_;
    std::unique_ptr<game_sim_base> simulation_;
    
    void run() {
        while (running_) {
            network_->accept();
            network_->receive();
            simulation_->tick();
            network_->broadcast();
            
            std::this_thread::sleep_for(33ms);  // 30 Hz
        }
    }
};
```

### Tool/Editor

Focused on UI with file operations:

```cpp
class level_editor : public application {
    void render_ui() {
        // Tool windows
        render_scene_hierarchy();
        render_property_editor();
        render_viewport();
    }
};
```

## Platform Considerations

### Windows

```cpp
#ifdef _WIN32
    // Windows-specific code
    
    // Unicode support
    int wmain(int argc, wchar_t* argv[]) {
        // Convert to UTF-8
        // ...
    }
    
    // High DPI support - handled by platform system
    // window_->setDPIAware(true);
#endif
```

### Linux

```cpp
#ifdef __linux__
    // Set app class for window managers - handled by platform system
    // window_->setX11Properties("YourApp", "your_app");
#endif
```

### macOS

```cpp
#ifdef __APPLE__
    // Retina display support - handled by platform system
    // window_->setRetinaSupport(true);
    
    // App bundle resources
    CFBundleRef bundle = CFBundleGetMainBundle();
    // ...
#endif
```

## Asset Management

### Loading Assets

```cpp
class AssetManager {
    std::expected<void, std::error_code> load_from_directory(std::string_view path) {
        namespace fs = std::filesystem;
        
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".toml") {
                auto result = loadconfig(entry.path());
                if (!result) return result;
            }
        }
        
        return {};
    }
};
```

### Embedded Resources

For small resources, embed directly:

```cpp
// Generated from assets
namespace embedded {
    extern const uint8_t default_shader[];
    extern const size_t default_shader_size;
}

auto shader = renderer_->create_shader(
    embedded::default_shader,
    embedded::default_shader_size
);
```

## Error Handling Patterns

### Graceful Degradation

```cpp
// Try to load user config, fall back to defaults
auto config_result = load_config("user.toml");
if (!config_result) {
    logger->warn("Using default config: " + config_result.error().what());
    config = get_default_config();
} else {
    config = config_result.value();
}
```

### Fatal Errors

```cpp
auto critical_resource = load_critical_resources();
if (!critical_resource) {
    // Log and show user-friendly error
    logger->error("Failed to load critical resource");
    
    show_error_dialog(
        "Startup Error",
        "Required files are missing. Please reinstall."
    );
    
    return EXIT_FAILURE;
}
```

## Performance Tips

### Frame Timing

```cpp
class frame_timer {
    static constexpr auto k_target_frame_time = 16.667ms;  // 60 FPS
    
    void regulate_frame_rate() {
        auto frame_time = clock::now() - frame_start_;
        
        if (frame_time < k_target_frame_time) {
            std::this_thread::sleep_for(
                k_target_frame_time - frame_time
            );
        }
    }
};
```

### Resource Pooling

```cpp
class particle_system {
    // Pre-allocate particles
    std::vector<particle> particle_pool_;
    std::queue<size_t> available_indices_;
    
    void emit() {
        if (available_indices_.empty()) return;
        
        size_t idx = available_indices_.front();
        available_indices_.pop();
        
        particle_pool_[idx].reset();
        particle_pool_[idx].active = true;
    }
};
```

## Testing applications

### Integration Tests

```cpp
TEST_CASE("application lifecycle", "[app]") {
    application::config config;
    config.headless = true;  // No window
    
    auto app = application::create(config);
    REQUIRE(app.has_value());
    
    // Simulate frames
    for (int i = 0; i < 60; ++i) {
        app.value()->update(1.0f / 60.0f);
    }
    
    // Should shut down cleanly
    app.value()->shutdown();
}
```

### Automated UI Tests

```cpp
TEST_CASE("UI interactions", "[app][ui]") {
    auto app = create_test_app();
    
    // Simulate input
    app->inject_mouse_click(100, 200);
    app->inject_key_press(SDL_KEY_SPACE);
    
    // Check state
    REQUIRE(app->is_menu_open());
}
```

## Distribution

### Package Structure

```
YourApp/
├── your_app.exe          # Main executable
├── assets/               # Game assets
│   ├── shaders/
│   ├── models/
│   └── config/
├── libs/                 # Required shared libraries (if any)
└── README.txt           # User documentation
```

### Build configurations

Release build for distribution:
```bash
cmake --preset release-windows
cmake --build --preset release-windows --target your_app
cmake --install build/release/Windows --prefix dist/
```

## Best Practices

### DO:
- ✅ Handle all errors gracefully
- ✅ Support command-line arguments
- ✅ Log important events
- ✅ Clean up resources properly
- ✅ Test on all target platforms
- ✅ Profile performance regularly

### DON'T:
- ❌ Assume assets exist
- ❌ Hard-code paths
- ❌ Ignore error returns
- ❌ Block the main thread
- ❌ Leak resources
- ❌ Use global state

## Example applications

Study these for reference:
- `apps/client/` - Full game client
- `apps/zoned/` - Headless server
- `apps/cube_viewer/` - Simple tutorial app

## Next Steps

1. Build and test your application
2. Add features incrementally
3. Profile and optimize
4. Package for distribution
5. Gather user feedback

Remember: Start simple, iterate quickly, test thoroughly!