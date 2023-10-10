#include <GLFW/glfw3.h>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/window2/Window2.hpp"

void quartz::rendering::Window2::glfwFramebufferSizeCallback(
    GLFWwindow* p_glfwWindow,
    int updatedWindowWidthPixels,
    int updatedWindowHeightPixels
) {
    // Get a pointer to the quartz::rendering::Window2 form the GLFWwindow* so we may update its values
    quartz::rendering::Window2* p_quartzWindow = reinterpret_cast<quartz::rendering::Window2*>(
        glfwGetWindowUserPointer(p_glfwWindow)
    );

    if (!p_quartzWindow) {
        LOG_CRITICAL(quartz::loggers::WINDOW, "Retrieved invalid window pointer from glfw window user pointer");
    }

    p_quartzWindow->m_widthPixels = updatedWindowWidthPixels;
    p_quartzWindow->m_heightPixels = updatedWindowHeightPixels;
    p_quartzWindow->m_wasResized = true;
}

std::shared_ptr<GLFWwindow> quartz::rendering::Window2::createGLFWwindowPtr(
    const std::string &name,
    const uint32_t widthPixels,
    const uint32_t heightPixels,
    const void* p_windowUser
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::WINDOW, "{} ({}x{}) , user = {}", name, widthPixels, heightPixels, p_windowUser);

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
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

    LOG_TRACE(quartz::loggers::WINDOW, "Setting GLFW window user pointer to this");
    glfwSetWindowUserPointer(p_glfwWindow.get(), const_cast<void*>(p_windowUser));

    LOG_TRACE(quartz::loggers::WINDOW, "Setting GLFW framebuffer size callback");
    glfwSetFramebufferSizeCallback(p_glfwWindow.get(), quartz::rendering::Window2::glfwFramebufferSizeCallback);

    return p_glfwWindow;
}

quartz::rendering::Window2::Window2(
    const std::string& name,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels
) :
    m_name(name),
    m_widthPixels(windowWidthPixels),
    m_heightPixels(windowHeightPixels),
    m_wasResized(false),
    mp_glfwWindow(quartz::rendering::Window2::createGLFWwindowPtr(
        name,
        windowWidthPixels,
        windowHeightPixels,
        this
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("{}", m_name, m_widthPixels, m_heightPixels);
}

quartz::rendering::Window2::~Window2() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis("Destroying GLFW window at {}", static_cast<void*>(mp_glfwWindow.get()));
    glfwDestroyWindow(mp_glfwWindow.get());

    LOG_TRACEthis("Terminating GLFW");
    glfwTerminate();
}

bool quartz::rendering::Window2::shouldClose() const {
    return static_cast<bool>(
        glfwWindowShouldClose(mp_glfwWindow.get())
    );
}