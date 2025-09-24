#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class UniformBufferInfo;
}
}

class quartz::rendering::UniformBufferInfo {
public: // member functions
    UniformBufferInfo(
        const uint32_t locallyMappedBufferSizeBytes,
        const vk::MemoryPropertyFlags locallyMappedBufferPropertyFlags,
        const uint32_t bindingLocation,
        const uint32_t descriptorCount,
        const uint32_t objectStrideBytes,
        const bool isDynamic,
        const vk::ShaderStageFlags shaderStageFlags
    );
    UniformBufferInfo(const UniformBufferInfo& other);
    UniformBufferInfo(UniformBufferInfo&& other);
    ~UniformBufferInfo();

    UniformBufferInfo& operator=(const UniformBufferInfo& other);
    UniformBufferInfo& operator=(UniformBufferInfo&& other);

    USE_LOGGER(PIPELINE);

    uint32_t getLocallyMappedBufferSize() const { return m_locallyMappedBufferSize; }
    vk::BufferUsageFlags getLocallyMappedBufferVulkanUsageFlags() const { return m_locallyMappedBufferVulkanUsageFlags; }
    vk::MemoryPropertyFlags getLocallyMappedBufferVulkanPropertyFlags() const { return m_locallyMappedBufferVulkanPropertyFlags; }
    uint32_t getBindingLocation() const { return m_bindingLocation; }
    uint32_t getDescriptorCount() const { return m_descriptorCount; }
    uint32_t getObjectStrideBytes() const { return m_objectStrideBytes; }
    vk::DescriptorType getVulkanDescriptorType() const { return m_vulkanDescriptorType; }
    vk::ShaderStageFlags getVulkanShaderStageFlags() const { return m_vulkanShaderStageFlags; }

public: // static functions
    static uint32_t calculateDynamicUniformBufferByteStride(
        const uint32_t minUniformBufferOffsetAlignment,
        const uint32_t uniformBufferObjectSizeBytes
    );

private: // member variables
    uint32_t m_locallyMappedBufferSize;
    vk::BufferUsageFlags m_locallyMappedBufferVulkanUsageFlags;
    vk::MemoryPropertyFlags m_locallyMappedBufferVulkanPropertyFlags;

    uint32_t m_bindingLocation;
    uint32_t m_descriptorCount;
    uint32_t m_objectStrideBytes;
    vk::DescriptorType m_vulkanDescriptorType;
    vk::ShaderStageFlags m_vulkanShaderStageFlags;
};
