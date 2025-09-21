#pragma once

#include <map>
#include <memory>

#include <GLFW/glfw3.h>

#include "quartz/managers/Loggers.hpp"

namespace quartz {

class Application;

namespace managers {
    class InputManager;
}

namespace unit_test {
    class InputManagerUnitTestClient;
}

} // namespace Quartz

class quartz::managers::InputManager {
public: // classes
    struct KeyPressInfo {
        bool down;
        bool impacted;
        bool released;
    };

    class Client {
    public: // member functions
        Client() = delete;

    private: // static functions
        static quartz::managers::InputManager& getInstance(const std::shared_ptr<GLFWwindow>& p_glfwWindow) { return quartz::managers::InputManager::getInstance(p_glfwWindow); }

    private: // friend classes
        friend class quartz::Application;
        friend class quartz::unit_test::InputManagerUnitTestClient;
    };

public: // member functions
    InputManager(InputManager&& other); /** @todo 2024/06/15 See if we can make this private and add a friend class. Seems like insert needs access to this??? We don't want others to see this. */
    InputManager(const InputManager& other) = delete;
    InputManager& operator=(const InputManager& other) = delete;

    USE_LOGGER(INPUTMAN);

    void collectInput();

    const KeyPressInfo& getKeyInfo_a() const { return m_a; }
    const KeyPressInfo& getKeyInfo_d() const { return m_d; }
    const KeyPressInfo& getKeyInfo_l() const { return m_l; }
    const KeyPressInfo& getKeyInfo_p() const { return m_p; }
    const KeyPressInfo& getKeyInfo_q() const { return m_q; }
    const KeyPressInfo& getKeyInfo_s() const { return m_s; }
    const KeyPressInfo& getKeyInfo_w() const { return m_w; }

    const KeyPressInfo& getKeyInfo_esc() const { return m_esc; }
    const KeyPressInfo& getKeyInfo_shift() const { return m_shift; }
    const KeyPressInfo& getKeyInfo_ctrl() const { return m_ctrl; }
    const KeyPressInfo& getKeyInfo_space() const { return m_space; }

    const KeyPressInfo& getKeyInfo_period() const { return m_period; }

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
    KeyPressInfo  getKeyPressInfo(const bool currentDown, const int glfwKey);
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

    KeyPressInfo m_a;
    KeyPressInfo m_d;
    KeyPressInfo m_l;
    KeyPressInfo m_p;
    KeyPressInfo m_q;
    KeyPressInfo m_s;
    KeyPressInfo m_w;

    KeyPressInfo m_esc;
    KeyPressInfo m_shift;
    KeyPressInfo m_ctrl;
    KeyPressInfo m_space;

    KeyPressInfo m_period;

    float m_mousePosition_x;
    float m_mousePosition_y;
    float m_mousePositionOffset_x;
    float m_mousePositionOffset_y;

    float m_scrollOffset_x;
    float m_scrollOffset_y;
};

