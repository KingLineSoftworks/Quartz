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
    const vk::UniqueSurfaceKHR& getVulkanSurfacePtr() const { return mp_vulkanSurface; }
    const vk::SurfaceCapabilitiesKHR& getVulkanSurfaceCapabilities() const { return m_vulkanSurfaceCapabilities; }
    const vk::SurfaceFormatKHR& getVulkanSurfaceFormat() const { return m_vulkanSurfaceFormat; }
    const vk::PresentModeKHR& getVulkanPresentMode() const { return m_vulkanPresentMode; }
    const vk::Extent2D& getVulkanExtent() const { return m_vulkanExtent; }

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

    std::shared_ptr<GLFWwindow> mp_glfwWindow;

    vk::UniqueSurfaceKHR mp_vulkanSurface;
    vk::SurfaceCapabilitiesKHR m_vulkanSurfaceCapabilities;
    vk::SurfaceFormatKHR m_vulkanSurfaceFormat;
    vk::PresentModeKHR m_vulkanPresentMode;
    vk::Extent2D m_vulkanExtent;

private: // friends
    friend void quartz::rendering::Window2::glfwFramebufferSizeCallback(
        GLFWwindow* p_glfwWindow,
        int updatedWindowWidthPixels,
        int updatedWindowHeightPixels
    );
};
