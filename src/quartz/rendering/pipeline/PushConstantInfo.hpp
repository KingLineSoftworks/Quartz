#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"

namespace quartz {
namespace rendering {
    class PushConstantInfo;
}
}

class quartz::rendering::PushConstantInfo {
public: // member functions
    PushConstantInfo(
        const vk::ShaderStageFlags shaderStageFlags,
        const uint32_t offset,
        const uint32_t size
    );
    PushConstantInfo(const PushConstantInfo& other);

    USE_LOGGER(PIPELINE);

    vk::ShaderStageFlags getVulkanShaderStageFlags() const { return m_vulkanShaderStageFlags; }
    uint32_t getOffset() const { return m_offset; }
    uint32_t getSize() const { return m_size; }

private: // member variables
    vk::ShaderStageFlags m_vulkanShaderStageFlags;
    uint32_t m_offset;
    uint32_t m_size;
};
