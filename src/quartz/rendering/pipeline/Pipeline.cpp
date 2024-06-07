#include <algorithm>

#include <vulkan/vulkan.hpp>

#include "util/macros.hpp"
#include "util/file_system/FileSystem.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/LocallyMappedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/rendering/model/Vertex.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

vk::UniqueShaderModule
quartz::rendering::Pipeline::createVulkanShaderModulePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const std::string& filepath
) {
    LOG_FUNCTION_CALL_TRACE(PIPELINE, "{}", filepath);

    const std::vector<char> shaderBytes = util::FileSystem::readBytesFromFile(filepath);

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo(
        {},
        shaderBytes.size(),
        reinterpret_cast<const uint32_t*>(shaderBytes.data())
    );

    vk::UniqueShaderModule p_shaderModule = p_logicalDevice->createShaderModuleUnique(shaderModuleCreateInfo);

    if (!p_shaderModule) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to create vk::ShaderModule");
    }

    return p_shaderModule;
}

std::vector<quartz::rendering::LocallyMappedBuffer>
quartz::rendering::Pipeline::createLocallyMappedBuffers(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
    const uint32_t maxNumFramesInFlight
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "{} frames in flight, {} buffer infos", maxNumFramesInFlight, uniformBufferInfos.size());

    LOG_TRACE(PIPELINE, "{} frames in flight", maxNumFramesInFlight);
    LOG_TRACE(PIPELINE, "{} buffer infos", uniformBufferInfos.size());
    LOG_TRACE(PIPELINE, "{} total locally mapped buffers", maxNumFramesInFlight * uniformBufferInfos.size());
    LOG_TRACE(PIPELINE, "because we need one buffer per uniform buffer per frame in flight");

    std::vector<quartz::rendering::LocallyMappedBuffer> buffers;

    for (uint32_t i = 0; i < maxNumFramesInFlight; ++i) {
        for (const quartz::rendering::UniformBufferInfo& uniformBufferInfo : uniformBufferInfos) {
            buffers.emplace_back(
                renderingDevice,
                uniformBufferInfo.getLocallyMappedBufferSize(),
                uniformBufferInfo.getLocallyMappedBufferVulkanUsageFlags(),
                uniformBufferInfo.getLocallyMappedBufferVulkanPropertyFlags()
            );
        }
    }

    LOG_TRACE(PIPELINE, "Created {} locally mapped buffers", buffers.size());

    if (buffers.size() != maxNumFramesInFlight * uniformBufferInfos.size()) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Created {} locally mapped buffers instead of expected {}", buffers.size(), maxNumFramesInFlight * uniformBufferInfos.size());
    }

    return buffers;
}

vk::UniqueDescriptorSetLayout
quartz::rendering::Pipeline::createVulkanDescriptorSetLayoutPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
    const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
    const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;

    LOG_TRACE(PIPELINE, "Creating {} layout bindings for uniform buffers", uniformBufferInfos.size());
    for (const quartz::rendering::UniformBufferInfo& uniformBufferInfo : uniformBufferInfos) {
        vk::DescriptorSetLayoutBinding uniformBufferLayoutBinding(
            uniformBufferInfo.getBindingLocation(),
            uniformBufferInfo.getVulkanDescriptorType(),
            uniformBufferInfo.getDescriptorCount(),
            uniformBufferInfo.getVulkanShaderStageFlags(),
            {}
        );
        layoutBindings.push_back(uniformBufferLayoutBinding);
    }

    LOG_TRACE(PIPELINE, "{}reating layout binding for uniform sampler", o_uniformSamplerInfo ? "C" : "Not c");
    if (o_uniformSamplerInfo) {
        vk::DescriptorSetLayoutBinding samplerLayoutBinding(
            o_uniformSamplerInfo->getBindingLocation(),
            o_uniformSamplerInfo->getVulkanDescriptorType(),
            o_uniformSamplerInfo->getDescriptorCount(),
            o_uniformSamplerInfo->getVulkanShaderStageFlags(),
            {}
        );
        layoutBindings.push_back(samplerLayoutBinding);
    }

    LOG_TRACE(PIPELINE, "{}reating layout binding for uniform texture array", o_uniformTextureArrayInfo ? "C" : "Not c");
    if (o_uniformTextureArrayInfo) {
        vk::DescriptorSetLayoutBinding textureArrayLayoutBinding(
            o_uniformTextureArrayInfo->getBindingLocation(),
            o_uniformTextureArrayInfo->getVulkanDescriptorType(),
            o_uniformTextureArrayInfo->getDescriptorCount(),
            o_uniformTextureArrayInfo->getVulkanShaderStageFlags(),
            {}
        );
        layoutBindings.push_back(textureArrayLayoutBinding);
    }

    LOG_TRACE(PIPELINE, "Using {} layout bindings", layoutBindings.size());

    vk::DescriptorSetLayoutCreateInfo layoutCreateInfo({}, layoutBindings);

    vk::UniqueDescriptorSetLayout p_descriptorSetLayout = p_logicalDevice->createDescriptorSetLayoutUnique(layoutCreateInfo);

    if (!p_descriptorSetLayout) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to create vk::DescriptorSetLayout");
    }

    return p_descriptorSetLayout;
}

