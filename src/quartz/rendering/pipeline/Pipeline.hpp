#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/window2/Window2.hpp"

namespace quartz {
namespace rendering {
    class Pipeline;
}
}

class quartz::rendering::Pipeline {
public: // member functions
    Pipeline(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window2& renderingWindow
    );
    ~Pipeline();

    USE_LOGGER(PIPELINE);

private: // static functions
    static vk::UniqueShaderModule createVulkanShaderModuleUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const std::string& filepath
    );
    static vk::UniqueDescriptorSetLayout createVulkanDescriptorSetLayoutUniquePtr(
        const vk::UniqueDevice& p_logicalDevice
    );
    static vk::UniquePipelineLayout createVulkanPipelineLayoutUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout
    );
    static vk::UniqueRenderPass createVulkanRenderPassUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::SurfaceFormatKHR& surfaceFormat
    );
    static vk::UniquePipeline createVulkanGraphicsPipelineUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::VertexInputBindingDescription vertexInputBindingDescriptions,
        const std::array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescriptions,
        const std::vector<vk::Viewport> viewports,
        const std::vector<vk::Rect2D> scissorRectangles,
        const std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates,
        const std::vector<vk::DynamicState> dynamicStates,
        const vk::UniqueShaderModule& p_vertexShaderModule,
        const vk::UniqueShaderModule& p_fragmentShaderModule,
        const vk::UniquePipelineLayout& p_pipelineLayout,
        const vk::UniqueRenderPass& p_renderPass
    );

private: // member variables
    vk::VertexInputBindingDescription m_vulkanVertexInputBindingDescriptions;
    std::array<vk::VertexInputAttributeDescription, 2> m_vulkanVertexInputAttributeDescriptions;
    std::vector<vk::Viewport> m_vulkanViewports;
    std::vector<vk::Rect2D> m_vulkanScissorRectangles;
    std::vector<vk::PipelineColorBlendAttachmentState> m_vulkanColorBlendAttachmentStates;
    std::vector<vk::DynamicState> m_vulkanDynamicStates;

    vk::UniqueShaderModule mp_vulkanVertexShaderModule;
    vk::UniqueShaderModule mp_vulkanFragmentShaderModule;
    vk::UniqueDescriptorSetLayout mp_vulkanDescriptorSetLayout;
    vk::UniquePipelineLayout mp_vulkanPipelineLayout;
    vk::UniqueRenderPass mp_vulkanRenderPass;
    vk::UniquePipeline mp_vulkanUniqueGraphicsPipeline;
};