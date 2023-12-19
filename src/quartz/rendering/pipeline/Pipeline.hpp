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
    class Pipeline;
}
}

/**
 * @todo 2023/12/19 Get rid of this and just use the normal camera class? We
 *   might need to get rid of some fields if we do so. We should at least move
 *   this struct to be defined in Camera.hpp. It could be convenient to also
 *   have a constructor for this class that just takes in a reference to a
 *   camera object and populates itself if we decide not to use the full
 *   camera class as the uniform buffer object
 */
struct quartz::rendering::CameraUniformBufferObject {
public: // member functions
    CameraUniformBufferObject() = default;
    CameraUniformBufferObject(
        const glm::mat4 viewMatrix_,
        const glm::mat4 projectionMatrix_
    );

private: // member variables
    UNUSED alignas(16) glm::mat4 viewMatrix;
    UNUSED alignas(16) glm::mat4 projectionMatrix;
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
        const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs
    );

    USE_LOGGER(PIPELINE);

    uint32_t getMaxNumFramesInFlight() const { return m_maxNumFramesInFlight; }
    uint32_t getCurrentInFlightFrameIndex() const { return m_currentInFlightFrameIndex; }
    const std::vector<vk::DescriptorSet>& getVulkanDescriptorSets() const { return m_vulkanDescriptorSets; }
    const vk::UniqueRenderPass& getVulkanRenderPassPtr() const { return mp_vulkanRenderPass; }
    const vk::UniquePipelineLayout& getVulkanPipelineLayoutPtr() const { return mp_vulkanPipelineLayout; }
    const vk::UniquePipeline& getVulkanGraphicsPipelinePtr() const { return mp_vulkanGraphicsPipeline; }

    void updateCameraUniformBuffer(const quartz::scene::Camera& camera);
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
        const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs
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
        const std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions,
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
    std::vector<vk::VertexInputAttributeDescription> m_vulkanVertexInputAttributeDescriptions;
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