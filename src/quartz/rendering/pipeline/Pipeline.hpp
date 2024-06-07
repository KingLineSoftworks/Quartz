#pragma once

#include <vector>
#include <optional>

#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/LocallyMappedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"
#include "quartz/rendering/pipeline/UniformSamplerInfo.hpp"
#include "quartz/rendering/pipeline/UniformTextureArrayInfo.hpp"
#include "quartz/rendering/render_pass/RenderPass.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"
#include "quartz/scene/light/PointLight.hpp"
#include "quartz/scene/light/SpotLight.hpp"

namespace quartz {
namespace rendering {
    class Pipeline;
}
}

class quartz::rendering::Pipeline {
public: // member functions
    /**
     * @brief This takes an r value to the vector of uniform buffer infos so we don't have to be copying the locally mapped buffers around,
     *    but instead just move them around
     * @todo 2024/06/06 Make the UniformBufferInfo class just be a POD and not contain the actually locally mapped buffer.
     *    We could have the pipeline manage the locally mapped buffers directly?
     *    We could have a custom UniformBuffer class that contains the locally mapped buffers and is constructed within the pipeline
     *       when we give the UniformBufferInfo POD?
     */
    Pipeline(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window& renderingWindow,
        const quartz::rendering::RenderPass& renderingRenderPass,
        const std::string& compiledVertexShaderFilepath,
        const std::string& compiledFragmentShaderFilepath,
        const uint32_t maxNumFramesInFlight,
        const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
        const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
        const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo
    );
    ~Pipeline();

    void reset();
    void recreate(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::RenderPass& renderingRenderPass
    );

    void updateVulkanDescriptorSets(
        const quartz::rendering::Device& renderingDevice,
        const vk::UniqueSampler& p_sampler,
        const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs
    );

    USE_LOGGER(PIPELINE);

    const std::vector<vk::Viewport>& getVulkanViewports() const { return m_vulkanViewports; }
    const std::vector<vk::Rect2D>& getVulkanScissorRectangles() const { return m_vulkanScissorRectangles; }
    const std::vector<vk::DescriptorSet>& getVulkanDescriptorSets() const { return m_vulkanDescriptorSets; }
    const vk::UniquePipelineLayout& getVulkanPipelineLayoutPtr() const { return mp_vulkanPipelineLayout; }
    const vk::UniquePipeline& getVulkanGraphicsPipelinePtr() const { return mp_vulkanGraphicsPipeline; }

    void updateCameraUniformBuffer(const quartz::scene::Camera& camera, const uint32_t currentInFlightFrameIndex);
    void updateAmbientLightUniformBuffer(const quartz::scene::AmbientLight& ambientLight, const uint32_t currentInFlightFrameIndex);
    void updateDirectionalLightUniformBuffer(const quartz::scene::DirectionalLight& directionalLight, const uint32_t currentInFlightFrameIndex);
    void updatePointLightUniformBuffer(const std::vector<quartz::scene::PointLight>& pointLights, const uint32_t currentInFlightFrameIndex);
    void updateSpotLightUniformBuffer(const std::vector<quartz::scene::SpotLight>& spotLights, const uint32_t currentInFlightFrameIndex);
    void updateMaterialArrayUniformBuffer(const uint32_t minUniformBufferOffsetAlignment, const uint32_t currentInFlightFrameIndex);

private: // static functions
    static vk::UniqueShaderModule createVulkanShaderModulePtr(
        const vk::UniqueDevice& p_logicalDevice,
        const std::string& filepath
    );
    static std::vector<quartz::rendering::LocallyMappedBuffer> createLocallyMappedBuffers(
        const quartz::rendering::Device& renderingDevice,
        const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
        const uint32_t maxNumFramesInFlight
    );
    static vk::UniqueDescriptorSetLayout createVulkanDescriptorSetLayoutPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
        const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
        const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo
    );
    static vk::UniqueDescriptorPool createVulkanDescriptorPoolPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
        const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
        const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo,
        const uint32_t numDescriptorSets
    );
    static std::vector<vk::DescriptorSet> allocateVulkanDescriptorSets(
        const vk::UniqueDevice& p_logicalDevice,
        const uint32_t maxNumFramesInFlight,
        const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout,
        const vk::UniqueDescriptorPool& p_descriptorPool
    );
    static void updateUniformBufferDescriptorSets(
        const vk::UniqueDevice& p_logicalDevice,
        const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
        const std::vector<quartz::rendering::LocallyMappedBuffer>& locallyMappedBuffers,
        const std::vector<vk::DescriptorSet>& descriptorSets
    );
    static void updateUniformSamplerDescriptorSets(
        const vk::UniqueDevice& p_logicalDevice,
        const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
        const vk::UniqueSampler& p_sampler,
        const std::vector<vk::DescriptorSet>& descriptorSets
    );
    static void updateUniformTextureArrayDescriptorSets(
        const vk::UniqueDevice& p_logicalDevice,
        const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo,
        const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs,
        const std::vector<vk::DescriptorSet>& descriptorSets
    );
    static void updateVulkanDescriptorSets(
        const vk::UniqueDevice& p_logicalDevice,
        const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
        const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
        const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo,
        const std::vector<quartz::rendering::LocallyMappedBuffer>& locallyMappedBuffers,
        const vk::UniqueSampler& p_sampler,
        const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs,
        const std::vector<vk::DescriptorSet>& descriptorSets
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
    vk::VertexInputBindingDescription m_vulkanVertexInputBindingDescriptions;
    std::vector<vk::VertexInputAttributeDescription> m_vulkanVertexInputAttributeDescriptions;
    std::vector<vk::Viewport> m_vulkanViewports;
    std::vector<vk::Rect2D> m_vulkanScissorRectangles;
    std::vector<vk::PipelineColorBlendAttachmentState> m_vulkanColorBlendAttachmentStates;
    std::vector<vk::DynamicState> m_vulkanDynamicStates;

    vk::UniqueShaderModule mp_vulkanVertexShaderModule;
    vk::UniqueShaderModule mp_vulkanFragmentShaderModule;

    std::vector<quartz::rendering::UniformBufferInfo> m_uniformBufferInfos;
    std::optional<quartz::rendering::UniformSamplerInfo> mo_uniformSamplerInfo;
    std::optional<quartz::rendering::UniformTextureArrayInfo> mo_uniformTextureArrayInfo;

    std::vector<quartz::rendering::LocallyMappedBuffer> m_locallyMappedBuffers;
    vk::UniqueDescriptorSetLayout mp_vulkanDescriptorSetLayout;
    vk::UniqueDescriptorPool m_vulkanDescriptorPoolPtr; /** @todo 2024/06/07 Do we need to track this? It is only used when allocating descriptor sets */
    std::vector<vk::DescriptorSet> m_vulkanDescriptorSets;

    vk::UniquePipelineLayout mp_vulkanPipelineLayout;
    vk::UniquePipeline mp_vulkanGraphicsPipeline;
};