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

constexpr uint32_t NUM_UNIQUE_UNIFORM_BUFFERS = 4;

/**
 * @todo 2024/05/11 Just make the member variables of the camera aligned and push that
 *   instead of creating a copy into this weird little struct
 */
quartz::rendering::CameraUniformBufferObject::CameraUniformBufferObject(
    const glm::mat4 viewMatrix_,
    const glm::mat4 projectionMatrix_
) :
    viewMatrix(viewMatrix_),
    projectionMatrix(projectionMatrix_)
{}

quartz::rendering::MaterialUniformBufferObject::MaterialUniformBufferObject(
    const uint32_t baseColorTextureMasterIndex_,
    const uint32_t metallicRoughnessTextureMasterIndex_,
    const uint32_t normalTextureMasterIndex_,
    const uint32_t emissionTextureMasterIndex_,
    const uint32_t occlusionTextureMasterIndex_,
    const glm::vec4& baseColorFactor_,
    const glm::vec3& emissiveFactor_,
    const float metallicFactor_,
    const float roughnessFactor_,
    const uint32_t alphaMode_,
    const float alphaCutoff_,
    const bool doubleSided_
) :
    baseColorTextureMasterIndex(baseColorTextureMasterIndex_),
    metallicRoughnessTextureMasterIndex(metallicRoughnessTextureMasterIndex_),
    normalTextureMasterIndex(normalTextureMasterIndex_),
    emissionTextureMasterIndex(emissionTextureMasterIndex_),
    occlusionTextureMasterIndex(occlusionTextureMasterIndex_),
    baseColorFactor(baseColorFactor_),
    emissiveFactor(emissiveFactor_),
    metallicFactor(metallicFactor_),
    roughnessFactor(roughnessFactor_),
    alphaMode(alphaMode_),
    alphaCutoff(alphaCutoff_),
    doubleSided(doubleSided_)
{}