vk::UniqueDescriptorPool
quartz::rendering::Pipeline::createVulkanDescriptorPoolPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
    const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
    const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo,
    const uint32_t numDescriptorSets /** should be the maximum number of frames in flight */
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "{} descriptor sets", numDescriptorSets);

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes;

    LOG_TRACE(PIPELINE, "Creating {} descriptor pool sizes for uniform buffers", uniformBufferInfos.size());
    for (const quartz::rendering::UniformBufferInfo& uniformBufferInfo : uniformBufferInfos) {
        vk::DescriptorPoolSize uniformBufferPoolSize(
            uniformBufferInfo.getVulkanDescriptorType(),
            numDescriptorSets * uniformBufferInfo.getDescriptorCount()
        );
        descriptorPoolSizes.push_back(uniformBufferPoolSize);
    }

    LOG_TRACE(PIPELINE, "{}reating descriptor pool size for uniform sampler", o_uniformSamplerInfo ? "C" : "Not c");
    if (o_uniformSamplerInfo) {
        vk::DescriptorPoolSize samplerPoolSize(
            o_uniformSamplerInfo->getVulkanDescriptorType(),
            numDescriptorSets * o_uniformSamplerInfo->getDescriptorCount()
        );
        descriptorPoolSizes.push_back(samplerPoolSize);
    }

    LOG_TRACE(PIPELINE, "{}reating descriptor pool size for uniform texture array", o_uniformTextureArrayInfo ? "C" : "Not c");
    if (o_uniformTextureArrayInfo) {
        vk::DescriptorPoolSize textureArrayPoolSize(
            o_uniformTextureArrayInfo->getVulkanDescriptorType(),
            numDescriptorSets * o_uniformTextureArrayInfo->getDescriptorCount()
        );
        descriptorPoolSizes.push_back(textureArrayPoolSize);
    }

    LOG_TRACE(PIPELINE, "Using {} pool sizes", descriptorPoolSizes.size());
    LOG_TRACE(PIPELINE, "Using maximum of {} descriptor sets", numDescriptorSets);

    vk::DescriptorPoolCreateInfo poolCreateInfo(
        {},
        /** @brief how many descriptor sets in total can be allocated from this pool */
        numDescriptorSets,
        /** @brief the descriptors that will be allocated (not in a single descriptor set, but in total) from this pool */
        descriptorPoolSizes
    );

    vk::UniqueDescriptorPool uniqueDescriptorPool = p_logicalDevice->createDescriptorPoolUnique(poolCreateInfo);

    if (!uniqueDescriptorPool) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to create vk::DescriptorPool");
    }

    return uniqueDescriptorPool;
}

