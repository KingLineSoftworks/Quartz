#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Window2.hpp"

namespace quartz {
namespace rendering {
    struct UniformBufferObject;
    class Pipeline;
}
}

struct quartz::rendering::UniformBufferObject {
public: // member functions
    UniformBufferObject() = default;

    UniformBufferObject(
        glm::mat4 model_,
        glm::mat4 view_,
        glm::mat4 projection_
    );

public: // member variables
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
};

class quartz::rendering::Pipeline {
public: // member functions
    Pipeline(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window2& renderingWindow,
        const uint32_t maxNumFramesInFlight
    );
    ~Pipeline();

    uint32_t getMaxNumFramesInFlight() const { return m_maxNumFramesInFlight; }
    const vk::UniqueRenderPass& getVulkanRenderPassPtr() const { return mp_vulkanRenderPass; }

    USE_LOGGER(PIPELINE);

private: // static functions
    static vk::UniqueShaderModule createVulkanShaderModuleUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const std::string& filepath
    );
    static std::vector<quartz::rendering::Buffer> createUniformBuffers(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t numBuffers
    );
    static vk::UniqueDescriptorSetLayout createVulkanDescriptorSetLayoutUniquePtr(
        const vk::UniqueDevice& p_logicalDevice
    );
    static vk::UniqueDescriptorPool createVulkanDescriptorPoolUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t numDescriptorSets
    );
    static std::vector<vk::DescriptorSet> allocateVulkanDescriptorSets(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t maxNumFramesInFlight,
        const std::vector<quartz::rendering::Buffer>& uniformBuffers,
        const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout,
        const vk::UniqueDescriptorPool& uniqueDescriptorPool
    );
    static vk::UniqueRenderPass createVulkanRenderPassUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::SurfaceFormatKHR& surfaceFormat
    );
    static vk::UniquePipelineLayout createVulkanPipelineLayoutUniquePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout
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
    const uint32_t m_maxNumFramesInFlight;
    vk::VertexInputBindingDescription m_vulkanVertexInputBindingDescriptions;
    std::array<vk::VertexInputAttributeDescription, 2> m_vulkanVertexInputAttributeDescriptions;
    std::vector<vk::Viewport> m_vulkanViewports;
    std::vector<vk::Rect2D> m_vulkanScissorRectangles;
    std::vector<vk::PipelineColorBlendAttachmentState> m_vulkanColorBlendAttachmentStates;
    std::vector<vk::DynamicState> m_vulkanDynamicStates;

    vk::UniqueShaderModule mp_vulkanVertexShaderModule;
    vk::UniqueShaderModule mp_vulkanFragmentShaderModule;

    std::vector<quartz::rendering::Buffer> m_uniformBuffers;
    vk::UniqueDescriptorSetLayout mp_vulkanDescriptorSetLayout;
    vk::UniqueDescriptorPool m_vulkanDescriptorPoolPtr;
    std::vector<vk::DescriptorSet> m_vulkanDescriptorSets;

    vk::UniqueRenderPass mp_vulkanRenderPass;

    vk::UniquePipelineLayout mp_vulkanPipelineLayout;
    vk::UniquePipeline mp_vulkanUniqueGraphicsPipeline;
};