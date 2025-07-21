# Your First Ion Vortex 2D Triangle Application

This tutorial will guide you through creating a simple application using the Ion Vortex framework. We'll build a basic viewer that displays a rotating 2D triangle.

## Prerequisites

Before starting, ensure you have:

* ✅ Completed [Prerequisites](prerequisites.md) installation
* ✅ Successfully [built Ion Vortex](building.md)
* ✅ Can run the example client:

  ```bash
  ./build/debug/apps/client/client
  ```

## Overview

We'll create an app that:

1. Initializes a window
2. Sets up SDL3 for rendering
3. Displays a rotating 2D triangle
4. Adds a basic ImGui interface
5. Handles input and updates

## Project Structure

Create a new app in the `apps/` directory:

```
apps/
└── triangle_viewer/
    ├── CMakeLists.txt
    ├── src/
    │   └── main.cpp
    └── README.md
```

## Step 1: Create CMakeLists.txt

Create `apps/triangle_viewer/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.28)

# Support standalone build
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    project(TriangleViewer VERSION 0.1.0 LANGUAGES CXX)
    
    # Add Ion Vortex helpers
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake")
    include(IonHelpers)
    ion_setup_build_interface()
    
    # Build required libraries
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../libs libs)
endif()

# Create the application
ion_add_application(
    NAME triangle_viewer
    DEPENDENCIES
        ion::core
        ion::render
        ion::ui
)

# Platform-specific settings
if(WIN32)
    set_target_properties(triangle_viewer PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
elseif(APPLE)
    set_target_properties(triangle_viewer PROPERTIES
        MACOSX_BUNDLE TRUE
    )
endif()
```

## Step 2: Basic Application Structure

Create `apps/triangle_viewer/src/main.cpp`:

```cpp
#include <ion/core/error/error.h>
#include <ion/core/logging/logger.h>
#include <ion/render/backend/render_backend.h>
#include <ion/render/view/render_view.h>
#include <ion/ui/imgui_context.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>
#include <memory>
#include <iostream>
#include <vector>

using namespace ion;

class triangle_viewer_app {
public:
    static std::expected<std::unique_ptr<triangle_viewer_app>, std::error_code> create() {
        auto app = std::make_unique<triangle_viewer_app>();

        // 1) Create SDL window
        app->window_ = SDL_CreateWindow(
            "Ion Vortex Triangle Viewer",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            800, 600,
            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
        );
        if (!app->window_) {
            return std::unexpected(std::make_error_code(std::errc::not_enough_memory));
        }

        // 2) Initialize renderer
        auto render_res = render::create_render_backend(app->window_);
        if (!render_res)
            return std::unexpected(render_res.error());
        app->renderer_ = std::move(render_res.value());

        // 3) Initialize ImGui
        auto ui_res = ui::create_ui_system(app->window_, app->renderer_.get());
        if (!ui_res)
            return std::unexpected(ui_res.error());
        app->ui_ = std::move(ui_res.value());

        // 4) Setup triangle geometry
        if (auto err = app->setup_triangle())
            return std::unexpected(err);

        return app;
    }

    void run() {
        auto start = std::chrono::steady_clock::now();
        while (!window_should_close()) {
            SDL_PollEvent(nullptr);

            // Time since start
            auto now = std::chrono::steady_clock::now();
            float t = std::chrono::duration<float>(now - start).count();

            update(t);
            render();
        }
    }

    ~triangle_viewer_app() {
        if (window_) SDL_DestroyWindow(window_);
    }

private:
    triangle_viewer_app() = default;

    std::expected<void, std::error_code> setup_triangle() {
        // Triangle vertices: position (x,y) + color (RGBA8)
        struct Vertex { float x, y; uint32_t color; };
        Vertex verts[3] = {
            {-0.5f, -0.5f, 0xff0000ff},  // bottom-left, red
            { 0.5f, -0.5f, 0xff00ff00},  // bottom-right, green
            { 0.0f,  0.5f, 0xffff0000}   // top, blue
        };

        // Create vertex buffer
        auto vb_res = renderer_->createVertexBuffer(verts, sizeof(verts));
        if (!vb_res) return std::unexpected(vb_res.error());
        vertex_buffer_ = std::move(vb_res.value());

        return {};
    }

    void update(float time) {
        rotation_ = time;  // rotate 1 radian per second
    }

    void render() {
        if (auto& frame = renderer_->frame(); frame.has_result()) {
            renderer_->clear(0x202020ff);

            // Build transform
            int w, h;
            SDL_GetWindowSize(window_, &w, &h);
            float aspect = float(w)/float(h);

            glm::mat4 proj = glm::ortho(-aspect, aspect, -1.0f, 1.0f);
            glm::mat4 model = glm::rotate(
                glm::mat4(1.0f),
                rotation_,
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
            glm::mat4 mvp = proj * model;

            renderer_->set_transform(mvp);
            renderer_->set_vertex_buffer(vertex_buffer_.get());
            renderer_->submit();

            // HUD
            if (auto& hud = ui_->frame("Triangle Viewer"); hud.has_value()) {
                // We'd be using the UI system, but you get the idea.
                ui_->text("FPS: %.1f", ImGui::GetIO().Framerate);
                ui_->text("Rotation: %.2f rad", rotation_);
                if (ui_->button("Reset")) {
                    rotation_ = 0.0f;
                }
            }
            ui_->render();
        }
    }

    bool window_should_close() {
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) return true;
        }
        return false;
    }

    SDL_Window* window_ = nullptr;
    std::unique_ptr<render::render_backend_base> renderer_;
    std::unique_ptr<ui::ui_system_base> ui_;
    std::unique_ptr<render::vertex_buffer> vertex_buffer_;
    float rotation_ = 0.0f;
};

int main() {
    auto logger = core::make_console_logger();

    auto app_res = triangle_viewer_app::create();
    if (!app_res) {
        logger->log(core::LogLevel::error,
            "Failed to create app: " + std::string(app_res.error().what()));
        return 1;
    }
    logger->log(core::LogLevel::info, "Starting Triangle Viewer...");
    app_res.value()->run();
    return 0;
}
```

## Step 3: Add to Main Build

Edit `apps/CMakeLists.txt` and add:

```cmake
add_subdirectory(triangle_viewer)
```

to the list of subdirectories.

## Step 4: Build and Run

```bash
# From the ion root
cmake --preset debug-linux
cmake --build --preset debug-linux --target triangle_viewer

# Run it
./build/debug/Linux/apps/triangle_viewer/triangle_viewer
```
