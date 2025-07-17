# Creating Applications

This guide covers creating applications that use the Oxide framework, from simple tools to full games.

## Overview

Oxide applications are:
- **Standalone executables** that link Oxide libraries
- **Platform-aware** with proper handling for Windows, Linux, macOS
- **Resource-managed** using RAII and smart pointers
- **Error-handling** with `std::expected` throughout

## Application Structure

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

### Step 1: Plan Your Application

Consider:
1. **What type of app?** (Game, tool, server, etc.)
2. **Which Oxide libraries needed?**
3. **Platform requirements?**
4. **Asset requirements?**

### Step 2: Create Directory

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
    
    # Add Oxide helpers
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
    include(OxideHelpers)
    oxide_setup_build_interface()
    
    # Build all required libraries
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../libs libs)
endif()

# Create application - dependencies auto-resolved
oxide_add_application(
    NAME your_app
    DEPENDENCIES
        oxide::core
        oxide::render
        oxide::ui
        oxide::audio
        oxide::protocol
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

### Step 4: Create Application Class

`src/application.h`:

```cpp
#pragma once

#include <oxide/core/error/error.h>
#include <oxide/core/logging/logger.h>
#include <oxide/render/backend/render_backend.h>
#include <oxide/ui/ui_system.h>
#include <oxide/audio/audio_engine.h>
#include <oxide/platform/window.h>

#include <memory>
#include <expected>
#include <chrono>

namespace your_app {

class Application {
public:
    // Configuration
    struct Config {
        std::string title = "Your App";
        int width = 1280;
        int height = 720;
        bool fullscreen = false;
        bool vsync = true;
        oxide::core::ILogger* logger = nullptr;
    };
    
    // Factory
    [[nodiscard("Handle creation failure"), gnu::warn_unused_result]]
    static std::expected<std::unique_ptr<Application>, oxide::core::Error> 
    Create(const Config& config);
    
    // Lifecycle
    void run();
    void shutdown();
    
    ~Application();
    
private:
    Application(const Config& config);
    
    [[nodiscard, gnu::warn_unused_result]]

    std::expected<void, oxide::core::Error> initialize();
    
    void update(float delta_time);
    void render();
    void handleInput();
    
    // Core systems (all through interfaces)
    Config config_;
    std::unique_ptr<oxide::platform::IWindow> window_;
    std::unique_ptr<oxide::render::IRenderBackend> renderer_;
    std::unique_ptr<oxide::ui::IUISystem> ui_;
    std::unique_ptr<oxide::audio::IAudioEngine> audio_;
    
    // State
    bool running_ = true;
    std::chrono::steady_clock::time_point last_frame_time_;
    
    // Prevent copying
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;
};

} // namespace your_app
```

`src/application.cpp`:

```cpp
#include "application.h"

namespace your_app {

std::expected<std::unique_ptr<Application>, oxide::core::Error>
Application::Create(const Config& config) {
    // Create instance
    auto app = std::unique_ptr<Application>(new Application(config));
    
    // Initialize
    if (auto result = app->initialize(); !result) {
        return std::unexpected(result.error());
    }
    
    return app;
}

Application::Application(const Config& config) 
    : config_(config) {
}

Application::~Application() {
    shutdown();
}

std::expected<void, oxide::core::Error> Application::initialize() {
    using namespace oxide::core;
    
    // Create window through oxide platform system
    oxide::platform::WindowConfig window_config;
    window_config.title = config_.title;
    window_config.width = config_.width;
    window_config.height = config_.height;
    window_config.fullscreen = config_.fullscreen;
    
    auto window_result = oxide::platform::makeWindow(window_config);
    if (!window_result) {
        return std::unexpected(window_result.error());
    }
    window_ = std::move(window_result.value());
    
    // Initialize renderer
    auto render_result = oxide::render::makeRenderBackend(window_.get());
    if (!render_result) {
        return std::unexpected(render_result.error());
    }
    renderer_ = std::move(render_result.value());
    
    // Initialize UI
    auto ui_result = oxide::ui::makeUISystem(window_.get(), renderer_.get());
    if (!ui_result) {
        return std::unexpected(ui_result.error());
    }
    ui_ = std::move(ui_result.value());
    
    // Initialize audio
    auto audio_result = oxide::audio::makeAudioEngine();
    if (!audio_result) {
        if (config_.logger) {
            config_.logger->log(oxide::core::LogLevel::Warn,
                "Audio initialization failed: " + 
                std::string(audio_result.error().what())
            );
        }
        // Audio is optional, continue without it
    } else {
        audio_ = std::move(audio_result.value());
    }
    
    // Setup window callbacks
    window_->setCloseCallback([this]() {
        running_ = false;
    });
    
    last_frame_time_ = std::chrono::steady_clock::now();
    
    return {};
}

void Application::run() {
    while (running_ && !window_->shouldClose()) {
        // Calculate delta time
        auto now = std::chrono::steady_clock::now();
        float delta_time = std::chrono::duration<float>(now - last_frame_time_).count();
        last_frame_time_ = now;
        
        // Poll events
        window_->pollEvents();
        
        // Update
        handleInput();
        update(delta_time);
        
        // Render
        render();
    }
}

void Application::update(float delta_time) {
    // Update game logic
    
    // Update audio
    if (audio_) {
        audio_->update();
    }
}

void Application::render() {
    // Begin frame
    renderer_->beginFrame();
    
    // Clear
    renderer_->clear(0x2a2a2aff);  // Dark gray
    
    // Render game objects
    // ...
    
    // UI
    ui_->newFrame();
    
    // Main menu bar
    if (ui_->beginMainMenuBar()) {
        if (ui_->beginMenu("File")) {
            if (ui_->menuItem("Exit")) {
                running_ = false;
            }
            ui_->endMenu();
        }
        ui_->endMainMenuBar();
    }
    
    // Debug window
    ui_->beginWindow("Debug");
    ui_->text("FPS: %.1f", ui_->getFramerate());
    ui_->endWindow();
    
    ui_->render();
    
    // End frame
    renderer_->endFrame();
}

void Application::handleInput() {
    // Handle keyboard through window interface
    if (window_->isKeyPressed(oxide::input::Key::Escape)) {
        running_ = false;
    }
    
    // Handle gamepad through input system
    // ...
}

void Application::shutdown() {
    // Clean shutdown in reverse order
    audio_.reset();
    ui_.reset();
    renderer_.reset();
    window_.reset();
}

} // namespace your_app
```

### Step 5: Create Main Entry Point

`src/main.cpp`:

```cpp
#include "application.h"
#include <oxide/core/logging/logger.h>

#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Create logger
    auto logger = oxide::core::makeConsoleLogger();
    
    // Parse command line arguments
    your_app::Application::Config config;
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
    
    // Create and run application
    logger->log(oxide::core::LogLevel::Info, "Starting application...");
    
    auto app_result = your_app::Application::Create(config);
    if (!app_result) {
        logger->log(oxide::core::LogLevel::Error,
            "Failed to create application: " + 
            std::string(app_result.error().what())
        );
        return 1;
    }
    
    try {
        app_result.value()->run();
    } catch (const std::exception& e) {
        logger->log(oxide::core::LogLevel::Critical,
            "Unhandled exception: " + std::string(e.what())
        );
        return 2;
    }
    
    logger->log(oxide::core::LogLevel::Info, "Application terminated normally");
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

## Application Types

### Game Client

Full game with rendering, audio, networking:

```cpp
class GameClient : public Application {
    // Game-specific systems
    std::unique_ptr<GameWorld> world_;
    std::unique_ptr<NetworkClient> network_;
    std::unique_ptr<InputManager> input_;
    
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
class GameServer {
    std::unique_ptr<NetworkServer> network_;
    std::unique_ptr<GameSimulation> simulation_;
    
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
class LevelEditor : public Application {
    void renderUI() {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) newLevel();
                if (ImGui::MenuItem("Open...")) openLevel();
                if (ImGui::MenuItem("Save")) saveLevel();
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        
        // Tool windows
        renderSceneHierarchy();
        renderPropertyEditor();
        renderViewport();
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
    std::expected<void, Error> loadFromDirectory(std::string_view path) {
        namespace fs = std::filesystem;
        
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".toml") {
                auto result = loadConfig(entry.path());
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

auto shader = renderer_->createShader(
    embedded::default_shader,
    embedded::default_shader_size
);
```

## Error Handling Patterns

### Graceful Degradation

```cpp
// Try to load user config, fall back to defaults
auto config_result = loadConfig("user.toml");
if (!config_result) {
    logger->warn("Using default config: " + config_result.error().what());
    config = getDefaultConfig();
} else {
    config = config_result.value();
}
```

### Fatal Errors

```cpp
auto critical_resource = loadCriticalResource();
if (!critical_resource) {
    // Log and show user-friendly error
    logger->error("Failed to load critical resource");
    
    showErrorDialog(
        "Startup Error",
        "Required files are missing. Please reinstall."
    );
    
    return EXIT_FAILURE;
}
```

## Performance Tips

### Frame Timing

```cpp
class FrameTimer {
    static constexpr auto TARGET_FRAME_TIME = 16.667ms;  // 60 FPS
    
    void regulateFrameRate() {
        auto frame_time = clock::now() - frame_start_;
        
        if (frame_time < TARGET_FRAME_TIME) {
            std::this_thread::sleep_for(
                TARGET_FRAME_TIME - frame_time
            );
        }
    }
};
```

### Resource Pooling

```cpp
class ParticleSystem {
    // Pre-allocate particles
    std::vector<Particle> particle_pool_;
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

## Testing Applications

### Integration Tests

```cpp
TEST_CASE("Application lifecycle", "[app]") {
    Application::Config config;
    config.headless = true;  // No window
    
    auto app = Application::Create(config);
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
    auto app = createTestApp();
    
    // Simulate input
    app->injectMouseClick(100, 200);
    app->injectKeyPress(GLFW_KEY_SPACE);
    
    // Check state
    REQUIRE(app->isMenuOpen());
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

### Build Configurations

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

## Example Applications

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