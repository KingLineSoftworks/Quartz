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
        const vk::BufferUsageFlags locallyMappedBufferUsageFlags,
        const vk::MemoryPropertyFlags locallyMappedBufferPropertyFlags,
        const uint32_t bindingLocation,
        const uint32_t descriptorCount,
        const uint32_t objectStrideBytes,
        const vk::DescriptorType descriptorType,
        const vk::ShaderStageFlags shaderStageFlags
    );
    UniformBufferInfo(UniformBufferInfo&& other);
    ~UniformBufferInfo();

    UniformBufferInfo& operator=(UniformBufferInfo&& other);

    USE_LOGGER(PIPELINE);

    uint32_t getBindingLocation() const { return m_bindingLocation; }
    uint32_t getDescriptorCount() const { return m_descriptorCount; }
    uint32_t getObjectStrideBytes() const { return m_objectStrideBytes; }
    vk::DescriptorType getVulkanDescriptorType() const { return m_vulkanDescriptorType; }
    vk::ShaderStageFlags getVulkanShaderStageFlags() const { return m_vulkanShaderStageFlags; }

    const quartz::rendering::LocallyMappedBuffer& getLocallyMappedBuffer() const { return m_locallyMappedBuffer; }

public: // static functions
    static uint32_t calculateDynamicUniformBufferByteStride(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t uniformBufferObjectSizeBytes
    );

private: // static functions
    static vk::WriteDescriptorSet createVulkanWriteDescriptorSet();

private: // member variables
    uint32_t m_bindingLocation;
    uint32_t m_descriptorCount;
    uint32_t m_objectStrideBytes;
    vk::DescriptorType m_vulkanDescriptorType;
    vk::ShaderStageFlags m_vulkanShaderStageFlags;

    quartz::rendering::LocallyMappedBuffer m_locallyMappedBuffer;
};