#pragma once

#include <memory>
#include <string>
#include <utility>

#include <vulkan/vulkan.hpp>

#include "quartz/Loggers.hpp"
#include "quartz/rendering/window/Window.hpp"

namespace quartz {
    class Application;
}

class quartz::Application {
public: // classes and enums
    struct QueueFamilyIndices {
        uint32_t graphicsFamilyIndex;
        uint32_t presentFamilyIndex;
    };

    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<vk::SurfaceFormatKHR> surfaceFormats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

public: // interface
    Application(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const uint32_t windowWidthPixels,
        const uint32_t windowHeightPixels,
        const bool validationLayersEnabled
    );
    ~Application();

    USE_LOGGER(APPLICATION);

    void run();

public: // static functions
    // The callback the validation layer uses
    static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
        void* p_userData
    );

private: // static functions
    static std::vector<const char*> getEnabledValidationLayerNames(
        const bool validationLayersEnabled
    );

    static std::vector<const char*> getEnabledInstanceExtensionNames(
        const bool validationLayersEnabled
    );

    static vk::UniqueInstance createVulkanUniqueInstance(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const std::vector<const char*>& enabledValidationLayerNames,
        const std::vector<const char*>& enabledExtensionNames
    );

    static vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> createVulkanUniqueDebugUtilsMessenger(
        const vk::UniqueInstance& uniqueInstance,
        const vk::DispatchLoaderDynamic& dispatchLoaderDynamic,
        const bool validationLayersEnabled
    );

    static vk::UniqueSurfaceKHR createVulkanSurface(
        const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
        const vk::UniqueInstance& uniqueInstance
    );

    static std::pair<vk::PhysicalDevice, quartz::Application::QueueFamilyIndices> getBestPhysicalDeviceAndQueueFamilyIndices(
        const vk::UniqueInstance& uniqueInstance,
        const vk::UniqueSurfaceKHR& uniqueSurface
    );

    static std::vector<const char*> getEnabledPhysicalDeviceExtensionNames(
        const vk::PhysicalDevice& physicalDevice
    );

    static vk::UniqueDevice createVulkanUniqueLogicalDevice(
        const vk::PhysicalDevice& physicalDevice,
        const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
        const std::vector<const char*>& validationLayerNames,
        const std::vector<const char*>& physicalDeviceExtensionNames
    );

    static vk::Extent2D getBestSwapExtent(
        const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
        const vk::SurfaceCapabilitiesKHR& surfaceCapabilities
    );

    static vk::SurfaceFormatKHR getBestSurfaceFormat(
        const vk::UniqueSurfaceKHR& uniqueSurface,
        const vk::PhysicalDevice& physicalDevice
    );

    static vk::PresentModeKHR getBestPresentMode(
        const vk::UniqueSurfaceKHR& uniqueSurface,
        const vk::PhysicalDevice& physicalDevice
    );

    static vk::UniqueSwapchainKHR createVulkanUniqueSwapchain(
        const vk::UniqueSurfaceKHR& uniqueSurface,
        const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
        const vk::Extent2D& swapExtent,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const vk::PresentModeKHR& presentMode
    );

    static std::vector<vk::UniqueImageView> createVulkanUniqueImageViews(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const std::vector<vk::Image>& swapchainImages
    );

private: // member variables
    const std::string m_applicationName;
    const uint32_t m_majorVersion;
    const uint32_t m_minorVersion;
    const uint32_t m_patchVersion;

    // ----- Context tings (tings lishted in the oahdah of which dey ahh creah'id mang) ---- //

    // instance
    std::vector<const char*> m_validationLayerNames;
    std::vector<const char*> m_instanceExtensionNames;
    vk::UniqueInstance m_vulkanUniqueInstance;
    vk::DispatchLoaderDynamic m_vulkanDispatchLoaderDynamic;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_vulkanUniqueDebugMessenger; // we need to use the dynamic loader instead of the static loader (not sure why we can't statically link 😔)
    // window
    std::shared_ptr<quartz::rendering::Window> mp_window;
    vk::UniqueSurfaceKHR m_vulkanUniqueSurface;
    // device (and its queues)
    std::pair<vk::PhysicalDevice, quartz::Application::QueueFamilyIndices> m_vulkanPhysicalDeviceAndQueueFamilyIndex; // Because these are both (physical device && queue family indices) determined at the "same" time and truly are coupled
    std::vector<const char*> m_physicalDeviceExtensionNames;
    vk::UniqueDevice  m_vulkanUniqueLogicalDevice;
    vk::Queue m_vulkanGraphicsQueue;
    vk::Queue m_vulkanPresentQueue;
    // swapchain
    vk::SurfaceCapabilitiesKHR m_vulkanSurfaceCapabilities; // should maybe go with the window stuff? seems directly related to the surface
    vk::Extent2D m_vulkanSwapExtent; // should maybe go with the window stuff? seems directly related to the surface
    vk::SurfaceFormatKHR m_vulkanSurfaceFormat; // should maybe go with the window stuff? seems directly related to the surface
    vk::PresentModeKHR m_vulkanPresentMode; // should maybe go with the window stuff? seems directly related to the surface
    vk::UniqueSwapchainKHR m_vulkanUniqueSwapchain;
    std::vector<vk::Image> m_vulkanSwapchainImages;
    std::vector<vk::UniqueImageView> m_vulkanUniqueImageViews;
};