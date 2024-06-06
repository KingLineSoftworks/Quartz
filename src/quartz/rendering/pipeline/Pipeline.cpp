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

constexpr uint32_t NUM_UNIQUE_UNIFORM_BUFFERS = 8;

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
            sizeof(quartz::scene::Camera::UniformBufferObject),
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

        LOG_TRACE(PIPELINE, "Creating point light count buffer {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            sizeof(uint32_t),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        LOG_TRACE(PIPELINE, "Creating point light buffer {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            sizeof(quartz::scene::PointLight) * QUARTZ_MAX_NUMBER_POINT_LIGHTS,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        LOG_TRACE(PIPELINE, "Creating spot light count buffer {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            sizeof(uint32_t),
            vk::BufferUsageFlagBits::eUniformBuffer,
            (
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        LOG_TRACE(PIPELINE, "Creating spot light buffer {} at buffer index {}", i, buffers.size());
        buffers.emplace_back(
            renderingDevice,
            sizeof(quartz::scene::SpotLight) * QUARTZ_MAX_NUMBER_SPOT_LIGHTS,
            vk::BufferUsageFlagBits::eUniformBuffer,
            (
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            )
        );

        const uint32_t minUniformBufferOffsetAlignment = renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment;
        const uint32_t materialByteStride = minUniformBufferOffsetAlignment > 0 ?
            (sizeof(quartz::rendering::Material::UniformBufferObject) + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1) :
            sizeof(quartz::rendering::Material::UniformBufferObject);
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
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
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

    vk::DescriptorSetLayoutBinding pointLightCountLayoutBinding(
        3,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding pointLightLayoutBinding(
        4,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding spotLightCountLayoutBinding(
        5,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding spotLightLayoutBinding(
        6,
        vk::DescriptorType::eUniformBuffer,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding rgbaTextureSamplerLayoutBinding(
        7,
        vk::DescriptorType::eSampler,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding textureArrayLayoutBinding(
        8,
        vk::DescriptorType::eSampledImage,
        QUARTZ_MAX_NUMBER_TEXTURES,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    vk::DescriptorSetLayoutBinding materialArrayLayoutBinding(
        9,
        vk::DescriptorType::eUniformBufferDynamic,
        1,
        vk::ShaderStageFlagBits::eFragment,
        {}
    );

    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings = {
        cameraUniformBufferLayoutBinding,
        ambientLightLayoutBinding,
        directionalLightLayoutBinding,
        pointLightCountLayoutBinding,
        pointLightLayoutBinding,
        spotLightCountLayoutBinding,
        spotLightLayoutBinding,
        rgbaTextureSamplerLayoutBinding,
        textureArrayLayoutBinding,
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

    vk::DescriptorPoolSize pointLightCountPoolSize(
        vk::DescriptorType::eUniformBuffer,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing point light count of type uniform buffer with count {}", directionalLightPoolSize.descriptorCount);

    vk::DescriptorPoolSize pointLightPoolSize(
        vk::DescriptorType::eUniformBuffer,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing point light array of type uniform buffer with count {}", directionalLightPoolSize.descriptorCount);

    vk::DescriptorPoolSize spotLightCountPoolSize(
        vk::DescriptorType::eUniformBuffer,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing spot light count of type uniform buffer with count {}", directionalLightPoolSize.descriptorCount);

    vk::DescriptorPoolSize spotLightPoolSize(
        vk::DescriptorType::eUniformBuffer,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing spot light array of type uniform buffer with count {}", directionalLightPoolSize.descriptorCount);

    vk::DescriptorPoolSize rgbaTextureSamplerPoolSize(
        vk::DescriptorType::eSampler,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing texture sampler of type combined image sampler with count {}", rgbaTextureSamplerPoolSize.descriptorCount);

    vk::DescriptorPoolSize textureArrayPoolSize(
        vk::DescriptorType::eSampledImage,
        numDescriptorSets * QUARTZ_MAX_NUMBER_TEXTURES
    );
    LOG_TRACE(PIPELINE, "Allowing textures of type sampled image with count {} (num descriptor sets {} * max number textures {}", textureArrayPoolSize.descriptorCount, numDescriptorSets, QUARTZ_MAX_NUMBER_TEXTURES);

    vk::DescriptorPoolSize materialArrayPoolSize(
        vk::DescriptorType::eUniformBufferDynamic,
        numDescriptorSets
    );
    LOG_TRACE(PIPELINE, "Allowing materials of type uniform buffer with count {}", materialArrayPoolSize.descriptorCount);

    std::vector<vk::DescriptorPoolSize> descriptorPoolSizes = {
        cameraUniformBufferObjectPoolSize,
        ambientLightPoolSize,
        directionalLightPoolSize,
        pointLightCountPoolSize,
        pointLightPoolSize,
        spotLightCountPoolSize,
        spotLightPoolSize,
        rgbaTextureSamplerPoolSize,
        textureArrayPoolSize,
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
            sizeof(quartz::scene::Camera::UniformBufferObject)
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

        // ---+++ the number of point lights +++--- //

        const uint32_t pointLightCountIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 3;
        LOG_TRACE(PIPELINE, "Allocating space for the uint32_t number of point lights");
        vk::DescriptorBufferInfo pointLightCountInfo(
            *(uniformBuffers[pointLightCountIndex].getVulkanLogicalBufferPtr()),
            0,
            sizeof(uint32_t)
        );
        vk::WriteDescriptorSet pointLightCountWriteDescriptorSet(
            descriptorSets[i],
            3,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &pointLightCountInfo,
            {}
        );

        // ---+++ the point lights +++--- //

        const uint32_t pointLightIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 4;
        LOG_TRACE(PIPELINE, "Allocating space for directional light using buffer at index {}", directionalLightIndex);
        vk::DescriptorBufferInfo pointLightInfo(
            *(uniformBuffers[pointLightIndex].getVulkanLogicalBufferPtr()),
            0,
            sizeof(quartz::scene::PointLight)
        );
        vk::WriteDescriptorSet pointLightWriteDescriptorSet(
            descriptorSets[i],
            4,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &pointLightInfo,
            {}
        );

        // ---+++ the number of spot lights +++--- //

        const uint32_t spotLightCountIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 5;
        LOG_TRACE(PIPELINE, "Allocating space for the uint32_t number of spot lights");
        vk::DescriptorBufferInfo spotLightCountInfo(
            *(uniformBuffers[spotLightCountIndex].getVulkanLogicalBufferPtr()),
            0,
            sizeof(uint32_t)
        );
        vk::WriteDescriptorSet spotLightCountWriteDescriptorSet(
            descriptorSets[i],
            5,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &spotLightCountInfo,
            {}
        );

        // ---+++ the spot lights +++--- //

        const uint32_t spotLightIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 6;
        LOG_TRACE(PIPELINE, "Allocating space for directional light using buffer at index {}", directionalLightIndex);
        vk::DescriptorBufferInfo spotLightInfo(
            *(uniformBuffers[spotLightIndex].getVulkanLogicalBufferPtr()),
            0,
            sizeof(quartz::scene::SpotLight)
        );
        vk::WriteDescriptorSet spotLightWriteDescriptorSet(
            descriptorSets[i],
            6,
            0,
            1,
            vk::DescriptorType::eUniformBuffer,
            {},
            &spotLightInfo,
            {}
        );

        // ---+++ the texture sampler +++--- //

        LOG_TRACE(PIPELINE, "Allocating space for texture sampler");
        vk::DescriptorImageInfo rgbaTextureSamplerImageInfo(
            *(texturePtrs[quartz::rendering::Texture::getBaseColorDefaultMasterIndex()]->getVulkanSamplerPtr()),
            {},
            {}
        );
        vk::WriteDescriptorSet rgbaTextureSamplerWriteDescriptorSet(
            descriptorSets[i],
            7,
            0,
            1,
            vk::DescriptorType::eSampler,
            &rgbaTextureSamplerImageInfo,
            {},
            {}
        );

        // ---+++ the texture array +++--- //

        /** @todo 2024/06/04 Populate the textures when we load the scene instead of when we create the pipeline. This is probably going
         *    to be more difficult than we think it ought to be
         */

        LOG_TRACE(PIPELINE, "Allocating space for {} textures", texturePtrs.size());
        std::vector<vk::DescriptorImageInfo> textureImageInfos;
        for (uint32_t j = 0; j < texturePtrs.size(); ++j) {
            textureImageInfos.emplace_back(
                nullptr,
                *(texturePtrs[j]->getVulkanImageViewPtr()),
                vk::ImageLayout::eShaderReadOnlyOptimal
            );
        }

        uint32_t remainingTextureSpaces = QUARTZ_MAX_NUMBER_TEXTURES - textureImageInfos.size();
        LOG_TRACE(PIPELINE, "Filling remaining {} textures with texture 0", remainingTextureSpaces);
        for (uint32_t j = 0; j < remainingTextureSpaces; ++j) {
            textureImageInfos.emplace_back(
                nullptr,
                *(texturePtrs[0]->getVulkanImageViewPtr()),
                vk::ImageLayout::eShaderReadOnlyOptimal
            );
        }

        vk::WriteDescriptorSet textureArrayDescriptorWriteSet(
            descriptorSets[i],
            8,
            0,
            QUARTZ_MAX_NUMBER_TEXTURES,
            vk::DescriptorType::eSampledImage,
            textureImageInfos.data(),
            {},
            {}
        );

        // ---+++ the dynamic material UBO +++--- //

        const uint32_t materialArrayIndex = i * NUM_UNIQUE_UNIFORM_BUFFERS + 7;
        LOG_TRACE(PIPELINE, "Allocating space for materials array dynamic UBO at index {}", materialArrayIndex);

        LOG_TRACE(PIPELINE, "Using un-adjusted material stride of 0x{:X} bytes", sizeof(quartz::rendering::Material::UniformBufferObject));
        const uint32_t materialByteStride = minUniformBufferOffsetAlignment > 0 ?
            (sizeof(quartz::rendering::Material::UniformBufferObject) + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1) :
            sizeof(quartz::rendering::Material::UniformBufferObject);
        LOG_TRACE(PIPELINE, "Using    adjusted material stride of 0x{:X} bytes", materialByteStride);

        vk::DescriptorBufferInfo materialArrayUBOBufferInfo(
            *(uniformBuffers[materialArrayIndex].getVulkanLogicalBufferPtr()),
            0,
            materialByteStride
        );
        vk::WriteDescriptorSet materialArrayUBODescriptorWriteSet(
            descriptorSets[i],
            9,
            0,
            1,
            vk::DescriptorType::eUniformBufferDynamic,
            {},
            &materialArrayUBOBufferInfo,
            {}
        );

        // ---+++ all the write descriptor sets used to +++--- //

        LOG_TRACE(PIPELINE, "Consolidating write descriptor sets");
        std::vector<vk::WriteDescriptorSet> writeDescriptorSets = {
            cameraUBODescriptorWriteSet,
            ambientLightDescriptorWriteSet,
            directionalLightWriteDescriptorSet,
            pointLightCountWriteDescriptorSet,
            pointLightWriteDescriptorSet,
            spotLightCountWriteDescriptorSet,
            spotLightWriteDescriptorSet,
            rgbaTextureSamplerWriteDescriptorSet,
            textureArrayDescriptorWriteSet,
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
    std::vector<quartz::rendering::UniformBufferInfo>&& uniformBufferInfos
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
    m_uniformBufferInfos(std::move(uniformBufferInfos)),
    m_uniformBuffers(
        quartz::rendering::Pipeline::createUniformBuffers(
            renderingDevice,
            maxNumFramesInFlight
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
            maxNumFramesInFlight
        )
    ),
    m_vulkanDescriptorSets(),
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
quartz::rendering::Pipeline::allocateVulkanDescriptorSets(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<std::shared_ptr<quartz::rendering::Texture>>& texturePtrs,
    const uint32_t maxNumFramesInFlight
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    m_vulkanDescriptorSets.clear();

    m_vulkanDescriptorSets = quartz::rendering::Pipeline::allocateVulkanDescriptorSets(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment,
        maxNumFramesInFlight,
        m_uniformBuffers,
        mp_vulkanDescriptorSetLayout,
        m_vulkanDescriptorPoolPtr,
        texturePtrs
    );
}

void
quartz::rendering::Pipeline::updateCameraUniformBuffer(
    const quartz::scene::Camera& camera,
    const uint32_t currentInFlightFrameIndex
) {
    quartz::scene::Camera::UniformBufferObject cameraUBO(
        camera.getWorldPosition(),
        camera.getViewMatrix(),
        camera.getProjectionMatrix()
    );

    const uint32_t cameraIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 0;
    memcpy(
        m_uniformBuffers[cameraIndex].getMappedLocalMemoryPtr(),
        &cameraUBO,
        sizeof(quartz::scene::Camera::UniformBufferObject)
    );
}

void
quartz::rendering::Pipeline::updateAmbientLightUniformBuffer(
    const quartz::scene::AmbientLight& ambientLight,
    const uint32_t currentInFlightFrameIndex
) {
    const uint32_t ambientLightIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 1;
    memcpy(
        m_uniformBuffers[ambientLightIndex].getMappedLocalMemoryPtr(),
        &ambientLight,
        sizeof(quartz::scene::AmbientLight)
    );
}

void
quartz::rendering::Pipeline::updateDirectionalLightUniformBuffer(
    const quartz::scene::DirectionalLight& directionalLight,
    const uint32_t currentInFlightFrameIndex
) {
    const uint32_t directionalLightIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 2;
    memcpy(
        m_uniformBuffers[directionalLightIndex].getMappedLocalMemoryPtr(),
        &directionalLight,
        sizeof(quartz::scene::DirectionalLight)
    );
}

void
quartz::rendering::Pipeline::updatePointLightUniformBuffer(
    const std::vector<quartz::scene::PointLight>& pointLights,
    const uint32_t currentInFlightFrameIndex
) {
    uint32_t pointLightCount = std::min<uint32_t>(pointLights.size(), QUARTZ_MAX_NUMBER_POINT_LIGHTS);
    const uint32_t pointLightCountIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 3;
    memcpy(
        m_uniformBuffers[pointLightCountIndex].getMappedLocalMemoryPtr(),
        &pointLightCount,
        sizeof(uint32_t)
    );

    const uint32_t pointLightIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 4;
    memcpy(
        m_uniformBuffers[pointLightIndex].getMappedLocalMemoryPtr(),
        pointLights.data(),
        sizeof(quartz::scene::PointLight) * pointLightCount
    );
}

void
quartz::rendering::Pipeline::updateSpotLightUniformBuffer(
    const std::vector<quartz::scene::SpotLight>& spotLights,
    const uint32_t currentInFlightFrameIndex
) {
    uint32_t spotLightCount = std::min<uint32_t>(spotLights.size(), QUARTZ_MAX_NUMBER_SPOT_LIGHTS);
    const uint32_t spotLightCountIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 5;
    memcpy(
        m_uniformBuffers[spotLightCountIndex].getMappedLocalMemoryPtr(),
        &spotLightCount,
        sizeof(uint32_t)
    );

    const uint32_t spotLightIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 6;
    memcpy(
        m_uniformBuffers[spotLightIndex].getMappedLocalMemoryPtr(),
        spotLights.data(),
        sizeof(quartz::scene::SpotLight) * spotLightCount
    );

}

void
quartz::rendering::Pipeline::updateMaterialArrayUniformBuffer(
    const uint32_t minUniformBufferOffsetAlignment,
    const uint32_t currentInFlightFrameIndex
) {
    std::vector<quartz::rendering::Material::UniformBufferObject> materialUBOs;
    for (const std::shared_ptr<quartz::rendering::Material>& p_material : quartz::rendering::Material::getMasterMaterialList()) {
        materialUBOs.emplace_back(
            p_material->getBaseColorTextureMasterIndex(),
            p_material->getMetallicRoughnessTextureMasterIndex(),
            p_material->getNormalTextureMasterIndex(),
            p_material->getEmissionTextureMasterIndex(),
            p_material->getOcclusionTextureMasterIndex(),

            p_material->getBaseColorFactor(),
            p_material->getEmissiveFactor(),
            p_material->getMetallicFactor(),
            p_material->getRoughnessFactor(),

            static_cast<uint32_t>(p_material->getAlphaMode()),
            p_material->getAlphaCutoff(),
            p_material->getDoubleSided()
        );
    }

    const uint32_t materialArrayIndex = currentInFlightFrameIndex * NUM_UNIQUE_UNIFORM_BUFFERS + 7;
    void* p_mappedBuffer = m_uniformBuffers[materialArrayIndex].getMappedLocalMemoryPtr();

    for (uint32_t i = 0; i < materialUBOs.size(); ++i) {
        uint32_t materialByteOffset = minUniformBufferOffsetAlignment > 0 ?
            (sizeof(quartz::rendering::Material::UniformBufferObject) + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1) :
            sizeof(quartz::rendering::Material::UniformBufferObject);
        materialByteOffset *= i;

        void* p_currentMaterialSlot = reinterpret_cast<uint8_t*>(p_mappedBuffer) + materialByteOffset;

        memcpy(
             p_currentMaterialSlot,
            &(materialUBOs[i]),
            sizeof(quartz::rendering::Material::UniformBufferObject)
        );
    }

}
