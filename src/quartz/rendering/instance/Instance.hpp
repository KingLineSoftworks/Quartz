#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"

namespace quartz {
namespace rendering {
    class Instance;
}
}

class quartz::rendering::Instance {
public: // member functions
    Instance(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const bool validationLayersEnabled
    );
    ~Instance();

    USE_LOGGER(INSTANCE);

    const std::vector<const char*>& getValidationLayerNames() const { return m_validationLayerNames; }
    const vk::UniqueInstance& getVulkanInstancePtr() const { return mp_vulkanInstance; }

public: // static functions
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
    static vk::UniqueInstance createVulkanInstancePtr(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const std::vector<const char*>& enabledValidationLayerNames,
        const std::vector<const char*>& enabledExtensionNames
    );
    static vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> createVulkanDebugUtilsMessengerPtr(
        const vk::UniqueInstance& p_instance,
        const vk::DispatchLoaderDynamic& dispatchLoaderDynamic,
        const bool validationLayersEnabled
    );

private: // member variables
    const std::vector<const char*> m_validationLayerNames;
    const std::vector<const char*> m_instanceExtensionNames;
    vk::UniqueInstance mp_vulkanInstance;
    vk::DispatchLoaderDynamic m_vulkanDispatchLoaderDynamic;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> mp_vulkanDebugMessenger;
};