vk::UniqueShaderModule
quartz::rendering::Pipeline::createVulkanShaderModulePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "{}", filepath);

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
quartz::rendering::Pipeline::createUniformBuffers(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t numBuffers
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "{} buffers", numBuffers);

    std::vector<quartz::rendering::LocallyMappedBuffer> buffers;

    for (uint32_t i = 0; i < numBuffers; ++i) {
        LOG_SCOPE_CHANGE_TRACE(PIPELINE);
        LOG_TRACE(PIPELINE, "Creating buffers for frame {}", i);

        LOG_TRACE(PIPELINE, "Creating camera buffer {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            sizeof(quartz::rendering::CameraUniformBufferObject),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        LOG_TRACE(PIPELINE, "Creating ambient light buffer {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            sizeof(quartz::scene::AmbientLight),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        LOG_TRACE(PIPELINE, "Creating directional light buffer {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            sizeof(quartz::scene::DirectionalLight),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        const uint32_t minUniformBufferOffsetAlignment = renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment;
        const uint32_t materialByteStride = minUniformBufferOffsetAlignment > 0 ?
            (sizeof(quartz::rendering::MaterialUniformBufferObject) + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1) :
            sizeof(quartz::rendering::MaterialUniformBufferObject);
        LOG_TRACE(PIPELINE, "Creating material uniform buffer array {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            materialByteStride * QUARTZ_MAX_NUMBER_MATERIALS,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible
        );
    }

    return buffers;
}

vk::UniqueDescriptorSetLayout
quartz::rendering::Pipeline::createVulkanDescriptorSetLayoutPtr(
    const vk::UniqueDevice& p_logicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    vk::DescriptorSetLayoutBinding cameraUniformBufferLayoutBinding(
        0,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eVertex,
        {}
    );

    vk::DescriptorSetLayoutBinding ambientLightLayoutBinding(
        1,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding directionalLightLayoutBinding(
        2,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding baseColorTextureSamplerLayoutBinding(
        3,
        vk::DescriptorType::eSampler,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding baseColorTexturesLayoutBinding(
        4,
        vk::DescriptorType::eSampledImage,
        QUARTZ_MAX_NUMBER_TEXTURES,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding materialArrayLayoutBinding(
        5,
        vk::DescriptorType::eUniformBufferDynamic,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings = {
        cameraUniformBufferLayoutBinding,
        ambientLightLayoutBinding,
        directionalLightLayoutBinding,
        baseColorTextureSamplerLayoutBinding,
        baseColorTexturesLayoutBinding,
        materialArrayLayoutBinding
    };

    LOG_TRACE(PIPELINE, "Using {} layout bindings", layoutBindings.size());

    vk::DescriptorSetLayoutCreateInfo layoutCreateInfo(
        {},
        layoutBindings
    );

    vk::UniqueDescriptorSetLayout p_descriptorSetLayout = p_logicalDevice->createDescriptorSetLayoutUnique(
        layoutCreateInfo
    );

    if (!p_descriptorSetLayout) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to create vk::DescriptorSetLayout");
    }

    return p_descriptorSetLayout;
}

vk::UniqueDescriptorPool
quartz::rendering::Pipeline::createVulkanDescriptorPoolPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t numDescriptorSets /** should be the maximum number of frames in flight */
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "{} descriptor sets", numDescriptorSets);

    vk::DescriptorPoolSize cameraUniformBufferObjectPoolSize(
        vk::DescriptorType::eUniformBuffer,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing camera ubo of type uniform buffer with count {}", cameraUniformBufferObjectPoolSize.descriptorCount);

    vk::DescriptorPoolSize ambientLightPoolSize(
        vk::DescriptorType::eUniformBuffer,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing ambient light of type uniform buffer with count {}", ambientLightPoolSize.descriptorCount);

    vk::DescriptorPoolSize directionalLightPoolSize(
        vk::DescriptorType::eUniformBuffer,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing directional light of type uniform buffer with count {}", directionalLightPoolSize.descriptorCount);

    vk::DescriptorPoolSize baseColorTextureSamplerPoolSize(
        vk::DescriptorType::eSampler,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing texture sampler of type combined image sampler with count {}", baseColorTextureSamplerPoolSize.descriptorCount);

    vk::DescriptorPoolSize baseColorTexturesPoolSize(
        vk::DescriptorType::eSampledImage,
        numDescriptorSets * QUARTZ_MAX_NUMBER_TEXTURES
    );
    LOG_TRACE(PIPELINE, "Allowing textures of type sampled image with count {}", baseColorTexturesPoolSize.descriptorCount);

    vk::DescriptorPoolSize materialArrayPoolSize(
        vk::DescriptorType::eUniformBufferDynamic,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing materials of type uniform buffer with count {}", materialArrayPoolSize.descriptorCount);

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
        cameraUniformBufferObjectPoolSize,
        ambientLightPoolSize,
        directionalLightPoolSize,
        baseColorTextureSamplerPoolSize,
        baseColorTexturesPoolSize,
        materialArrayPoolSize
    };

    LOG_TRACE(PIPELINE, "Using {} pool sizes", descriptorPoolSizes.size());
    LOG_TRACE(PIPELINE, "Using maximum of {} descriptor sets", numDescriptorSets);

    vk::DescriptorPoolCreateInfo poolCreateInfo(
        {},
        /**
         * @brief how many descriptor sets in total can be allocated from this pool
         */
        numDescriptorSets,
        /**
         * @brief the descriptors that will be allocated (not in a single descriptor set,
         *   but in total) from this pool
         */
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
    const uint32_t minUniformBufferOffsetAlignment,
    const uint32_t maxNumFramesInFlight,
    const std::vector<quartz::rendering::LocallyMappedBuffer>& uniformBuffers,
    const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout,
    const vk::UniqueDescriptorPool& p_descriptorPool,
    const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs
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
        LOG_SCOPE_CHANGE_TRACE(PIPELINE);

        if (!descriptorSets[i]) {
            LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to allocate vk::DescriptorSet {}", i);
        }
        LOG_TRACE(PIPELINE, "Successfully allocated vk::DescriptorSet {}", i);

        /**
         * @todo 2023/11/11 Have some programmatic way of determining indices for all of the
         *   different buffer types we are using so we can easily determine what
         *   index they are for each frame
         */

        // ---+++ the camera +++--- //

        const uint32_t cameraIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 0;
        LOG_TRACE(PIPELINE, "Allocating space for camera UBO using buffer at index {}", cameraIndex);
        vk::DescriptorBufferInfo cameraUBOBufferInfo(
            *(uniformBuffers[cameraIndex].getVulkanLogicalBufferPtr()),
            0,
            sizeof(quartz::rendering::CameraUniformBufferObject)
        );
        vk::WriteDescriptorSet cameraUBODescriptorWriteSet(
            descriptorSets[i],
            0,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &cameraUBOBufferInfo,
            {}
        );

        // ---+++ the ambient light +++--- //

        const uint32_t ambientLightIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 1;
        LOG_TRACE(PIPELINE, "Allocating space for ambient light using buffer at index {}", ambientLightIndex);
        vk::DescriptorBufferInfo ambientLightBufferInfo(
            *(uniformBuffers[ambientLightIndex].getVulkanLogicalBufferPtr()),
            0,
            sizeof(quartz::scene::AmbientLight)
        );
        vk::WriteDescriptorSet ambientLightDescriptorWriteSet(
            descriptorSets[i],
            1,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &ambientLightBufferInfo,
            {}
        );

        // ---+++ the directional light +++--- //

        const uint32_t directionalLightIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 2;
        LOG_TRACE(PIPELINE, "Allocating space for directional light using buffer at index {}", directionalLightIndex);
        vk::DescriptorBufferInfo directionalLightInfo(
            *(uniformBuffers[directionalLightIndex].getVulkanLogicalBufferPtr()),
            0,
            sizeof(quartz::scene::DirectionalLight)
        );
        vk::WriteDescriptorSet directionalLightWriteDescriptorSet(
            descriptorSets[i],
            2,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &directionalLightInfo,
            {}
        );

        // ---+++ the texture sampler +++--- //

        LOG_TRACE(PIPELINE, "Allocating space for base color texture sampler");
        vk::DescriptorImageInfo baseColorTextureSamplerImageInfo(
            *(texturePtrs[0]->getVulkanSamplerPtr()),
            {},
            {}
        );
        vk::WriteDescriptorSet baseColorTextureSamplerWriteDescriptorSet(
            descriptorSets[i],
            3,
            0,
            1,
            vk::DescriptorType::eSampler,
            &baseColorTextureSamplerImageInfo,
            {},
            {}
        );

        // ---+++ the texture array +++--- //

        LOG_TRACE(PIPELINE, "Allocating space for {} textures", texturePtrs.size());
        std::vector<vk::DescriptorImageInfo> baseColorTextureImageInfos;
        for (uint32_t j = 0; j < texturePtrs.size(); ++j) {
            baseColorTextureImageInfos.emplace_back(
                nullptr,
                *(texturePtrs[j]->getVulkanImageViewPtr()),
                vk::ImageLayout::eShaderReadOnlyOptimal
            );
        }

        uint32_t remainingTextureSpaces = QUARTZ_MAX_NUMBER_TEXTURES - baseColorTextureImageInfos.size();
        LOG_TRACE(PIPELINE, "Filling remaining {} textures with texture 0", remainingTextureSpaces);
        for (uint32_t j = 0; j < remainingTextureSpaces; ++j) {
            baseColorTextureImageInfos.emplace_back(
                nullptr,
                *(texturePtrs[0]->getVulkanImageViewPtr()),
                vk::ImageLayout::eShaderReadOnlyOptimal
            );
        }

        vk::WriteDescriptorSet baseColorTexturesDescriptorWriteSet(
            descriptorSets[i],
            4,
            0,
            QUARTZ_MAX_NUMBER_TEXTURES,
            vk::DescriptorType::eSampledImage,
            baseColorTextureImageInfos.data(),
            {},
            {}
        );

        // ---+++ the dynamic material UBO +++--- //

        const uint32_t materialArrayIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 3;
        LOG_TRACE(PIPELINE, "Allocating space for materials array dynamic UBO at index {}", materialArrayIndex);

        LOG_TRACE(PIPELINE, "Using un-adjusted material stride of 0x{:X} bytes", sizeof(quartz::rendering::MaterialUniformBufferObject));
        const uint32_t materialByteStride = minUniformBufferOffsetAlignment > 0 ?
            (sizeof(quartz::rendering::MaterialUniformBufferObject) + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1) :
            sizeof(quartz::rendering::MaterialUniformBufferObject);
        LOG_TRACE(PIPELINE, "Using    adjusted material stride of 0x{:X} bytes", materialByteStride);

        vk::DescriptorBufferInfo materialArrayUBOBufferInfo(
            *(uniformBuffers[materialArrayIndex].getVulkanLogicalBufferPtr()),
            0,
            materialByteStride
        );

        vk::WriteDescriptorSet materialArrayUBODescriptorWriteSet(
            descriptorSets[i],
            5,
            0,
            1,
            vk::DescriptorType::eUniformBufferDynamic,
            {},
            &materialArrayUBOBufferInfo,
            {}
        );

        // ---+++ all of the write descriptor sets used to +++--- //

        LOG_TRACE(PIPELINE, "Consolidating write descriptor sets");
        std::vector<vk::WriteDescriptorSet> writeDescriptorSets = {
            cameraUBODescriptorWriteSet,
            ambientLightDescriptorWriteSet,
            directionalLightWriteDescriptorSet,
            baseColorTextureSamplerWriteDescriptorSet,
            baseColorTexturesDescriptorWriteSet,
            materialArrayUBODescriptorWriteSet
        };

        LOG_TRACE(PIPELINE, "Updating descriptor sets with {} descriptor writes", writeDescriptorSets.size());

        p_logicalDevice->updateDescriptorSets(
            writeDescriptorSets.size(),
            writeDescriptorSets.data(),
            0,
            nullptr
        );
    }

    return descriptorSets;
}

vk::UniqueRenderPass
quartz::rendering::Pipeline::createVulkanRenderPassPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const vk::Format& depthFormat
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    vk::AttachmentDescription colorAttachment(
        {},
        surfaceFormat.format,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    );
    vk::AttachmentReference colorAttachmentRef(
        0,
        vk::ImageLayout::eColorAttachmentOptimal
    );

    vk::AttachmentDescription depthAttachment(
        {},
        depthFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eDontCare,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    );
    vk::AttachmentReference depthAttachmentRef(
        1,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    );

    vk::SubpassDescription subpassDescription(
        {},
        vk::PipelineBindPoint::eGraphics,
        {},
        colorAttachmentRef,
        {},
        &depthAttachmentRef,
        {}
    );

    vk::SubpassDependency subpassDependency(
        VK_SUBPASS_EXTERNAL,
        0,
        (
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests
        ),
        (
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests
        ),
        {},
        (
            vk::AccessFlagBits::eColorAttachmentWrite |
            vk::AccessFlagBits::eDepthStencilAttachmentWrite
        ),
        {}
    );

    std::vector<vk::AttachmentDescription> attachments = {
        colorAttachment,
        depthAttachment
    };
    vk::RenderPassCreateInfo renderPassCreateInfo(
        {},
        attachments,
        subpassDescription,
        subpassDependency
    );

    vk::UniqueRenderPass p_renderPass = p_logicalDevice->createRenderPassUnique(renderPassCreateInfo);

    if (!p_renderPass) {
        LOG_THROW(PIPELINE, util::VulkanCreationFailedError, "Failed to create vk::RenderPass");
    }

    return p_renderPass;
}

vk::UniquePipelineLayout
quartz::rendering::Pipeline::createVulkanPipelineLayoutPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout
) {
    LOG_FUNCTION_SCOPE_TRACE(PIPELINE, "");

    vk::PushConstantRange vertexPushConstantRange(
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(glm::mat4)
    );

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
    const uint32_t maxNumFramesInFlight
) :
    m_maxNumFramesInFlight(maxNumFramesInFlight),
    m_currentInFlightFrameIndex(0),
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
            util::FileSystem::getCompiledShaderAbsoluteFilepath(
                "shader.vert"
            )
        )
    ),
    mp_vulkanFragmentShaderModule(
        quartz::rendering::Pipeline::createVulkanShaderModulePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            util::FileSystem::getCompiledShaderAbsoluteFilepath(
                "shader.frag"
            )
        )
    ),
    m_uniformBuffers(
        quartz::rendering::Pipeline::createUniformBuffers(
            renderingDevice,
            m_maxNumFramesInFlight
        )
    ),
    mp_vulkanDescriptorSetLayout(
        quartz::rendering::Pipeline::createVulkanDescriptorSetLayoutPtr(
            renderingDevice.getVulkanLogicalDevicePtr()
        )
    ),
    m_vulkanDescriptorPoolPtr(
        quartz::rendering::Pipeline::createVulkanDescriptorPoolPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_maxNumFramesInFlight
        )
    ),
    m_vulkanDescriptorSets(),
    mp_vulkanRenderPass(
        quartz::rendering::Pipeline::createVulkanRenderPassPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingWindow.getVulkanSurfaceFormat(),
            renderingWindow.getVulkanDepthBufferFormat()
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
            mp_vulkanRenderPass
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
    mp_vulkanRenderPass.reset();
}

void
quartz::rendering::Pipeline::recreate(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanRenderPass = quartz::rendering::Pipeline::createVulkanRenderPassPtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanSurfaceFormat(),
        renderingWindow.getVulkanDepthBufferFormat()
    );
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
        mp_vulkanRenderPass
    );
}

