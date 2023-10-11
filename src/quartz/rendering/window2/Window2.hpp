#pragma once

#include <string>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"

namespace quartz {
namespace rendering {
    class Window2;
}
}

class quartz::rendering::Window2 {
public: // interface
    Window2(
        const std::string& name,
        const uint32_t widthPixels,
        const uint32_t heightPixels,
        const quartz::rendering::Instance& renderingInstance,
        const quartz::rendering::Device& renderingDevice
    );
    ~Window2();

    USE_LOGGER(WINDOW);

    const std::shared_ptr<GLFWwindow>& getGLFWwindowPtr() const { return mp_glfwWindow; }

    bool shouldClose() const;

    bool getWasResized() const { return m_wasResized; }
    void clearWasResized() { m_wasResized = false; }

public: // static functions
    // The callback we give to glfw to use when it resizes the window
    static void glfwFramebufferSizeCallback(
        GLFWwindow* p_glfwWindow,
        int updatedWindowWidthPixels,
        int updatedWindowHeightPixels
    );

private: // static functions
    static std::shared_ptr<GLFWwindow> createGLFWwindowPtr(
        const std::string& name,
        const uint32_t widthPixels,
        const uint32_t heightPixels,
        const void* p_windowUser
    );
    static vk::UniqueSurfaceKHR createVulkanSurfaceUniquePtr(
        const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
        const vk::UniqueInstance& p_instance
    );
    static vk::SurfaceFormatKHR getBestSurfaceFormat(
        const vk::UniqueSurfaceKHR& p_surface,
        const vk::PhysicalDevice& physicalDevice
    );
    static vk::PresentModeKHR getBestPresentMode(
        const vk::UniqueSurfaceKHR& p_surface,
        const vk::PhysicalDevice& physicalDevice
    );
    static vk::Extent2D getBestVulkanExtent(
        const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
        const vk::SurfaceCapabilitiesKHR& surfaceCapabilities
    );

private: // member variables
    const std::string m_name;

    uint32_t m_widthPixels;
    uint32_t m_heightPixels;
    bool m_wasResized;

    UNUSED std::shared_ptr<GLFWwindow> mp_glfwWindow;
    UNUSED vk::UniqueSurfaceKHR mp_vulkanSurface;
    UNUSED vk::SurfaceCapabilitiesKHR m_vulkanSurfaceCapabilities;
    UNUSED vk::SurfaceFormatKHR m_vulkanSurfaceFormat;
    UNUSED vk::PresentModeKHR m_vulkanPresentMode;
    UNUSED vk::Extent2D m_vulkanExtent;

private: // friends
    friend void quartz::rendering::Window2::glfwFramebufferSizeCallback(
        GLFWwindow* p_glfwWindow,
        int updatedWindowWidthPixels,
        int updatedWindowHeightPixels
    );
};

