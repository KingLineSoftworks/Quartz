#pragma once

#include <vector>

#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/LocallyMappedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"

namespace quartz {
namespace rendering {
    struct CameraUniformBufferObject;
    struct ModelUniformBufferObject;
    class Pipeline;
}
}

struct quartz::rendering::CameraUniformBufferObject {
public: // member functions
    CameraUniformBufferObject() = default;
    CameraUniformBufferObject(
        const glm::mat4 viewMatrix_,
        const glm::mat4 projectionMatrix_
    );

public: // member variables
    alignas(16) glm::mat4 viewMatrix;
    alignas(16) glm::mat4 projectionMatrix;
};

struct quartz::rendering::ModelUniformBufferObject {
public: // member functions
    ModelUniformBufferObject() = default;
    ModelUniformBufferObject(
        const glm::mat4 modelMatrix_
    );

public: // member variables
    alignas(16) glm::mat4 modelMatrix;
};

class quartz::rendering::Pipeline {
public: // member functions
    Pipeline(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window& renderingWindow,
        const uint32_t maxNumFramesInFlight
    );
    ~Pipeline();

    void reset();
    void recreate(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window& renderingWindow
    );

    void allocateVulkanDescriptorSets(
        const quartz::rendering::Device& renderingDevice,
        const std::vector<quartz::rendering::Texture>& textures
    );

    USE_LOGGER(PIPELINE);

    uint32_t getMaxNumFramesInFlight() const { return m_maxNumFramesInFlight; }
    uint32_t getCurrentInFlightFrameIndex() const { return m_currentInFlightFrameIndex; }
    const std::vector<vk::DescriptorSet>& getVulkanDescriptorSets() const { return m_vulkanDescriptorSets; }
    const vk::UniqueRenderPass& getVulkanRenderPassPtr() const { return mp_vulkanRenderPass; }
    const vk::UniquePipelineLayout& getVulkanPipelineLayoutPtr() const { return mp_vulkanPipelineLayout; }
    const vk::UniquePipeline& getVulkanGraphicsPipelinePtr() const { return mp_vulkanGraphicsPipeline; }

    void updateCameraUniformBuffer(const quartz::scene::Camera& camera);
    void updateModelUniformBuffer(const quartz::scene::Doodad& doodad);
    void updateAmbientLightUniformBuffer(const quartz::scene::AmbientLight& ambientLight);
    void updateDirectionalLightUniformBuffer(const quartz::scene::DirectionalLight& directionalLight);
    void incrementCurrentInFlightFrameIndex() { m_currentInFlightFrameIndex = (m_currentInFlightFrameIndex + 1) % m_maxNumFramesInFlight; }

private: // static functions
    static vk::UniqueShaderModule createVulkanShaderModulePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const std::string& filepath
    );

    static std::vector<quartz::rendering::LocallyMappedBuffer> createUniformBuffers(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t numBuffers
    );

    static vk::UniqueDescriptorSetLayout createVulkanDescriptorSetLayoutPtr(
        const vk::UniqueDevice& p_logicalDevice
    );

    static vk::UniqueDescriptorPool createVulkanDescriptorPoolPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t numDescriptorSets
    );

    static std::vector<vk::DescriptorSet> allocateVulkanDescriptorSets(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t maxNumFramesInFlight, // should be m_maxNumFramesInFlight
        const std::vector<quartz::rendering::LocallyMappedBuffer>& uniformBuffers,
        const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout,
        const vk::UniqueDescriptorPool& uniqueDescriptorPool,
        const std::vector<quartz::rendering::Texture>& textures
    );

    static vk::UniqueRenderPass createVulkanRenderPassPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const vk::Format& depthFormat
    );

    static vk::UniquePipelineLayout createVulkanPipelineLayoutPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout
    );

    static vk::UniquePipeline createVulkanGraphicsPipelinePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::VertexInputBindingDescription vertexInputBindingDescriptions,
        const std::array<vk::VertexInputAttributeDescription, 4> vertexInputAttributeDescriptions,
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
    uint32_t m_currentInFlightFrameIndex;
    vk::VertexInputBindingDescription m_vulkanVertexInputBindingDescriptions;
    std::array<vk::VertexInputAttributeDescription, 4> m_vulkanVertexInputAttributeDescriptions;
    std::vector<vk::Viewport> m_vulkanViewports;
    std::vector<vk::Rect2D> m_vulkanScissorRectangles;
    std::vector<vk::PipelineColorBlendAttachmentState> m_vulkanColorBlendAttachmentStates;
    std::vector<vk::DynamicState> m_vulkanDynamicStates;

    vk::UniqueShaderModule mp_vulkanVertexShaderModule;
    vk::UniqueShaderModule mp_vulkanFragmentShaderModule;

    std::vector<quartz::rendering::LocallyMappedBuffer> m_uniformBuffers;
    vk::UniqueDescriptorSetLayout mp_vulkanDescriptorSetLayout;
    vk::UniqueDescriptorPool m_vulkanDescriptorPoolPtr;
    std::vector<vk::DescriptorSet> m_vulkanDescriptorSets;

    vk::UniqueRenderPass mp_vulkanRenderPass;

    vk::UniquePipelineLayout mp_vulkanPipelineLayout;
    vk::UniquePipeline mp_vulkanGraphicsPipeline;
};