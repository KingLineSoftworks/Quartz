#include "quartz/rendering/pipeline/UniformSamplerInfo.hpp"

quartz::rendering::UniformSamplerInfo::UniformSamplerInfo(
    const uint32_t bindingLocation,
    const uint32_t descriptorCount,
    const vk::ShaderStageFlags shaderStageFlags
) :
    m_bindingLocation(bindingLocation),
    m_descriptorCount(descriptorCount),
    m_vulkanDescriptorType(vk::DescriptorType::eSampler),
    m_vulkanShaderStageFlags(shaderStageFlags)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::UniformSamplerInfo::UniformSamplerInfo(
    const quartz::rendering::UniformSamplerInfo& other
) :
    m_bindingLocation(other.m_bindingLocation),
    m_descriptorCount(other.m_descriptorCount),
    m_vulkanDescriptorType(other.m_vulkanDescriptorType),
    m_vulkanShaderStageFlags(other.m_vulkanShaderStageFlags)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::UniformSamplerInfo::~UniformSamplerInfo() {
    LOG_FUNCTION_CALL_TRACEthis("");
}