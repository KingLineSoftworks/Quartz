#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferHelper.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class StagedBuffer;
}
}

class quartz::rendering::StagedBuffer {
public: // member functions
    StagedBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t sizeBytes,
        const vk::BufferUsageFlags usageFlags,
        const void* p_bufferData
    );
    StagedBuffer(StagedBuffer&& other);
    ~StagedBuffer();

    USE_LOGGER(BUFFER);

    const vk::UniqueBuffer& getVulkanLogicalBufferPtr() const { return mp_vulkanLogicalBuffer; }

private: // static functions
    static void populateVulkanLogicalBufferWithStagedData(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t sizeBytes,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::UniqueBuffer& p_logicalStagingBuffer
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceDestinationMemoryUniquePtr(
        const vk::PhysicalDevice& physicalDevice,
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t sizeBytes,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags requiredMemoryProperties,
        const vk::UniqueBuffer& p_logicalStagingBuffer
    );

private: // member variables
    uint32_t m_sizeBytes;
    vk::BufferUsageFlags m_usageFlags;

    vk::UniqueBuffer mp_vulkanLogicalStagingBuffer;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceStagingMemory;

    vk::UniqueBuffer mp_vulkanLogicalBuffer;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
};
