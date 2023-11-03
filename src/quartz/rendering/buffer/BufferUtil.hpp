#pragma once

#include <memory>
#include <string>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class BufferUtil;
    class ImageBuffer;
    class ImageBufferUtil;
    class LocallyMappedBuffer;
    class StagedBuffer;
    class StagedImageBuffer;
}
}

class quartz::rendering::BufferUtil {
public: // member functions
    BufferUtil() = delete;

public: // static functions
    static std::string getUsageFlagsString(const vk::BufferUsageFlags bufferUsageFlags);

private: // static functions
    static vk::UniqueBuffer createVulkanBufferPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t bufferSizeBytes,
        const vk::BufferUsageFlags bufferUsageFlags
    );
    static uint32_t chooseMemoryTypeIndex(
        const vk::PhysicalDevice& physicalDevice,
        const vk::MemoryPropertyFlags requiredMemoryProperties,
        const vk::MemoryRequirements& memoryRequirements
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceMemoryPtr(
        const vk::PhysicalDevice& physicalDevice,
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t sizeBytes,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags requiredMemoryProperties
    );
    static void populateVulkanPhysicalDeviceMemoryWithLocalData(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t sizeBytes,
        const void* p_bufferData,
        vk::UniqueDeviceMemory& p_logicalBufferPhysicalMemory
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceStagingMemoryPtr(
        const vk::PhysicalDevice& physicalDevice,
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t sizeBytes,
        const void* p_bufferData,
        const vk::UniqueBuffer& p_logicalBuffer,
        const vk::MemoryPropertyFlags requiredMemoryProperties
    );

    // ----- command buffer things ----- //

    static void submitVulkanCommandBufferPtr(
        const vk::Queue& graphicsQueue,
        const vk::UniqueCommandBuffer& p_commandBuffer
    );

private: // friends
    friend class quartz::rendering::ImageBuffer;
    friend class quartz::rendering::ImageBufferUtil;
    friend class quartz::rendering::LocallyMappedBuffer;
    friend class quartz::rendering::StagedBuffer;
    friend class quartz::rendering::StagedImageBuffer;
};

class quartz::rendering::ImageBufferUtil {
public: // member functions
    ImageBufferUtil() = delete;

private: // static functions
    static vk::UniqueImage createVulkanImagePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling
    );
    static vk::UniqueDeviceMemory allocateVulkanPhysicalDeviceImageMemory(
        const vk::PhysicalDevice& physicalDevice,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueImage& p_image,
        const vk::MemoryPropertyFlags requiredMemoryProperties
    );

private: // friends
    friend class quartz::rendering::ImageBuffer;
    friend class quartz::rendering::StagedImageBuffer;
};