std::vector<vk::DescriptorSet>
quartz::rendering::Pipeline::allocateVulkanDescriptorSets(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t maxNumFramesInFlight,
    const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout,
    const vk::UniqueDescriptorPool& p_descriptorPool
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "{} frames in flight", maxNumFramesInFlight);

    std::vector<const vk::DescriptorSetLayout> descriptorSetLayouts(
        maxNumFramesInFlight,
        *p_descriptorSetLayout
    );

    vk::DescriptorSetAllocateInfo allocateInfo(
        *p_descriptorPool,
        descriptorSetLayouts.size(),
        descriptorSetLayouts.data()
    );

    LOG_TRACE(PIPELINE, "Attempting to allocate {} descriptor sets", descriptorSetLayouts.size());
    std::vector<vk::DescriptorSet> descriptorSets = p_logicalDevice->allocateDescriptorSets(allocateInfo);

    if (descriptorSets.size() != descriptorSetLayouts.size()) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Allocated {} vk::DescriptorSet(s) instead of requested amount: {}", descriptorSets.size(), descriptorSetLayouts.size());
    } else if (descriptorSets.size() != maxNumFramesInFlight) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Allocated {} vk::DescriptorSet(s) count does not match the max number of frames in flight: {}", descriptorSets.size(), maxNumFramesInFlight);
    }
    LOG_TRACE(PIPELINE, "Successfully allocated {} vk::DescriptorSet(s)", descriptorSets.size());

    for (uint32_t i = 0; i < descriptorSets.size(); ++i) {
        if (!descriptorSets[i]) {
            LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to allocate vk::DescriptorSet {}", i);
        }
        LOG_TRACE(PIPELINE, "Successfully allocated vk::DescriptorSet {}", i);
    }

    return descriptorSets;
}

void
quartz::rendering::Pipeline::updateUniformBufferDescriptorSets(
    const vk::UniqueDevice& p_logicalDevice,
    const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
    const std::vector<quartz::rendering::LocallyMappedBuffer>& locallyMappedBuffers,
    const std::vector<vk::DescriptorSet>& descriptorSets
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    for (uint32_t i = 0; i < descriptorSets.size(); ++i) {
        LOG_TRACE(PIPELINE, "Updating descriptor set {}", i);
        const vk::DescriptorSet& descriptorSet = descriptorSets[i];

        std::vector<vk::WriteDescriptorSet> writeDescriptorSets;

        LOG_TRACE(PIPELINE, "  Using {} uniform buffer infos", uniformBufferInfos.size());
        std::vector<vk::DescriptorBufferInfo> uniformBufferDescriptorInfos;
        uniformBufferDescriptorInfos.reserve(uniformBufferInfos.size()); // Everything breaks if we don't reserve the space ahead of time

        for (uint32_t j = 0; j < uniformBufferInfos.size(); ++j) {
            const uint32_t locallyMappedBufferIndex = i * uniformBufferInfos.size() + j;
            LOG_TRACE(PIPELINE, "    Using uniform buffer info {}", j);
            LOG_TRACE(PIPELINE, "      destination binding    = {}", uniformBufferInfos[j].getBindingLocation());
            LOG_TRACE(PIPELINE, "      descriptor count       = {}", uniformBufferInfos[j].getDescriptorCount());
            LOG_TRACE(PIPELINE, "      object stride in bytes = {}", uniformBufferInfos[j].getObjectStrideBytes());
            LOG_TRACE(PIPELINE, "      vulkan descriptor type = {}", quartz::rendering::VulkanUtil::toString(uniformBufferInfos[j].getVulkanDescriptorType()));

            uniformBufferDescriptorInfos.emplace_back(
                *(locallyMappedBuffers[locallyMappedBufferIndex].getVulkanLogicalBufferPtr()),
                0,
                uniformBufferInfos[j].getObjectStrideBytes()
            );
            vk::WriteDescriptorSet writeDescriptorSet(
                descriptorSet,
                uniformBufferInfos[j].getBindingLocation(),
                0,
                uniformBufferInfos[j].getDescriptorCount(),
                uniformBufferInfos[j].getVulkanDescriptorType(),
                {},
                &uniformBufferDescriptorInfos.back(),
                {}
            );
            writeDescriptorSets.push_back(writeDescriptorSet);
        }

        LOG_TRACE(PIPELINE, "  Updating descriptor set {} with {} descriptor writes", i, writeDescriptorSets.size());

        /**
         * @todo 2024/06/07 What happens when writeDescriptorSets is empty and we give 0 as the count to this function?
         *    Best case - it handles 0 gracefully and just does not do anything
         *    Worst case - it dies screaming
         *      We might need to put a gate at the beginning of this function that ensures uniformBufferInfos is not empty
         */
        p_logicalDevice->updateDescriptorSets(
            writeDescriptorSets.size(),
            writeDescriptorSets.data(),
            0,
            nullptr
        );
    }
}

