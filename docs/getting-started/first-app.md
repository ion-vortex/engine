# Your First Oxide Application

This tutorial will guide you through creating a simple application using the Oxide framework. We'll build a basic viewer that displays a spinning cube.

## Prerequisites

Before starting, ensure you have:
- ✅ Completed [Prerequisites](prerequisites.md) installation
- ✅ Successfully [built Oxide](building.md)
- ✅ Can run the example client: `./build/debug/apps/client/client`

## Overview

We'll create an app that:
1. Initializes a window using GLFW
2. Sets up bgfx for rendering
3. Displays a simple 3D cube
4. Adds basic ImGui interface
5. Handles input and updates

## Project Structure

Create a new app in the `apps/` directory:

```
apps/
└── cube_viewer/
    ├── CMakeLists.txt
    ├── src/
    │   └── main.cpp
    └── README.md
```

## Step 1: Create CMakeLists.txt

Create `apps/cube_viewer/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.28)

# Support standalone build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(CubeViewer VERSION 0.1.0 LANGUAGES CXX)
    
    # Add path to Oxide helpers
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
    include(OxideHelpers)
    oxide_setup_build_interface()
    
    # Build required libraries
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../libs libs)
endif()

# Create the application
oxide_add_application(
    NAME cube_viewer
    DEPENDENCIES
        oxide::core
        oxide::render
        oxide::ui
)

# Platform-specific settings
if(WIN32)
    set_target_properties(cube_viewer PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(cube_viewer PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()
```

## Step 2: Basic Application Structure

Create `apps/cube_viewer/src/main.cpp`:

```cpp
#include <oxide/core/error/error.h>
#include <oxide/core/logging/logger.h>
#include <oxide/render/backend/render_backend.h>
#include <oxide/render/view/render_view.h>
#include <oxide/ui/imgui_context.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <memory>
#include <iostream>

using namespace oxide;

class CubeViewerApp {
public:
    [[nodiscard]]
    static std::expected<std::unique_ptr<CubeViewerApp>, core::Error> Create() {
        auto app = std::unique_ptr<CubeViewerApp>(new CubeViewerApp());
        
        // Initialize GLFW
        if (!glfwInit()) {
            return std::unexpected(core::Error(
                core::ErrorCode::InitializationFailed, 
                "Failed to initialize GLFW"
            ));
        }
        
        // Create window
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        app->window_ = glfwCreateWindow(1280, 720, "Oxide Cube Viewer", nullptr, nullptr);
        if (!app->window_) {
            glfwTerminate();
            return std::unexpected(core::Error(
                core::ErrorCode::ResourceUnavailable,
                "Failed to create window"
            ));
        }
        
        // Initialize renderer
        auto render_result = render::IRenderBackend::Create(app->window_);
        if (!render_result) {
            return std::unexpected(render_result.error());
        }
        app->renderer_ = std::move(render_result.value());
        
        // Initialize UI
        auto ui_result = ui::IImGuiContext::Create(app->window_, app->renderer_.get());
        if (!ui_result) {
            return std::unexpected(ui_result.error());
        }
        app->ui_ = std::move(ui_result.value());
        
        // Setup cube geometry
        if (auto result = app->setupCube(); !result) {
            return std::unexpected(result.error());
        }
        
        return app;
    }
    
    void run() {
        auto start_time = std::chrono::steady_clock::now();
        
        while (!glfwWindowShouldClose(window_)) {
            glfwPollEvents();
            
            // Calculate time
            auto current_time = std::chrono::steady_clock::now();
            float time = std::chrono::duration<float>(current_time - start_time).count();
            
            // Update
            update(time);
            
            // Render
            render();
        }
    }
    
    ~CubeViewerApp() {
        if (window_) {
            glfwDestroyWindow(window_);
        }
        glfwTerminate();
    }

private:
    CubeViewerApp() = default;
    
    [[nodiscard]]
    std::expected<void, core::Error> setupCube() {
        // Cube vertices (position + color)
        struct Vertex {
            float x, y, z;
            uint32_t color;
        };
        
        Vertex vertices[] = {
            // Front face (red)
            {-1.0f, -1.0f,  1.0f, 0xff0000ff},
            { 1.0f, -1.0f,  1.0f, 0xff0000ff},
            { 1.0f,  1.0f,  1.0f, 0xff0000ff},
            {-1.0f,  1.0f,  1.0f, 0xff0000ff},
            // Back face (green)
            {-1.0f, -1.0f, -1.0f, 0xff00ff00},
            { 1.0f, -1.0f, -1.0f, 0xff00ff00},
            { 1.0f,  1.0f, -1.0f, 0xff00ff00},
            {-1.0f,  1.0f, -1.0f, 0xff00ff00},
        };
        
        uint16_t indices[] = {
            0, 1, 2, 2, 3, 0, // Front
            4, 6, 5, 6, 4, 7, // Back
            0, 3, 7, 7, 4, 0, // Left
            1, 5, 6, 6, 2, 1, // Right
            3, 2, 6, 6, 7, 3, // Top
            0, 4, 5, 5, 1, 0  // Bottom
        };
        
        // Create vertex buffer
        auto vb_result = renderer_->createVertexBuffer(vertices, sizeof(vertices));
        if (!vb_result) return std::unexpected(vb_result.error());
        vertex_buffer_ = std::move(vb_result.value());
        
        // Create index buffer
        auto ib_result = renderer_->createIndexBuffer(indices, sizeof(indices));
        if (!ib_result) return std::unexpected(ib_result.error());
        index_buffer_ = std::move(ib_result.value());
        
        return {};
    }
    
    void update(float time) {
        rotation_ = time * 0.5f;
    }
    
    void render() {
        // Begin frame
        renderer_->beginFrame();
        
        // Clear
        renderer_->clear(0x303030ff);
        
        // Setup matrices
        int width, height;
        glfwGetFramebufferSize(window_, &width, &height);
        
        glm::mat4 projection = glm::perspective(
            glm::radians(60.0f),
            float(width) / float(height),
            0.1f, 100.0f
        );
        
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, -5.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );
        
        glm::mat4 model = glm::rotate(
            glm::mat4(1.0f),
            rotation_,
            glm::vec3(1.0f, 1.0f, 0.0f)
        );
        
        glm::mat4 mvp = projection * view * model;
        
        // Draw cube
        renderer_->setTransform(mvp);
        renderer_->setVertexBuffer(vertex_buffer_.get());
        renderer_->setIndexBuffer(index_buffer_.get());
        renderer_->submit();
        
        // UI
        ui_->newFrame();
        
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Cube Viewer");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::Text("Rotation: %.2f", rotation_);
        ImGui::Separator();
        if (ImGui::Button("Reset Rotation")) {
            rotation_ = 0.0f;
        }
        ImGui::End();
        
        ui_->render();
        
        // End frame
        renderer_->endFrame();
    }
    
private:
    GLFWwindow* window_ = nullptr;
    std::unique_ptr<render::IRenderBackend> renderer_;
    std::unique_ptr<ui::IImGuiContext> ui_;
    std::unique_ptr<render::IVertexBuffer> vertex_buffer_;
    std::unique_ptr<render::IIndexBuffer> index_buffer_;
    float rotation_ = 0.0f;
};

int main(int argc, char* argv[]) {
    // Create logger
    auto logger = core::makeConsoleLogger();
    
    // Create and run app
    auto app_result = CubeViewerApp::Create();
    if (!app_result) {
        logger->log(core::LogLevel::Error, 
            "Failed to create app: " + std::string(app_result.error().what()));
        return 1;
    }
    
    logger->log(core::LogLevel::Info, "Starting Cube Viewer...");
    app_result.value()->run();
    
    return 0;
}
```

