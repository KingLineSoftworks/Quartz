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
    class ImageBuffer;
    class DepthBuffer;
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
    friend class quartz::rendering::ImageBuffer;
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

    const vk::UniqueBuffer& getVulkanLogicalBufferPtr() const { return mp_vulkanLogicalBuffer; }

private: // static functions
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

class quartz::rendering::ImageBuffer {
public: // member functions
    ImageBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const uint32_t sizeBytes,
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling,
        const void* p_bufferData
    );
    ImageBuffer(ImageBuffer&& other);
    ~ImageBuffer();

    USE_LOGGER(BUFFER);

    const vk::UniqueImage& getVulkanImagePtr() const { return mp_vulkanImage; }

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
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::UniqueBuffer& p_stagingBuffer,
        const vk::UniqueImage& p_image,
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );
    static void transitionImageLayout(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const vk::UniqueImage& p_image,
        const vk::ImageLayout inputLayout,
        const vk::ImageLayout outputLayout
    );
    static void copyStagedBufferToImage(
        const uint32_t graphicsQueueFamilyIndex,
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Queue& graphicsQueue,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::UniqueBuffer& p_stagingBuffer,
        const vk::UniqueImage& p_image
    );

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    uint32_t m_sizeBytes;
    vk::ImageUsageFlags m_usageFlags;
    vk::Format m_format;
    vk::ImageTiling m_tiling;

    vk::UniqueBuffer mp_vulkanLogicalStagingBuffer;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceStagingMemory;

    vk::UniqueImage mp_vulkanImage;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
};

class quartz::rendering::DepthBuffer {
public: // member functions
    DepthBuffer(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const vk::ImageUsageFlags usageFlags,
        const vk::Format format,
        const vk::ImageTiling tiling
    );
    DepthBuffer(DepthBuffer&& other);
    DepthBuffer& operator=(DepthBuffer&& other);
    ~DepthBuffer();

    void reset();

    USE_LOGGER(BUFFER);

    const vk::UniqueImage& getVulkanImagePtr() const { return mp_vulkanImage; }
    const vk::UniqueImageView& getVulkanImageViewPtr() const { return mp_vulkanImageView; }

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
        const vk::MemoryPropertyFlags memoryPropertyFlags
    );
    static vk::UniqueImageView createVulkanImageViewPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::Format format,
        const vk::UniqueImage& p_image
    );

private: // member variables
    uint32_t m_imageWidth;
    uint32_t m_imageHeight;
    vk::ImageUsageFlags m_usageFlags;
    vk::Format m_format;
    vk::ImageTiling m_tiling;

    vk::UniqueImage mp_vulkanImage;
    vk::UniqueDeviceMemory mp_vulkanPhysicalDeviceMemory;
    vk::UniqueImageView mp_vulkanImageView;
};