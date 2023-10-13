#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"

namespace quartz {
namespace rendering {
    class Buffer;
}
}

class quartz::rendering::Buffer {
public: // member functions
    Buffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t sizeBytes,
        const vk::BufferUsageFlags usageFlags,
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );
    Buffer(Buffer&& other);
    ~Buffer();

    USE_LOGGER(BUFFER);

public: // static functions
    static std::string getUsageFlagsString(const vk::BufferUsageFlags bufferUsageFlags);

private: // static functions
    static vk::UniqueBuffer createVulkanBufferUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t bufferSizeBytes,
        const vk::BufferUsageFlags bufferUsageFlags
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceMemory(
        const vk::PhysicalDevice& physicalDevice,
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t bufferSizeBytes,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags requiredMemoryProperties
    );
    static void* mapVulkanPhysicalDeviceMemoryToLocalMemory(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t bufferSizeBytes,
        const vk::UniqueDeviceMemory& p_physicalDeviceMemory
    );

private: // member variables
    uint32_t m_sizeBytes;
    vk::BufferUsageFlags m_usageFlags;
    vk::MemoryPropertyFlags m_memoryPropertyFlags;
    vk::UniqueBuffer mp_vulkanLogicalBuffer;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
    void* mp_mappedLocalMemory;
};