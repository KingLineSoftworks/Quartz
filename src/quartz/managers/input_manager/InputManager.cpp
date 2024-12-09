#include <GLFW/glfw3.h>

#include "quartz/managers/input_manager/InputManager.hpp"

std::map<const GLFWwindow* const, quartz::managers::InputManager> quartz::managers::InputManager::inputManagerMap;

quartz::managers::InputManager::InputManager(
    const std::shared_ptr<GLFWwindow>& p_glfwWindow
) :
    mp_glfwWindow(p_glfwWindow),

    m_shouldCollectMouseInput(true),
    m_mousePositionInitialized(false),

    m_shouldCollectKeyInput(true),

    m_keyDown_q(false),
    m_keyImpact_q(false),
    m_keyDown_esc(false),
    m_keyImpact_esc(false),

    m_keyDown_w(false),
    m_keyDown_a(false),
    m_keyDown_s(false),
    m_keyDown_d(false),
    m_keyDown_space(false),
    m_keyDown_shift(false),

    m_mousePosition_x(0.0f),
    m_mousePosition_y(0.0f),
    m_mousePositionOffset_x(0.0f),
    m_mousePositionOffset_y(0.0f),

    m_scrollOffset_x(0.0f),
    m_scrollOffset_y(0.0f)
{
    LOG_FUNCTION_CALL_TRACEthis("GLFW Window pointer {}", static_cast<const void*>(p_glfwWindow.get()));
}

quartz::managers::InputManager::InputManager(
    quartz::managers::InputManager&& other
) :
    mp_glfwWindow(std::move(other.mp_glfwWindow)),

    m_shouldCollectMouseInput(other.m_shouldCollectMouseInput),
    m_mousePositionInitialized(other.m_mousePositionInitialized),

    m_shouldCollectKeyInput(other.m_shouldCollectKeyInput),

    m_keyDown_q(other.m_keyDown_q),
    m_keyImpact_q(other.m_keyImpact_q),
    m_keyDown_esc(other.m_keyDown_esc),
    m_keyImpact_esc(other.m_keyImpact_esc),

    m_keyDown_w(other.m_keyDown_w),
    m_keyDown_a(other.m_keyDown_a),
    m_keyDown_s(other.m_keyDown_s),
    m_keyDown_d(other.m_keyDown_d),
    m_keyDown_space(other.m_keyDown_space),
    m_keyDown_shift(other.m_keyDown_shift),

    m_mousePosition_x(other.m_mousePosition_x),
    m_mousePosition_y(other.m_mousePosition_y),
    m_mousePositionOffset_x(other.m_mousePositionOffset_x),
    m_mousePositionOffset_y(other.m_mousePositionOffset_y),

    m_scrollOffset_x(other.m_scrollOffset_x),
    m_scrollOffset_y(other.m_scrollOffset_y)
{}

void
quartz::managers::InputManager::collectInput() {
    m_mousePositionOffset_x = 0.0f;
    m_mousePositionOffset_y = 0.0f;

    m_scrollOffset_x = 0.0f;
    m_scrollOffset_y = 0.0f;

    glfwPollEvents();

    bool keyDown_q = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_Q);
    m_keyDown_q = keyDown_q && !m_keyDown_q;
    m_keyDown_q = keyDown_q;
    bool keyDown_esc = glfwGetKey(mp_glfwWindow.get(), GLFW_KEY_ESCAPE);
    m_keyImpact_esc = keyDown_esc && !m_keyDown_esc;
    m_keyDown_esc = keyDown_esc;

    if (!m_shouldCollectKeyInput) {
        return;
    }

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
    m_mousePositionInitialized = !shouldCollect;

    LOG_DEBUGthis("{} mouse input", (m_shouldCollectMouseInput ? "Enabling" : "Disabling"));
}

void
quartz::managers::InputManager::setShouldCollectKeyInput(const bool shouldCollect) {
    m_shouldCollectKeyInput = shouldCollect;

    LOG_DEBUGthis("{} mouse input", (m_shouldCollectKeyInput ? "Enabling" : "Disabling"));
}

quartz::managers::InputManager&
quartz::managers::InputManager::getInstance(
    const std::shared_ptr<GLFWwindow>& p_glfwWindow
) {
    static std::map<const GLFWwindow* const, quartz::managers::InputManager> inputManagerMap;

    LOG_FUNCTION_SCOPE_TRACE(INPUTMAN, "GLFW Window pointer {}", static_cast<const void*>(p_glfwWindow.get()));

    if (quartz::managers::InputManager::inputManagerMap.count(p_glfwWindow.get()) > 0) {
        LOG_TRACE(INPUTMAN, "Input manager already exists for GLFW Window pointer at {}", static_cast<const void*>(p_glfwWindow.get()));
        return quartz::managers::InputManager::inputManagerMap.at(p_glfwWindow.get());
    }

    LOG_TRACE(INPUTMAN, "Creating input manager for GLFW Window pointer at {}", static_cast<const void*>(p_glfwWindow.get()));

    quartz::managers::InputManager::inputManagerMap.insert({
        p_glfwWindow.get(),
        quartz::managers::InputManager(p_glfwWindow)
    });

    // We want to allow a nullptr p_glfwWindow so we can construct a "dummy" input manager for the sake
    // of testing without having a window or rendering capabilities
    if (!p_glfwWindow) {
        LOG_INFO(INPUTMAN, "Getting dummy input manager with GLFW Window pointer at {}", static_cast<const void*>(p_glfwWindow.get()));
        return quartz::managers::InputManager::inputManagerMap.at(p_glfwWindow.get());
    }

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

    return quartz::managers::InputManager::inputManagerMap.at(p_glfwWindow.get());
}

void
quartz::managers::InputManager::mousePositionInputCallback(
    GLFWwindow* p_glfwWindow,
    double updatedMousePosition_x,
    double updatedMousePosition_y
) {
    quartz::managers::InputManager& inputManager = quartz::managers::InputManager::inputManagerMap.at(p_glfwWindow);

    if (!inputManager.m_shouldCollectMouseInput) {
        return;
    }

    if (inputManager.m_mousePositionInitialized) {
        inputManager.m_mousePositionOffset_x = inputManager.m_mousePosition_x - updatedMousePosition_x;
        inputManager.m_mousePositionOffset_y = inputManager.m_mousePosition_y - updatedMousePosition_y;
    }

    inputManager.m_mousePosition_x = updatedMousePosition_x;
    inputManager.m_mousePosition_y = updatedMousePosition_y;
    inputManager.m_mousePositionInitialized = true;
}

void
quartz::managers::InputManager::scrollInputCallback(
    GLFWwindow* p_glfwWindow,
    double scrollOffset_x,
    double scrollOffset_y
) {
    quartz::managers::InputManager& inputManager = quartz::managers::InputManager::inputManagerMap.at(p_glfwWindow);

    if (!inputManager.m_shouldCollectMouseInput) {
        return;
    }

    inputManager.m_scrollOffset_x = scrollOffset_x;
    inputManager.m_scrollOffset_y = scrollOffset_y;
}
