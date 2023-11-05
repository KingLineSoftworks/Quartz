#include <GLFW/glfw3.h>

#include "quartz/managers/input_manager/InputManager.hpp"

std::map<
    const GLFWwindow* const,
    std::shared_ptr<quartz::managers::InputManager>
> quartz::managers::InputManager::inputManagerPtrMap;

quartz::managers::InputManager::InputManager(
    const std::shared_ptr<GLFWwindow>& p_glfwWindow
) :
    mp_glfwWindow(p_glfwWindow)
{
    LOG_FUNCTION_CALL_TRACEthis(
        "GLFW Window pointer {}", static_cast<const void*>(p_glfwWindow.get())
    );
}

void
quartz::managers::InputManager::collectInput() {
    glfwPollEvents();

    m_keypressed_q = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_Q);
    m_keypressed_esc = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_ESCAPE);

    m_keypressed_w = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_W);
    m_keypressed_a = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_S);
    m_keypressed_s = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_A);
    m_keypressed_d = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_D);
    m_keypressed_space = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_SPACE);
    m_keypressed_shift = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_LEFT_SHIFT);
}

std::shared_ptr<quartz::managers::InputManager>
quartz::managers::InputManager::getPtr(
    const std::shared_ptr<GLFWwindow>& p_glfwWindow
) {
    LOG_FUNCTION_SCOPE_TRACE(
        INPUTMAN, "GLFW Window pointer {}",
        static_cast<const void*>(p_glfwWindow.get())
    );

    if (quartz::managers::InputManager::inputManagerPtrMap.count(p_glfwWindow.get()) > 0) {
        LOG_TRACE(
            INPUTMAN, "Input manager already exists for GLFW Window pointer at {}",
            static_cast<const void*>(p_glfwWindow.get())
        );

        return quartz::managers::InputManager::inputManagerPtrMap[p_glfwWindow.get()];
    }

    LOG_TRACE(
        INPUTMAN, "Creating input manager for GLFW Window pointer at {}",
        static_cast<const void*>(p_glfwWindow.get())
    );

    quartz::managers::InputManager inputManager(p_glfwWindow);
    quartz::managers::InputManager::inputManagerPtrMap.insert({
        p_glfwWindow.get(),
        std::make_shared<quartz::managers::InputManager>(std::move(inputManager))
    });

    return quartz::managers::InputManager::inputManagerPtrMap[p_glfwWindow.get()];

    return nullptr;
}