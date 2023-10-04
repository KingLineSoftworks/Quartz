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

    const std::shared_ptr<GLFWwindow>& getGLFWwindowPtr() const { return mp_glfwWindow; }

    bool shouldClose() const;

    bool getWasResized() const { return m_wasResized; }
    void clearWasResized() { m_wasResized = false; }

public: // static functions
    // The callback we give to glfw to use when it resizes the window
    static void glfwFramebufferSizeCallback(
        GLFWwindow* p_glfwWindow,
        int updatedWindowWidthPixels,
        int updatedWindowHeightPixels
    );

private: // static functions
    static std::shared_ptr<GLFWwindow> createGLFWwindowPtr(
        const std::string& name,
        const uint32_t widthPixels,
        const uint32_t heightPixels,
        const void* p_windowUser
    );

private: // member variables
    const std::string m_name;

    uint32_t m_widthPixels;
    uint32_t m_heightPixels;
    bool m_wasResized;

    std::shared_ptr<GLFWwindow> mp_glfwWindow;

private: // friends
    friend void quartz::rendering::Window::glfwFramebufferSizeCallback(
        GLFWwindow* p_glfwWindow,
        int updatedWindowWidthPixels,
        int updatedWindowHeightPixels
    );
};

