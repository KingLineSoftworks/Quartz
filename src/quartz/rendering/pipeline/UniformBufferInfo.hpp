#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/LocallyMappedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class UniformBufferInfo;
}
}

class quartz::rendering::UniformBufferInfo {
public: // member functions
    UniformBufferInfo(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t locallyMappedBufferSizeBytes,
        const vk::MemoryPropertyFlags locallyMappedBufferPropertyFlags,
        const uint32_t bindingLocation,
        const uint32_t descriptorCount,
        const uint32_t objectStrideBytes,
        const vk::DescriptorType descriptorType, /** @todo 2024/06/06 Make this a boolean flag for whether or not the buffer is dynamic */
        const vk::ShaderStageFlags shaderStageFlags
    );
    UniformBufferInfo(const UniformBufferInfo& other);
    UniformBufferInfo(UniformBufferInfo&& other);
    ~UniformBufferInfo();

    UniformBufferInfo& operator=(const UniformBufferInfo& other);
    UniformBufferInfo& operator=(UniformBufferInfo&& other);

    USE_LOGGER(PIPELINE);

    uint32_t getLocallyMappedBufferSize() const { return m_locallyMappedBufferSize; }
    vk::BufferUsageFlags getLocallyMappedBufferUsageFlags() const { return m_locallyMappedBufferUsageFlags; }
    vk::MemoryPropertyFlags getLocallyMappedBufferPropertyFlags() const { return m_locallyMappedBufferPropertyFlags; }
    uint32_t getBindingLocation() const { return m_bindingLocation; }
    uint32_t getDescriptorCount() const { return m_descriptorCount; }
    uint32_t getObjectStrideBytes() const { return m_objectStrideBytes; }
    vk::DescriptorType getVulkanDescriptorType() const { return m_vulkanDescriptorType; }
    vk::ShaderStageFlags getVulkanShaderStageFlags() const { return m_vulkanShaderStageFlags; }

public: // static functions
    static uint32_t calculateDynamicUniformBufferByteStride(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t uniformBufferObjectSizeBytes
    );

private: // member variables
    uint32_t m_locallyMappedBufferSize;
    vk::BufferUsageFlags m_locallyMappedBufferUsageFlags;
    vk::MemoryPropertyFlags m_locallyMappedBufferPropertyFlags;

    uint32_t m_bindingLocation;
    uint32_t m_descriptorCount;
    uint32_t m_objectStrideBytes;
    vk::DescriptorType m_vulkanDescriptorType;
    vk::ShaderStageFlags m_vulkanShaderStageFlags;
};