## Step 3: Add to Main Build

Edit `apps/CMakeLists.txt` to include your new app:

```cmake
add_subdirectory(client)
add_subdirectory(cube_viewer)  # Add this line
add_subdirectory(unid)
add_subdirectory(zoned)
```

## Step 4: Build and Run

```bash
# From the oxide root directory
cmake --preset debug-linux    # or debug-windows, debug-macos
cmake --build --preset debug-linux --target cube_viewer

# Run it
./build/debug/Linux/apps/cube_viewer/cube_viewer
```

## Understanding the Code

### Factory Pattern
Following Oxide conventions, we use a static `Create()` factory method that returns `std::expected<std::unique_ptr<T>, Error>`.

### Error Handling
All fallible operations return `std::expected`. We check results and propagate errors up the call stack.

### Interface Usage
We use Oxide's interface types (`IRenderBackend`, `IImGuiContext`) rather than concrete implementations.

### Resource Management
All resources are managed via `std::unique_ptr`. No manual memory management.

## Next Steps

Now that you have a working application, try:

1. **Add More Geometry**: Create different shapes
2. **Load Models**: Use `oxide::asset` to load 3D models
3. **Add Physics**: Integrate `oxide::physics` for collision
4. **Network It**: Use `oxide::protocol` for multiplayer

## Common Issues

### "Cannot find oxide headers"
Ensure you're building from the oxide root or have properly set up the standalone build.

### "Undefined reference to..."
Make sure all required libraries are listed in `DEPENDENCIES`.

### "Window doesn't appear"
Check that your graphics drivers are up to date and support OpenGL/Vulkan.

## Going Further

### Adding Audio
```cpp
#include <oxide/audio/audio_engine.h>

auto audio_result = audio::IAudioEngine::Create();
if (audio_result) {
    audio_ = std::move(audio_result.value());
    audio_->playSound("assets/audio/startup.wav");
}
```

### Loading Assets
```cpp
#include <oxide/asset/asset_loader.h>

auto loader_result = asset::IAssetLoader::Create();
if (loader_result) {
    auto model = loader_result.value()->loadModel("assets/models/ship.gltf");
}
```

### Adding Networking
```cpp
#include <oxide/protocol/tcp_client.h>

auto client_result = protocol::ITcpClient::Create("localhost", 7777);
if (client_result) {
    client_->connect();
    client_->send(my_packet);
}
```

## Conclusion

Congratulations! You've created your first Oxide application. You've learned:
- How to structure an Oxide application
- How to use the factory pattern for object creation
- How to handle errors with `std::expected`
- How to integrate rendering and UI
- How to follow Oxide coding standards

Continue exploring the framework by:
- Reading the [Architecture](../architecture/) documentation
- Studying the [example client](../../apps/client/src/main.cpp)
- Building your own game or tool!