#include <GLFW/glfw3.h>

#include "quartz/managers/Loggers.hpp"
#include "quartz/managers/input_manager/InputManager.hpp"

std::map<const GLFWwindow* const, quartz::managers::InputManager> quartz::managers::InputManager::inputManagerMap;

quartz::managers::InputManager::InputManager(
    const std::shared_ptr<GLFWwindow>& p_glfwWindow
) :
    mp_glfwWindow(p_glfwWindow),

    m_shouldCollectMouseInput(true),
    m_mousePositionInitialized(false),

    m_shouldCollectKeyInput(true),

    m_a(false, false, false),
    m_d(false, false, false),
    m_l(false, false, false),
    m_p(false, false, false),
    m_q(false, false, false),
    m_s(false, false, false),
    m_w(false, false, false),

    m_esc(false, false, false),
    m_shift(false, false, false),
    m_ctrl(false, false, false),
    m_space(false, false, false),

    m_period(false, false, false),


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

    m_a(other.m_a),
    m_d(other.m_d),
    m_l(other.m_l),
    m_p(other.m_p),
    m_q(other.m_q),
    m_s(other.m_s),
    m_w(other.m_w),

    m_esc(other.m_esc),
    m_shift(other.m_shift),
    m_ctrl(other.m_ctrl),
    m_space(other.m_space),

    m_period(other.m_period),

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

    m_esc = getKeyPressInfo(m_w.down, GLFW_KEY_ESCAPE);
    m_q = getKeyPressInfo(m_q.down, GLFW_KEY_Q);

    if (!m_shouldCollectKeyInput) {
        return;
    }

    m_a = getKeyPressInfo(m_a.down, GLFW_KEY_A);
    m_d = getKeyPressInfo(m_d.down, GLFW_KEY_D);
    m_l = getKeyPressInfo(m_l.down, GLFW_KEY_L);
    m_p = getKeyPressInfo(m_p.down, GLFW_KEY_P);
    m_s = getKeyPressInfo(m_s.down, GLFW_KEY_S);
    m_w = getKeyPressInfo(m_w.down, GLFW_KEY_W);

    m_shift = getKeyPressInfo(m_shift.down, GLFW_KEY_LEFT_SHIFT);
    m_ctrl = getKeyPressInfo(m_ctrl.down, GLFW_KEY_LEFT_CONTROL);
    m_space = getKeyPressInfo(m_space.down, GLFW_KEY_SPACE);

    m_period = getKeyPressInfo(m_period.down, GLFW_KEY_PERIOD);
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

quartz::managers::InputManager::KeyPressInfo
quartz::managers::InputManager::getKeyPressInfo(
    const bool currentDown,
    const int glfwKey
) {
    const bool keyDown = glfwGetKey(mp_glfwWindow.get(), glfwKey);
    const bool keyImpacted = keyDown && !currentDown;
    const bool keyReleased = !keyDown && currentDown;

    return quartz::managers::InputManager::KeyPressInfo(keyDown, keyImpacted, keyReleased);
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
