#include "quartz/rendering/pipeline/UniformTextureArrayInfo.hpp"

quartz::rendering::UniformTextureArrayInfo::UniformTextureArrayInfo(
    const uint32_t bindingLocation,
    const uint32_t descriptorCount,
    const vk::ShaderStageFlags shaderStageFlags
) :
    m_bindingLocation(bindingLocation),
    m_descriptorCount(descriptorCount),
    m_vulkanDescriptorType(vk::DescriptorType::eSampledImage),
    m_vulkanShaderStageFlags(shaderStageFlags)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::UniformTextureArrayInfo::UniformTextureArrayInfo(
    const quartz::rendering::UniformTextureArrayInfo& other
) :
    m_bindingLocation(other.m_bindingLocation),
    m_descriptorCount(other.m_descriptorCount),
    m_vulkanDescriptorType(other.m_vulkanDescriptorType),
    m_vulkanShaderStageFlags(other.m_vulkanShaderStageFlags)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::UniformTextureArrayInfo::~UniformTextureArrayInfo() {
    LOG_FUNCTION_CALL_TRACEthis("");
}