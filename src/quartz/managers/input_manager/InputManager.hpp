#pragma once

#include <map>
#include <memory>

#include <GLFW/glfw3.h>

#include "quartz/managers/Loggers.hpp"

namespace quartz {
namespace managers {
    class InputManager;
}
}

class quartz::managers::InputManager {
public: // member functions
    InputManager(const InputManager& other) = delete;
    void operator=(const InputManager& other) = delete;

    InputManager(InputManager&& other) = default;

    USE_LOGGER(INPUTMAN);

    void collectInput();

    bool getKeyDown_q() const { return m_keyDown_q; }
    bool getKeyImpact_q() const { return m_keyImpact_q; }
    bool getKeyDown_esc() const { return m_keyDown_esc; }
    bool getKeyImpact_esc() const { return m_keyImpact_esc; }

    bool getKeyDown_w() const { return m_keyDown_w; }
    bool getKeyDown_a() const { return m_keyDown_a; }
    bool getKeyDown_s() const { return m_keyDown_s; }
    bool getKeyDown_d() const { return m_keyDown_d; }
    bool getKeyDown_space() const { return m_keyDown_space; }
    bool getKeyDown_shift() const { return m_keyDown_shift; }

    float getMousePosition_x() const { return m_mousePosition_x; }
    float getMousePosition_y() const { return m_mousePosition_y; }
    float getMousePositionOffset_x() const { return m_mousePositionOffset_x; }
    float getMousePositionOffset_y() const { return m_mousePositionOffset_y; }

    float getScrollOffset_x() const { return m_scrollOffset_x; }
    float getScrollOffset_y() const { return m_scrollOffset_y; }

    void setShouldCollectMouseInput(const bool shouldCollect);
    void setShouldCollectKeyInput(const bool shouldCollect);

private: // member functions
    InputManager(const std::shared_ptr<GLFWwindow>& p_glfwWindow);

public: // static functions
    static std::shared_ptr<InputManager> getPtr(const std::shared_ptr<GLFWwindow>& p_glfwWindow);

    static void mousePositionInputCallback(
        GLFWwindow* p_glfwWindow,
        double updatedMousePosition_x,
        double updatedMousePosition_y
    );
    static void scrollInputCallback(
        GLFWwindow* p_glfwWindow,
        double scrollOffset_x,
        double scrollOffset_y
    );

private: // static functions

private: // static variables
    static std::map<
        const GLFWwindow* const,
        std::shared_ptr<quartz::managers::InputManager>
    > inputManagerPtrMap;

private: // member variables
    std::shared_ptr<GLFWwindow> mp_glfwWindow;

    bool m_shouldCollectMouseInput;
    bool m_mousePositionInitialized;

    bool m_shouldCollectKeyInput;

    bool m_keyDown_q;
    bool m_keyImpact_q;
    bool m_keyDown_esc;
    bool m_keyImpact_esc;

    bool m_keyDown_w;
    bool m_keyDown_a;
    bool m_keyDown_s;
    bool m_keyDown_d;
    bool m_keyDown_space;
    bool m_keyDown_shift;

    float m_mousePosition_x;
    float m_mousePosition_y;
    float m_mousePositionOffset_x;
    float m_mousePositionOffset_y;

    float m_scrollOffset_x;
    float m_scrollOffset_y;
};