void
quartz::rendering::Pipeline::allocateVulkanDescriptorSets(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    m_vulkanDescriptorSets.clear();

    m_vulkanDescriptorSets = quartz::rendering::Pipeline::allocateVulkanDescriptorSets(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment,
        m_maxNumFramesInFlight,
        m_uniformBuffers,
        mp_vulkanDescriptorSetLayout,
        m_vulkanDescriptorPoolPtr,
        texturePtrs
    );
}

void
quartz::rendering::Pipeline::updateCameraUniformBuffer(
    const quartz::scene::Camera& camera
) {
    quartz::rendering::CameraUniformBufferObject cameraUBO(
        camera.getViewMatrix(),
        camera.getProjectionMatrix()
    );

    const uint32_t cameraIndex = m_currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 0;
    memcpy(
        m_uniformBuffers[cameraIndex].getMappedLocalMemoryPtr(),
        &cameraUBO,
        sizeof(quartz::rendering::CameraUniformBufferObject)
    );
}

void
quartz::rendering::Pipeline::updateAmbientLightUniformBuffer(
    const quartz::scene::AmbientLight& ambientLight
) {
    const uint32_t ambientLightIndex = m_currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 1;
    memcpy(
        m_uniformBuffers[ambientLightIndex].getMappedLocalMemoryPtr(),
        &ambientLight,
        sizeof(quartz::scene::AmbientLight)
    );
}

