#include <GLFW/glfw3.h>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/window/Window.hpp"

std::shared_ptr<GLFWwindow> quartz::rendering::Window::createGLFWwindowPtr(
    const std::string &name,
    const uint32_t widthPixels,
    const uint32_t heightPixels
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::WINDOW, "{} ({}x{})", name, widthPixels, heightPixels);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef ON_MAC
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    std::shared_ptr<GLFWwindow> p_glfwWindow(
        glfwCreateWindow(
            widthPixels,
            heightPixels,
            name.c_str(),
            nullptr,
            nullptr
        ),
        [](GLFWwindow* p_window) { glfwDestroyWindow(p_window); }
    );
    LOG_TRACE(quartz::loggers::WINDOW, "Created GLFW window pointer at {}", static_cast<void*>(p_glfwWindow.get()));
    if (!p_glfwWindow) {
        LOG_CRITICAL(quartz::loggers::WINDOW, "Failed to create GLFW window pointer. Terminating");
        glfwTerminate();
        throw std::runtime_error("");
    }

    return p_glfwWindow;
}

quartz::rendering::Window::Window(
    const std::string& name,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels
) :
    m_name(name),
    m_widthPixels(windowWidthPixels),
    m_heightPixels(windowHeightPixels),
    mp_glfwWindow(quartz::rendering::Window::createGLFWwindowPtr(name, windowWidthPixels, windowHeightPixels))
{
    LOG_FUNCTION_CALL_TRACEthis("{}", m_name, m_widthPixels, m_heightPixels);
}

quartz::rendering::Window::~Window() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis("Destroying GLFW window at {}", static_cast<void*>(mp_glfwWindow.get()));
    glfwDestroyWindow(mp_glfwWindow.get());

    LOG_TRACEthis("Terminating GLFW");
    glfwTerminate();
}

bool quartz::rendering::Window::shouldClose() const {
    return static_cast<bool>(
        glfwWindowShouldClose(mp_glfwWindow.get())
    );
}