void
quartz::rendering::Pipeline::updateUniformSamplerDescriptorSets(
    const vk::UniqueDevice& p_logicalDevice,
    const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
    const vk::UniqueSampler& p_sampler,
    const std::vector<vk::DescriptorSet>& descriptorSets
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    if (!o_uniformSamplerInfo) {
        LOG_TRACE(PIPELINE, "No uniform sampler info. Not updating descriptor sets for it");
        return;
    }

    for (uint32_t i = 0; i < descriptorSets.size(); ++i) {
        LOG_TRACE(PIPELINE, "Updating descriptor set {}", i);
        const vk::DescriptorSet& descriptorSet = descriptorSets[i];

        LOG_TRACE(PIPELINE, "  Using uniform sampler info");
        LOG_TRACE(PIPELINE, "    destination binding    = {}", o_uniformSamplerInfo->getBindingLocation());
        LOG_TRACE(PIPELINE, "    descriptor count       = {}", o_uniformSamplerInfo->getDescriptorCount());
        LOG_TRACE(PIPELINE, "    vulkan descriptor type = {}", quartz::rendering::VulkanUtil::toString(o_uniformSamplerInfo->getVulkanDescriptorType()));
        vk::DescriptorImageInfo samplerImageInfo(
            *(p_sampler),
            {},
            {}
        );
        vk::WriteDescriptorSet writeDescriptorSet(
            descriptorSet,
            o_uniformSamplerInfo->getBindingLocation(),
            0,
            o_uniformSamplerInfo->getDescriptorCount(),
            o_uniformSamplerInfo->getVulkanDescriptorType(),
            &samplerImageInfo,
            {},
            {}
        );

        LOG_TRACE(PIPELINE, "  Updating descriptor set {} with 1 descriptor writes", i);

        p_logicalDevice->updateDescriptorSets(
            1,
            &writeDescriptorSet,
            0,
            nullptr
        );
    }
}

void quartz::rendering::Pipeline::updateUniformTextureArrayDescriptorSets(
    const vk::UniqueDevice& p_logicalDevice,
    const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo,
    const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs,
    const std::vector<vk::DescriptorSet>& descriptorSets
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    if (!o_uniformTextureArrayInfo) {
        LOG_TRACE(PIPELINE, "No uniform texture array info. Not updating descriptor sets for it");
        return;
    }

    for (uint32_t i = 0; i < descriptorSets.size(); ++i) {
        LOG_TRACE(PIPELINE, "Updating descriptor set {}", i);
        const vk::DescriptorSet& descriptorSet = descriptorSets[i];

        const uint32_t numTexturesToUse = std::min<uint32_t>(o_uniformTextureArrayInfo->getDescriptorCount(), texturePtrs.size());
        LOG_TRACE(PIPELINE, "  Using texture array sampler info");
        LOG_TRACE(PIPELINE, "    destination binding    = {}", o_uniformTextureArrayInfo->getBindingLocation());
        LOG_TRACE(PIPELINE, "    descriptor count       = {}", o_uniformTextureArrayInfo->getDescriptorCount());
        LOG_TRACE(PIPELINE, "    vulkan descriptor type = {}", quartz::rendering::VulkanUtil::toString(o_uniformTextureArrayInfo->getVulkanDescriptorType()));
        LOG_TRACE(PIPELINE, "    given       {} textures", texturePtrs.size());
        LOG_TRACE(PIPELINE, "    allocating  {} textures", o_uniformTextureArrayInfo->getDescriptorCount());
        LOG_TRACE(PIPELINE, "    using       {} textures", numTexturesToUse);
        std::vector<vk::DescriptorImageInfo> textureImageInfos(
            o_uniformTextureArrayInfo->getDescriptorCount(),
            {
                nullptr,
                *(texturePtrs[0]->getVulkanImageViewPtr()),
                vk::ImageLayout::eShaderReadOnlyOptimal
            }
        );
        for (uint32_t j = 0; j < texturePtrs.size(); ++j) {
            textureImageInfos[j] = vk::DescriptorImageInfo(
                nullptr,
                *(texturePtrs[j]->getVulkanImageViewPtr()),
                vk::ImageLayout::eShaderReadOnlyOptimal
            );
        }
        vk::WriteDescriptorSet writeDescriptorSet(
            descriptorSet,
            o_uniformTextureArrayInfo->getBindingLocation(),
            0,
            o_uniformTextureArrayInfo->getDescriptorCount(),
            o_uniformTextureArrayInfo->getVulkanDescriptorType(),
            textureImageInfos.data(),
            {},
            {}
        );

        LOG_TRACE(PIPELINE, "  Updating descriptor set {} with 1 descriptor writes", i);

        p_logicalDevice->updateDescriptorSets(
            1,
            &writeDescriptorSet,
            0,
            nullptr
        );
    }
}

