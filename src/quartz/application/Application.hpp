#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

#include "quartz/Loggers.hpp"
#include "quartz/rendering/window/Window.hpp"

namespace quartz {
    class Application;
}

class quartz::Application {
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

    static vk::UniqueInstance createUniqueVulkanInstance(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const bool validationLayersEnabled
    );

    static vk::DebugUtilsMessengerEXT createVulkanDebugUtilsMessenger(
        const vk::UniqueInstance& uniqueInstance,
        const bool validationLayersEnabled
    );

private: // member variables
    const std::string m_applicationName;
    const uint32_t m_majorVersion;
    const uint32_t m_minorVersion;
    const uint32_t m_patchVersion;

    std::unique_ptr<quartz::rendering::Window> mp_window;

    vk::UniqueInstance m_uniqueVulkanInstance;
    vk::DebugUtilsMessengerEXT m_vulkanDebugMessenger; // linking errors when we try to use the Unique version
};