void
quartz::rendering::Pipeline::updateDirectionalLightUniformBuffer(
    const quartz::scene::DirectionalLight& directionalLight
) {
    const uint32_t directionalLightIndex = m_currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 2;
    memcpy(
        m_uniformBuffers[directionalLightIndex].getMappedLocalMemoryPtr(),
        &directionalLight,
        sizeof(quartz::scene::DirectionalLight)
    );
}

void
quartz::rendering::Pipeline::updateMaterialArrayUniformBuffer(
    const uint32_t minUniformBufferOffsetAlignment
) {
    std::vector<quartz::rendering::Material> materials(19);

    std::vector<quartz::rendering::MaterialUniformBufferObject> materialUBOs;
    for (const quartz::rendering::Material& material : materials) {
        materialUBOs.emplace_back(
            material.getBaseColorTextureMasterIndex(),
            material.getMetallicRoughnessTextureMasterIndex(),
            material.getNormalTextureMasterIndex(),
            material.getEmissionTextureMasterIndex(),
            material.getOcclusionTextureMasterIndex(),

            material.getBaseColorFactor(),
            material.getEmissiveFactor(),
            material.getMetallicFactor(),
            material.getRoughnessFactor(),

            static_cast<uint32_t>(material.getAlphaMode()),
            material.getAlphaCutoff(),
            material.getDoubleSided()
        );
    }

    const uint32_t materialArrayIndex = m_currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 3;
    void* p_mappedBuffer = m_uniformBuffers[materialArrayIndex].getMappedLocalMemoryPtr();

    for (uint32_t i = 0; i < materialUBOs.size(); ++i) {
        uint32_t materialByteOffset = minUniformBufferOffsetAlignment > 0 ?
            (sizeof(quartz::rendering::MaterialUniformBufferObject) + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1) :
            sizeof(quartz::rendering::MaterialUniformBufferObject);
        materialByteOffset *= i;

        void* p_currentMaterialSlot = reinterpret_cast<uint8_t*>(p_mappedBuffer) + materialByteOffset;

        memcpy(
             p_currentMaterialSlot,
            &(materialUBOs[i]),
            sizeof(quartz::rendering::MaterialUniformBufferObject)
        );
    }

}
