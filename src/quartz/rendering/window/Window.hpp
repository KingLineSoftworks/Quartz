#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include "quartz/rendering/Loggers.hpp"

namespace quartz {
namespace rendering {
    class Window;
}
}

class quartz::rendering::Window {
public: // interface
    Window(
        const std::string& name,
        const uint32_t widthPixels,
        const uint32_t heightPixels
    );
    ~Window();

    USE_LOGGER(WINDOW);

    bool shouldClose() const;

private: // static functions
    static std::shared_ptr<GLFWwindow> initGLFWandCreateGLFWWindow(
        const std::string& name,
        const uint32_t widthPixels,
        const uint32_t heightPixels
    );

private: // member variables
    const std::string m_name;

    uint32_t m_widthPixels;
    uint32_t m_heightPixels;

    std::shared_ptr<GLFWwindow> mp_glfwWindow;
};

