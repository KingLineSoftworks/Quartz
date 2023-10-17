#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"

namespace quartz {
namespace rendering {
    class BufferHelper;
    class LocallyMappedBuffer;
    class StagedBuffer;
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

private: // friends
    friend class quartz::rendering::LocallyMappedBuffer;
    friend class quartz::rendering::StagedBuffer;
};

class quartz::rendering::LocallyMappedBuffer {
public: // member functions
    LocallyMappedBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t sizeBytes,
        const vk::BufferUsageFlags usageFlags,
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );
    LocallyMappedBuffer(LocallyMappedBuffer&& other);
    ~LocallyMappedBuffer();

    USE_LOGGER(BUFFER);

    const vk::UniqueBuffer& getVulkanLogicalBufferPtr() const { return mp_vulkanLogicalBuffer; }
    void* getMappedLocalMemoryPtr() { return mp_mappedLocalMemory; }

private: // static functions
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceMemoryUniquePtr(
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

    const vk::UniqueBuffer& getLogicalBufferPtr() const { return mp_vulkanLogicalBuffer; }

private: // static functions
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceStagingMemoryUniquePtr(
        const vk::PhysicalDevice& physicalDevice,
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t sizeBytes,
        const void* p_bufferData,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceDestinationMemoryUniquePtr(
        const vk::PhysicalDevice& physicalDevice,
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t sizeBytes,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags memoryPropertyFlags,
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