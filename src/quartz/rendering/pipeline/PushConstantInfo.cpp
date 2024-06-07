#include "PushConstantInfo.hpp"

quartz::rendering::PushConstantInfo::PushConstantInfo(
    const vk::ShaderStageFlags shaderStageFlags,
    const uint32_t offset,
    const uint32_t size
) :
    m_vulkanShaderStageFlags(shaderStageFlags),
    m_offset(offset),
    m_size(size)
{}


quartz::rendering::PushConstantInfo::PushConstantInfo(
    const quartz::rendering::PushConstantInfo& other
) :
    m_vulkanShaderStageFlags(other.m_vulkanShaderStageFlags),
    m_offset(other.m_offset),
    m_size(other.m_size)
{}