#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class BufferHelper;
    class LocallyMappedBuffer;
    class StagedBuffer;
    class StagedImageBuffer;
    class ImageBuffer;
}
}

class quartz::rendering::BufferHelper {
public: // member functions
    BufferHelper() = delete;

public: // static functions
    static std::string getUsageFlagsString(const vk::BufferUsageFlags bufferUsageFlags);

private: // static functions
    static vk::UniqueBuffer createVulkanBufferUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t bufferSizeBytes,
        const vk::BufferUsageFlags bufferUsageFlags
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceMemoryUniquePtr(
        const vk::PhysicalDevice& physicalDevice,
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t sizeBytes,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags requiredMemoryProperties
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceStagingMemoryUniquePtr(
        const vk::PhysicalDevice& physicalDevice,
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t sizeBytes,
        const void* p_bufferData,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );

private: // friends
    friend class quartz::rendering::LocallyMappedBuffer;
    friend class quartz::rendering::StagedBuffer;
    friend class quartz::rendering::StagedImageBuffer;
};
