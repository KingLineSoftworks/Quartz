#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferUtil.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class LocallyMappedBuffer;
}
}

class quartz::rendering::LocallyMappedBuffer {
public: // member functions
    LocallyMappedBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t sizeBytes,
        const vk::BufferUsageFlags usageFlags,
        const vk::MemoryPropertyFlags requiredMemoryProperties
    );
    LocallyMappedBuffer(LocallyMappedBuffer&& other);
    ~LocallyMappedBuffer();

    LocallyMappedBuffer& operator=(LocallyMappedBuffer&& other);

    USE_LOGGER(BUFFER);

    const vk::UniqueBuffer& getVulkanLogicalBufferPtr() const { return mp_vulkanLogicalBuffer; }
    void* getMappedLocalMemoryPtr() { return mp_mappedLocalMemory; }

private: // static functions
    static void* mapVulkanPhysicalDeviceMemoryToLocalMemory(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t bufferSizeBytes,
        const vk::UniqueDeviceMemory& p_physicalDeviceMemory
    );

private: // member variables
    uint32_t m_sizeBytes;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_requiredMemoryProperties;

    vk::UniqueBuffer mp_vulkanLogicalBuffer;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
    void* mp_mappedLocalMemory;
};