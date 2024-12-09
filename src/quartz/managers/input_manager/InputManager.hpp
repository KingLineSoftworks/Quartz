#pragma once

#include <map>
#include <memory>

#include <GLFW/glfw3.h>

#include "quartz/managers/Loggers.hpp"

namespace quartz {

class Application;

namespace unit_test {
    class UnitTestClient;
}

namespace managers {
    class InputManager;
}
}

class quartz::managers::InputManager {
public: // classes
    class Client {
    public: // member functions
        Client() = delete;

    private: // static functions
        static quartz::managers::InputManager& getInstance(const std::shared_ptr<GLFWwindow>& p_glfwWindow) { return quartz::managers::InputManager::getInstance(p_glfwWindow); }

    private: // friend classes
        friend class quartz::Application;
        friend class quartz::unit_test::UnitTestClient;
    };

public: // member functions
    InputManager(InputManager&& other); /** @todo 2024/06/15 See if we can make this private and add a friend class. Seems like insert needs access to this??? We don't want others to see this. */
    InputManager(const InputManager& other) = delete;
    InputManager& operator=(const InputManager& other) = delete;

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

public: // static functions
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

private: // member functions
    static InputManager& getInstance(const std::shared_ptr<GLFWwindow>& p_glfwWindow);

private: // member functions
    InputManager(const std::shared_ptr<GLFWwindow>& p_glfwWindow);

private: // static variables
    static std::map<const GLFWwindow* const, quartz::managers::InputManager> inputManagerMap; // leaving this as member variable for the callbacks //

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
