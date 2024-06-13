#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"

namespace quartz {
namespace rendering {
    class UniformTextureArrayInfo;
}
}

class quartz::rendering::UniformTextureArrayInfo {
public: // member functions
    UniformTextureArrayInfo(
        const uint32_t bindingLocation,
        const uint32_t descriptorCount,
        const vk::ShaderStageFlags shaderStageFlags
    );
    UniformTextureArrayInfo(const UniformTextureArrayInfo& other);
    ~UniformTextureArrayInfo();

    USE_LOGGER(PIPELINE);

    uint32_t getBindingLocation() const { return m_bindingLocation; }
    uint32_t getDescriptorCount() const { return m_descriptorCount; }
    vk::DescriptorType getVulkanDescriptorType() const { return m_vulkanDescriptorType; }
    vk::ShaderStageFlags getVulkanShaderStageFlags() const { return m_vulkanShaderStageFlags; }

private: // member variables
    uint32_t m_bindingLocation;
    uint32_t m_descriptorCount;
    vk::DescriptorType m_vulkanDescriptorType;
    vk::ShaderStageFlags m_vulkanShaderStageFlags;
};