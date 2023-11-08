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

    bool getKeypressed_q() const { return m_keypressed_q; }
    bool getKeypressed_esc() const { return m_keypressed_esc; }

    bool getKeypressed_w() const { return m_keypressed_w; }
    bool getKeypressed_a() const { return m_keypressed_a; }
    bool getKeypressed_s() const { return m_keypressed_s; }
    bool getKeypressed_d() const { return m_keypressed_d; }
    bool getKeypressed_space() const { return m_keypressed_space; }
    bool getKeypressed_shift() const { return m_keypressed_shift; }

    float getMousePosition_x() const { return m_mousePosition_x; }
    float getMousePosition_y() const { return m_mousePosition_y; }
    float getMousePositionOffset_x() const { return m_mousePositionOffset_x; }
    float getMousePositionOffset_y() const { return m_mousePositionOffset_y; }

    float getScrollOffset_x() const { return m_scrollOffset_x; }
    float getScrollOffset_y() const { return m_scrollOffset_y; }

    void setShouldCollectMouseInput(const bool shouldCollect);

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

    bool m_keypressed_q;
    bool m_keypressed_esc;

    bool m_keypressed_w;
    bool m_keypressed_a;
    bool m_keypressed_s;
    bool m_keypressed_d;
    bool m_keypressed_space;
    bool m_keypressed_shift;

    bool m_mouseUpdated;
    float m_mousePosition_x;
    float m_mousePosition_y;
    float m_mousePositionOffset_x;
    float m_mousePositionOffset_y;

    float m_scrollOffset_x;
    float m_scrollOffset_y;
};