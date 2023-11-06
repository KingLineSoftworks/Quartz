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
    m_keypressed_q(false),
    m_keypressed_esc(false),
    m_keypressed_w(false),
    m_keypressed_a(false),
    m_keypressed_s(false),
    m_keypressed_d(false),
    m_keypressed_space(false),
    m_keypressed_shift(false),
    m_cursorPosition_x(0.0f),
    m_cursorPosition_y(0.0f),
    m_cursorPositionOffset_x(0.0f),
    m_cursorPositionOffset_y(0.0f),
    m_scrollOffset_x(0.0f),
    m_scrollOffset_y(0.0f)
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

    m_cursorPositionOffset_x = 0.0f;
    m_cursorPositionOffset_y = 0.0f;

    m_scrollOffset_x = 0.0f;
    m_scrollOffset_y = 0.0f;
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
        GLFW_CURSOR_NORMAL
    );

    glfwSetCursorPosCallback(
        p_glfwWindow.get(),
        quartz::managers::InputManager::cursorPositionInputCallback
    );

    glfwSetScrollCallback(
        p_glfwWindow.get(),
        quartz::managers::InputManager::scrollInputCallback
    );

    return quartz::managers::InputManager::inputManagerPtrMap[p_glfwWindow.get()];
}

void
quartz::managers::InputManager::cursorPositionInputCallback(
    GLFWwindow* p_glfwWindow,
    double updatedCursorPosition_x,
    double updatedCursorPosition_y
) {
    std::shared_ptr<quartz::managers::InputManager> p_inputManager =
        quartz::managers::InputManager::inputManagerPtrMap[p_glfwWindow];

    if (!p_inputManager->m_shouldCollectMouseInput) {
        return;
    }

    p_inputManager->m_cursorPositionOffset_x =
        p_inputManager->m_cursorPosition_x - updatedCursorPosition_x;
    p_inputManager->m_cursorPositionOffset_y =
        p_inputManager->m_cursorPositionOffset_x - updatedCursorPosition_y;

    p_inputManager->m_cursorPosition_x = updatedCursorPosition_x;
    p_inputManager->m_cursorPosition_y = updatedCursorPosition_y;
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