#include "quartz/rendering/pipeline/UniformSamplerCubeInfo.hpp"

quartz::rendering::UniformSamplerCubeInfo::UniformSamplerCubeInfo(
    const uint32_t bindingLocation,
    const uint32_t descriptorCount,
    const vk::ShaderStageFlags shaderStageFlags
) :
    m_bindingLocation(bindingLocation),
    m_descriptorCount(descriptorCount),
    m_vulkanDescriptorType(vk::DescriptorType::eCombinedImageSampler),
    m_vulkanShaderStageFlags(shaderStageFlags)
{}

quartz::rendering::UniformSamplerCubeInfo::UniformSamplerCubeInfo(
    const quartz::rendering::UniformSamplerCubeInfo& other
) :
    m_bindingLocation(other.m_bindingLocation),
    m_descriptorCount(other.m_descriptorCount),
    m_vulkanDescriptorType(other.m_vulkanDescriptorType),
    m_vulkanShaderStageFlags(other.m_vulkanShaderStageFlags)
{}

quartz::rendering::UniformSamplerCubeInfo::~UniformSamplerCubeInfo() {}