void
quartz::rendering::Pipeline::updateVulkanDescriptorSets(
    const vk::UniqueDevice& p_logicalDevice,
    const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
    const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
    const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo,
    const std::vector<quartz::rendering::LocallyMappedBuffer>& locallyMappedBuffers,
    const vk::UniqueSampler& p_sampler,
    const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs,
    const std::vector<vk::DescriptorSet>& descriptorSets
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    quartz::rendering::Pipeline::updateUniformBufferDescriptorSets(
        p_logicalDevice,
        uniformBufferInfos,
        locallyMappedBuffers,
        descriptorSets
    );

    quartz::rendering::Pipeline::updateUniformSamplerDescriptorSets(
        p_logicalDevice,
        o_uniformSamplerInfo,
        p_sampler,
        descriptorSets
    );

    quartz::rendering::Pipeline::updateUniformTextureArrayDescriptorSets(
        p_logicalDevice,
        o_uniformTextureArrayInfo,
        texturePtrs,
        descriptorSets
    );
}

vk::UniquePipelineLayout
quartz::rendering::Pipeline::createVulkanPipelineLayoutPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout
) {
    LOG_FUNCTION_CALL_TRACE(PIPELINE, "");

    vk::PushConstantRange vertexPushConstantRange(
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(glm::mat4)
    );

    /** @brief 2024/05/16 This isn't actually used for anything and is just here as an example of using a push constant in the fragment shader */
    vk::PushConstantRange fragmentPushConstantRange(
        vk::ShaderStageFlagBits::eFragment,
        sizeof(glm::mat4),
        sizeof(uint32_t)
    );

    std::vector<vk::PushConstantRange> pushConstantRanges = {
        vertexPushConstantRange,
        fragmentPushConstantRange
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        {},
        *p_descriptorSetLayout,
        pushConstantRanges
    );

    vk::UniquePipelineLayout p_pipelineLayout = p_logicalDevice->createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    if (!p_pipelineLayout) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to create vk::PipelineLayout");
    }

    return p_pipelineLayout;
}

