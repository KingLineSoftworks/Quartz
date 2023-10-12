#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/instance/Instance.hpp"

namespace quartz {
namespace rendering {
    class Device;
}
}

class quartz::rendering::Device {
public: // member functions
    Device(const quartz::rendering::Instance& renderingInstance);
    ~Device();

    USE_LOGGER(DEVICE);

    const vk::PhysicalDevice& getVulkanPhysicalDevice() const { return m_vulkanPhysicalDevice; }
    uint32_t getGraphicsQueueFamilyIndex() const { return m_graphicsQueueFamilyIndex; }
    const vk::UniqueDevice& getVulkanLogicalDevicePtr() const { return mp_vulkanLogicalDevice; }

private: // static functions
    static vk::PhysicalDevice getBestPhysicalDevice(
        const vk::UniqueInstance& p_instance
    );
    static uint32_t getGraphicsQueueFamilyIndex(
        const vk::PhysicalDevice& physicalDevice
    );
    static std::vector<const char*> getEnabledPhysicalDeviceExtensionNames(
        const vk::PhysicalDevice& physicalDevice
    );
    static vk::UniqueDevice createVulkanLogicalDeviceUniquePtr(
        const vk::PhysicalDevice& physicalDevice,
        const uint32_t graphicsQueueFamilyIndex,
        const std::vector<const char*>& validationLayerNames,
        const std::vector<const char*>& physicalDeviceExtensionNames
    );

private: // member variables
    vk::PhysicalDevice m_vulkanPhysicalDevice;
    const uint32_t m_graphicsQueueFamilyIndex;
    const std::vector<const char*> m_physicalDeviceExtensionNames;
    vk::UniqueDevice mp_vulkanLogicalDevice;
    vk::Queue m_vulkanGraphicsQueue;
    vk::Queue m_vulkanPresentQueue;
};