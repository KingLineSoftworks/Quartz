#include <GLFW/glfw3.h>

#include "quartz/managers/input_manager/InputManager.hpp"

std::map<
    const GLFWwindow* const,
    std::shared_ptr<quartz::managers::InputManager>
> quartz::managers::InputManager::inputManagerPtrMap;

quartz::managers::InputManager::InputManager(
    const std::shared_ptr<GLFWwindow>& p_glfwWindow
) :
    mp_glfwWindow(p_glfwWindow),
    m_shouldCollectMouseInput(true),
    m_keyDown_q(false),
    m_keyDown_esc(false),
    m_keyPressed_esc(false),
    m_keyDown_w(false),
    m_keyDown_a(false),
    m_keyDown_s(false),
    m_keyDown_d(false),
    m_keyDown_space(false),
    m_keyDown_shift(false),
    m_mouseUpdated(false),
    m_mousePosition_x(0.0f),
    m_mousePosition_y(0.0f),
    m_mousePositionOffset_x(0.0f),
    m_mousePositionOffset_y(0.0f),
    m_scrollOffset_x(0.0f),
    m_scrollOffset_y(0.0f)
{
    LOG_FUNCTION_CALL_TRACEthis(
        "GLFW Window pointer {}", static_cast<const void*>(p_glfwWindow.get())
    );
}

void
quartz::managers::InputManager::collectInput() {
    m_mousePositionOffset_x = 0.0f;
    m_mousePositionOffset_y = 0.0f;

    m_scrollOffset_x = 0.0f;
    m_scrollOffset_y = 0.0f;

    glfwPollEvents();

    m_keyDown_q = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_Q);

    bool keyDown_esc = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_ESCAPE);
    m_keyPressed_esc = keyDown_esc && !m_keyDown_esc;
    m_keyDown_esc = keyDown_esc;

    m_keyDown_w = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_W);
    m_keyDown_a = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_A);
    m_keyDown_s = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_S);
    m_keyDown_d = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_D);
    m_keyDown_space = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_SPACE);
    m_keyDown_shift = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_LEFT_SHIFT);
}

void
quartz::managers::InputManager::setShouldCollectMouseInput(const bool shouldCollect) {
    m_shouldCollectMouseInput = shouldCollect;

    LOG_DEBUGthis("{} mouse input", (m_shouldCollectMouseInput ? "Enabling" : "Disabling"));
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

    glfwSetInputMode(
        p_glfwWindow.get(),
        GLFW_CURSOR,
        GLFW_CURSOR_DISABLED
    );

    glfwSetCursorPosCallback(
        p_glfwWindow.get(),
        quartz::managers::InputManager::mousePositionInputCallback
    );

    glfwSetScrollCallback(
        p_glfwWindow.get(),
        quartz::managers::InputManager::scrollInputCallback
    );

    return quartz::managers::InputManager::inputManagerPtrMap[p_glfwWindow.get()];
}

void
quartz::managers::InputManager::mousePositionInputCallback(
    GLFWwindow* p_glfwWindow,
    double updatedMousePosition_x,
    double updatedMousePosition_y
) {
    std::shared_ptr<quartz::managers::InputManager> p_inputManager =
        quartz::managers::InputManager::inputManagerPtrMap[p_glfwWindow];

    if (!p_inputManager->m_shouldCollectMouseInput) {
        return;
    }

    if (p_inputManager->m_mouseUpdated) {
        p_inputManager->m_mousePositionOffset_x =
            p_inputManager->m_mousePosition_x - updatedMousePosition_x;
        p_inputManager->m_mousePositionOffset_y =
            p_inputManager->m_mousePosition_y - updatedMousePosition_y;
    }

    p_inputManager->m_mousePosition_x = updatedMousePosition_x;
    p_inputManager->m_mousePosition_y = updatedMousePosition_y;
    p_inputManager->m_mouseUpdated = true;
}

void
quartz::managers::InputManager::scrollInputCallback(
    GLFWwindow* p_glfwWindow,
    double scrollOffset_x,
    double scrollOffset_y
) {
    std::shared_ptr<quartz::managers::InputManager> p_inputManager =
        quartz::managers::InputManager::inputManagerPtrMap[p_glfwWindow];

    if (!p_inputManager->m_shouldCollectMouseInput) {
        return;
    }

    p_inputManager->m_scrollOffset_x = scrollOffset_x;
    p_inputManager->m_scrollOffset_y = scrollOffset_y;
}