vk::UniquePipeline
quartz::rendering::Pipeline::createVulkanGraphicsPipelinePtr(
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
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    // ----- shader stage tings ----- //

    LOG_TRACE(PIPELINE, "Creating vk::PipelineShaderStageCreateInfo(s)");
    std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos = {
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eVertex,
            *p_vertexShaderModule,
            "main"
        ),
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eFragment,
            *p_fragmentShaderModule,
            "main"
        )
    };

    // ----- vertex input tings ----- //

    LOG_TRACE(PIPELINE, "Creating vk::PipelineVertexInputStateCreateInfo");
    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo(
        {},
        vertexInputBindingDescriptions,
        vertexInputAttributeDescriptions
    );

    LOG_TRACE(PIPELINE, "Creating vk::PipelineInputAssemblyStateCreateInfo");
    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
        {},
        vk::PrimitiveTopology::eTriangleList,
        false
    );

    // ----- tessellation tings ----- //

    vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo = vk::PipelineTessellationStateCreateInfo();

    // ----- viewport and scissor tings ----- //

    LOG_TRACE(PIPELINE, "Using {} viewports", viewports.size());
    for (UNUSED const vk::Viewport& viewport : viewports) {
        LOG_TRACE(PIPELINE, "  - {} x {}", viewport.width, viewport.height);
    }
    LOG_TRACE(PIPELINE, "Using {} scissor rectangles", scissorRectangles.size());
    for (UNUSED const vk::Rect2D& scissorRectangle : scissorRectangles) {
        LOG_TRACE(PIPELINE, "  - {} , {}",scissorRectangle.offset.x, scissorRectangle.offset.y);
    }

    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo(
        {},
        viewports,
        scissorRectangles
    );

    // ----- rasterizer tings ----- //

    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo(
        {},
        false,
        false,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eCounterClockwise,
        false,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    // ----- multisample tings ----- //

    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo(
        {},
        vk::SampleCountFlagBits::e1,
        false,
        1.0f,
        nullptr,
        false,
        false
    );

    // ----- depth stencil tings ----- //

    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
        {},
        true,
        true,
        vk::CompareOp::eLess,
        false, /// @todo 2023/11/01 enable with phys dev features
        false,
        {},
        {},
        0.0f,
        1.0f
    );

    // ----- color blend tings ----- //

    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo(
        {},
        false,
        vk::LogicOp::eCopy,
        colorBlendAttachmentStates,
        { 0.0f, 0.0f, 0.0f, 0.0f}
    );

    // ----- dynamic state tings ----- //

    LOG_TRACE(PIPELINE, "Using {} dynamic states", dynamicStates.size());
    for (UNUSED const vk::DynamicState& dynamicState : dynamicStates) {
        LOG_TRACE(PIPELINE, "  - {}", static_cast<uint32_t>(dynamicState));
    }

    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo(
        {},
        dynamicStates
    );

    // ----- actually creating the graphics pipeline ----- //

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        {},
        pipelineShaderStageCreateInfos,
        &pipelineVertexInputStateCreateInfo,
        &pipelineInputAssemblyStateCreateInfo,
        &pipelineTessellationStateCreateInfo,
        &pipelineViewportStateCreateInfo,
        &pipelineRasterizationStateCreateInfo,
        &pipelineMultisampleStateCreateInfo,
        &pipelineDepthStencilStateCreateInfo,
        &pipelineColorBlendStateCreateInfo,
        &pipelineDynamicStateCreateInfo,
        *p_pipelineLayout,
        *p_renderPass,
        0,
        VK_NULL_HANDLE,
        -1
    );

    vk::ResultValue<vk::UniquePipeline> graphicsPipelineCreationResult = p_logicalDevice->createGraphicsPipelineUnique(
        VK_NULL_HANDLE,
        graphicsPipelineCreateInfo
    );

    if (graphicsPipelineCreationResult.result != vk::Result::eSuccess) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to create vk::Pipeline");
    }

    return std::move(graphicsPipelineCreationResult.value);
}

quartz::rendering::Pipeline::Pipeline(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow,
    const quartz::rendering::RenderPass& renderingRenderPass,
    const std::string& compiledVertexShaderFilepath,
    const std::string& compiledFragmentShaderFilepath,
    const uint32_t maxNumFramesInFlight,
    const std::vector<quartz::rendering::UniformBufferInfo>& uniformBufferInfos,
    const std::optional<quartz::rendering::UniformSamplerInfo>& o_uniformSamplerInfo,
    const std::optional<quartz::rendering::UniformTextureArrayInfo>& o_uniformTextureArrayInfo
) :
    m_vulkanVertexInputBindingDescriptions(
        quartz::rendering::Vertex::getVulkanVertexInputBindingDescription()
    ),
    m_vulkanVertexInputAttributeDescriptions(
        quartz::rendering::Vertex::getVulkanVertexInputAttributeDescriptions()
    ),
    m_vulkanViewports({
        vk::Viewport(
            0.0f,
            0.0f,
            static_cast<float>(renderingWindow.getVulkanExtent().width),
            static_cast<float>(renderingWindow.getVulkanExtent().height),
            0.0f,
            1.0f
        )
    }),
    m_vulkanScissorRectangles({
        vk::Rect2D(
            vk::Offset2D(0.0f, 0.0f),
            renderingWindow.getVulkanExtent()
        )
    }),
    m_vulkanColorBlendAttachmentStates({
        vk::PipelineColorBlendAttachmentState(
            true,
            vk::BlendFactor::eSrcAlpha,
            vk::BlendFactor::eOneMinusSrcAlpha,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            {
                vk::ColorComponentFlagBits::eR |
                vk::ColorComponentFlagBits::eG |
                vk::ColorComponentFlagBits::eB |
                vk::ColorComponentFlagBits::eA
            }
        )
    }),
    m_vulkanDynamicStates({
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    }),
    mp_vulkanVertexShaderModule(
        quartz::rendering::Pipeline::createVulkanShaderModulePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            compiledVertexShaderFilepath
        )
    ),
    mp_vulkanFragmentShaderModule(
        quartz::rendering::Pipeline::createVulkanShaderModulePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            compiledFragmentShaderFilepath
        )
    ),
    m_uniformBufferInfos(uniformBufferInfos),
    mo_uniformSamplerInfo(o_uniformSamplerInfo),
    mo_uniformTextureArrayInfo(o_uniformTextureArrayInfo),
    m_locallyMappedBuffers(
        quartz::rendering::Pipeline::createLocallyMappedBuffers(
            renderingDevice,
            m_uniformBufferInfos,
            maxNumFramesInFlight
        )
    ),
    mp_vulkanDescriptorSetLayout(
        quartz::rendering::Pipeline::createVulkanDescriptorSetLayoutPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_uniformBufferInfos,
            mo_uniformSamplerInfo,
            mo_uniformTextureArrayInfo
        )
    ),
    m_vulkanDescriptorPoolPtr(
        quartz::rendering::Pipeline::createVulkanDescriptorPoolPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_uniformBufferInfos,
            mo_uniformSamplerInfo,
            mo_uniformTextureArrayInfo,
            maxNumFramesInFlight
        )
    ),
    m_vulkanDescriptorSets(
        quartz::rendering::Pipeline::allocateVulkanDescriptorSets(
            renderingDevice.getVulkanLogicalDevicePtr(),
            maxNumFramesInFlight,
            mp_vulkanDescriptorSetLayout,
            m_vulkanDescriptorPoolPtr
        )
    ),
    mp_vulkanPipelineLayout(
        quartz::rendering::Pipeline::createVulkanPipelineLayoutPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            mp_vulkanDescriptorSetLayout
        )
    ),
    mp_vulkanGraphicsPipeline(
        quartz::rendering::Pipeline::createVulkanGraphicsPipelinePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_vulkanVertexInputBindingDescriptions,
            m_vulkanVertexInputAttributeDescriptions,
            m_vulkanViewports,
            m_vulkanScissorRectangles,
            m_vulkanColorBlendAttachmentStates,
            m_vulkanDynamicStates,
            mp_vulkanVertexShaderModule,
            mp_vulkanFragmentShaderModule,
            mp_vulkanPipelineLayout,
            renderingRenderPass.getVulkanRenderPassPtr()
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Pipeline::~Pipeline() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::rendering::Pipeline::reset() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanGraphicsPipeline.reset();
    mp_vulkanPipelineLayout.reset();
}

void
quartz::rendering::Pipeline::recreate(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::RenderPass& renderingRenderPass
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanPipelineLayout = quartz::rendering::Pipeline::createVulkanPipelineLayoutPtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        mp_vulkanDescriptorSetLayout
    );
    mp_vulkanGraphicsPipeline = quartz::rendering::Pipeline::createVulkanGraphicsPipelinePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_vulkanVertexInputBindingDescriptions,
        m_vulkanVertexInputAttributeDescriptions,
        m_vulkanViewports,
        m_vulkanScissorRectangles,
        m_vulkanColorBlendAttachmentStates,
        m_vulkanDynamicStates,
        mp_vulkanVertexShaderModule,
        mp_vulkanFragmentShaderModule,
        mp_vulkanPipelineLayout,
        renderingRenderPass.getVulkanRenderPassPtr()
    );
}

void
quartz::rendering::Pipeline::updateVulkanDescriptorSets(
    const quartz::rendering::Device& renderingDevice,
    const vk::UniqueSampler& p_sampler,
    const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    quartz::rendering::Pipeline::updateVulkanDescriptorSets(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_uniformBufferInfos,
        mo_uniformSamplerInfo,
        mo_uniformTextureArrayInfo,
        m_locallyMappedBuffers,
        p_sampler,
        texturePtrs,
        m_vulkanDescriptorSets
    );
}

void
quartz::rendering::Pipeline::updateUniformBuffer(
    const uint32_t currentInFlightFrameIndex,
    const uint32_t uniformIndex,
    void* p_dataToCopy
) {
    const quartz::rendering::UniformBufferInfo& uniformBufferInfo = m_uniformBufferInfos[uniformIndex];

    const uint32_t locallyMappedBufferIndex = currentInFlightFrameIndex * m_uniformBufferInfos.size() + uniformIndex;

    memcpy(
        m_locallyMappedBuffers[locallyMappedBufferIndex].getMappedLocalMemoryPtr(),
        p_dataToCopy,
        uniformBufferInfo.getLocallyMappedBufferSize()
    );
}
