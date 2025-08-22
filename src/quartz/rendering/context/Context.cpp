#include <string>

#include "math/transform/Mat4.hpp"

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/cube_map/CubeMap.hpp"
#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"
#include "quartz/rendering/pipeline/PushConstantInfo.hpp"
#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"
#include "quartz/rendering/pipeline/UniformSamplerInfo.hpp"
#include "quartz/rendering/pipeline/UniformTextureArrayInfo.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"
#include "quartz/scene/light/PointLight.hpp"
#include "quartz/scene/light/SpotLight.hpp"
#include "util/logger/Logger.hpp"

quartz::rendering::Pipeline
quartz::rendering::Context::createSkyBoxRenderingPipeline(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow,
    const quartz::rendering::RenderPass& renderingRenderPass,
    const uint32_t maxNumFramesInFlight
) {
    LOG_FUNCTION_SCOPE_DEBUG(CONTEXT, "");

    std::vector<quartz::rendering::UniformBufferInfo> uniformBufferInfos = {
        // camera
        {
            sizeof(quartz::scene::Camera::UniformBufferObject),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            0,
            1,
            sizeof(quartz::scene::Camera::UniformBufferObject),
            false,
            vk::ShaderStageFlagBits::eVertex
        }
    };

    quartz::rendering::UniformSamplerCubeInfo uniformSamplerCubeInfo(
        1,
        1,
        vk::ShaderStageFlagBits::eFragment
    );

    LOG_DEBUG(PIPELINE, "Using {} uniform buffers", uniformBufferInfos.size());

    return {
        renderingDevice,
        renderingWindow,
        renderingRenderPass,
        util::FileSystem::getCompiledShaderAbsoluteFilepath("skybox.vert"),
        util::FileSystem::getCompiledShaderAbsoluteFilepath("skybox.frag"),
        maxNumFramesInFlight,
        quartz::rendering::CubeMap::getVulkanVertexInputBindingDescription(),
        quartz::rendering::CubeMap::getVulkanVertexInputAttributeDescriptions(),
        vk::CullModeFlagBits::eFront,
        false,
        {},
        uniformBufferInfos,
        uniformSamplerCubeInfo,
        std::nullopt,
        std::nullopt
    };
}

quartz::rendering::Pipeline
quartz::rendering::Context::createDoodadRenderingPipeline(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow,
    const quartz::rendering::RenderPass& renderingRenderPass,
    const uint32_t maxNumFramesInFlight
) {
    LOG_FUNCTION_SCOPE_DEBUG(CONTEXT, "");

    std::vector<quartz::rendering::PushConstantInfo> pushConstantInfos = {
        // perObjectVertexPushConstant (for model matrix)
        {
            vk::ShaderStageFlagBits::eVertex,
            0,
            sizeof(math::Mat4)
        },
        // dummy
        {
            vk::ShaderStageFlagBits::eFragment,
            sizeof(math::Mat4),
            sizeof(uint32_t)
        }
    };

    const uint32_t materialByteStride = quartz::rendering::UniformBufferInfo::calculateDynamicUniformBufferByteStride(renderingDevice, sizeof(quartz::rendering::Material::UniformBufferObject));
    std::vector<quartz::rendering::UniformBufferInfo> uniformBufferInfos = {
        // the camera
        {
            sizeof(quartz::scene::Camera::UniformBufferObject),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            0,
            1,
            sizeof(quartz::scene::Camera::UniformBufferObject),
            false,
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
        },
        // the ambient light
        {
            sizeof(quartz::scene::AmbientLight),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            1,
            1,
            sizeof(quartz::scene::AmbientLight),
            false,
            vk::ShaderStageFlagBits::eFragment
        },
        // the directional light
        {
            sizeof(quartz::scene::DirectionalLight),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            2,
            1,
            sizeof(quartz::scene::DirectionalLight),
            false,
            vk::ShaderStageFlagBits::eFragment
        },
        // the number of point lights
        {
            sizeof(uint32_t),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            3,
            1,
            sizeof(uint32_t),
            false,
            vk::ShaderStageFlagBits::eFragment
        },
        // the point lights
        {
            sizeof(quartz::scene::PointLight) * QUARTZ_MAX_NUMBER_POINT_LIGHTS,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            4,
            1,
            sizeof(quartz::scene::PointLight),
            false,
            vk::ShaderStageFlagBits::eFragment
        },
        // the number of spot lights
        {
            sizeof(uint32_t),
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            5,
            1,
            sizeof(uint32_t),
            false,
            vk::ShaderStageFlagBits::eFragment
        },
        // the spot lights
        {
            sizeof(quartz::scene::SpotLight) * QUARTZ_MAX_NUMBER_SPOT_LIGHTS,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            6,
            1,
            sizeof(quartz::scene::SpotLight),
            false,
            vk::ShaderStageFlagBits::eFragment
        },
        // the materials
        {
            materialByteStride * QUARTZ_MAX_NUMBER_MATERIALS,
            vk::MemoryPropertyFlagBits::eHostVisible,
            9,
            1,
            materialByteStride,
            true,
            vk::ShaderStageFlagBits::eFragment
        },
    };

    quartz::rendering::UniformSamplerInfo uniformSamplerInfo(
        7,
        1,
        vk::ShaderStageFlagBits::eFragment
    );

    quartz::rendering::UniformTextureArrayInfo uniformTextureArrayInfo(
        8,
        QUARTZ_MAX_NUMBER_TEXTURES,
        vk::ShaderStageFlagBits::eFragment
    );

    LOG_DEBUG(PIPELINE, "Using {} push constants", pushConstantInfos.size());
    LOG_DEBUG(PIPELINE, "Using {} uniform buffers", uniformBufferInfos.size());
    LOG_DEBUG(PIPELINE, "Using a uniform sampler");
    LOG_DEBUG(PIPELINE, "Using a uniform texture array");

    return {
        renderingDevice,
        renderingWindow,
        renderingRenderPass,
        util::FileSystem::getCompiledShaderAbsoluteFilepath("shader.vert"),
        util::FileSystem::getCompiledShaderAbsoluteFilepath("shader.frag"),
        maxNumFramesInFlight,
        quartz::rendering::Vertex::getVulkanVertexInputBindingDescription(),
        quartz::rendering::Vertex::getVulkanVertexInputAttributeDescriptions(),
        vk::CullModeFlagBits::eBack,
        true,
        pushConstantInfos,
        uniformBufferInfos,
        std::nullopt,
        uniformSamplerInfo,
        uniformTextureArrayInfo
    };
}

quartz::rendering::Context::Context(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels,
    const bool validationLayersEnabled
) :
    m_maxNumFramesInFlight(2),
    m_currentInFlightFrameIndex(0),
    m_renderingInstance(
        applicationName,
        applicationMajorVersion,
        applicationMinorVersion,
        applicationPatchVersion,
        validationLayersEnabled
    ),
    m_renderingDevice(m_renderingInstance),
    m_renderingWindow(
        applicationName,
        windowWidthPixels,
        windowHeightPixels,
        m_renderingInstance,
        m_renderingDevice
    ),
    m_renderingRenderPass(
        m_renderingDevice,
        m_renderingWindow
    ),
    m_skyBoxRenderingPipeline(
        quartz::rendering::Context::createSkyBoxRenderingPipeline(
            m_renderingDevice,
            m_renderingWindow,
            m_renderingRenderPass,
            m_maxNumFramesInFlight
        )
    ),
    m_doodadRenderingPipeline(
        quartz::rendering::Context::createDoodadRenderingPipeline(
            m_renderingDevice,
            m_renderingWindow,
            m_renderingRenderPass,
            m_maxNumFramesInFlight
        )
    ),
    m_renderingSwapchain(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingRenderPass,
        m_maxNumFramesInFlight
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Context::~Context() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::rendering::Context::loadScene(const quartz::scene::Scene& scene) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_DEBUGthis("Updating skybox rendering pipeline's descriptor sets");
    m_skyBoxRenderingPipeline.updateUniformBufferDescriptorSets(m_renderingDevice);
    m_skyBoxRenderingPipeline.updateSamplerCubeDescriptorSets(m_renderingDevice, scene.getSkyBox().getCubeMap().getVulkanSamplerPtr(), scene.getSkyBox().getCubeMap().getVulkanImageViewPtr());

    LOG_DEBUGthis("Updating doodad rendering pipeline's descriptor sets");
    m_doodadRenderingPipeline.updateUniformBufferDescriptorSets(m_renderingDevice);
    m_doodadRenderingPipeline.updateSamplerDescriptorSets(m_renderingDevice, quartz::rendering::Texture::getDefaultVulkanSamplerPtr());
    m_doodadRenderingPipeline.updateTextureArrayDescriptorSets(m_renderingDevice, quartz::rendering::Texture::getMasterTextureList());

    m_renderingSwapchain.setScreenClearColor(scene.getScreenClearColor());
}

void
quartz::rendering::Context::draw(
    const quartz::scene::Scene& scene
) {
    m_renderingSwapchain.waitForInFlightFence(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    const uint32_t availableSwapchainImageIndex = m_renderingSwapchain.getAvailableImageIndex(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        recreateSwapchain();
        return;
    }

    // update skybox pipeline //

    quartz::scene::Camera::UniformBufferObject cameraUBO(scene.getCamera());
    m_skyBoxRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 0, &cameraUBO);

    // update doodad drawing pipeline //

    m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 0, &cameraUBO);

    quartz::scene::AmbientLight ambientLight(scene.getAmbientLight());
    m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 1, &ambientLight);

    quartz::scene::DirectionalLight directionalLight(scene.getDirectionalLight());
    m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 2, &directionalLight);

    uint32_t pointLightCount = scene.getPointLights().size();
    m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 3, &pointLightCount);
    if (pointLightCount > 0) {
        m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 4, const_cast<quartz::scene::PointLight*>(scene.getPointLights().data()));
    }

    uint32_t spotLightCount = scene.getSpotLights().size();
    m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 5, &spotLightCount);
    if (spotLightCount > 0) {
        m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 6, const_cast<quartz::scene::SpotLight*>(scene.getSpotLights().data()));
    }

    std::vector<uint8_t> alignedMaterialUBOBytes(m_doodadRenderingPipeline.getUniformBufferInfo(7).getLocallyMappedBufferSize(), 0x00);
    const uint32_t numBytesToCopy = sizeof(quartz::rendering::Material::UniformBufferObject);
    const uint32_t objectStride = m_doodadRenderingPipeline.getUniformBufferInfo(7).getObjectStrideBytes();
    uint8_t* p_bufferStart = alignedMaterialUBOBytes.data();
    for (uint32_t i = 0; i < quartz::rendering::Material::getMasterMaterialList().size(); ++i) {
        quartz::rendering::Material::UniformBufferObject materialUBO(*(quartz::rendering::Material::getMasterMaterialList()[i]));
        const uint32_t addressOffsetBytes = objectStride * i;
        uint8_t* p_UBODestination = p_bufferStart + addressOffsetBytes;

        memcpy(p_UBODestination, &materialUBO, numBytesToCopy);
    }
    m_doodadRenderingPipeline.updateUniformBuffer(m_currentInFlightFrameIndex, 7, alignedMaterialUBOBytes.data());

    // reset //

    m_renderingSwapchain.resetInFlightFence(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    m_renderingSwapchain.resetAndBeginDrawingCommandBuffer(
        m_renderingWindow,
        m_renderingRenderPass,
        m_currentInFlightFrameIndex,
        availableSwapchainImageIndex
    );

    // skybox pipeline //

    m_renderingSwapchain.bindPipelineToDrawingCommandBuffer(
        m_renderingWindow,
        m_skyBoxRenderingPipeline,
        m_currentInFlightFrameIndex
    );

    m_renderingSwapchain.recordSkyBoxToDrawingCommandBuffer(
        m_skyBoxRenderingPipeline,
        scene.getSkyBox(),
        m_currentInFlightFrameIndex
    );

    // doodad drawing pipeline //

    m_renderingSwapchain.bindPipelineToDrawingCommandBuffer(
        m_renderingWindow,
        m_doodadRenderingPipeline,
        m_currentInFlightFrameIndex
    );

    for (const quartz::scene::Doodad& doodad : scene.getDoodads()) {
        m_renderingSwapchain.recordDoodadToDrawingCommandBuffer(
            m_renderingDevice,
            m_doodadRenderingPipeline,
            doodad,
            m_currentInFlightFrameIndex
        );
    }

    // submit //

    m_renderingSwapchain.endAndSubmitDrawingCommandBuffer(
        m_renderingDevice,
        m_currentInFlightFrameIndex
    );

    m_renderingSwapchain.presentImage(
        m_renderingDevice,
        m_currentInFlightFrameIndex,
        availableSwapchainImageIndex
    );

    if (m_renderingSwapchain.getShouldRecreate() || m_renderingWindow.getWasResized()) {
        recreateSwapchain();
        return;
    }

    m_currentInFlightFrameIndex = (m_currentInFlightFrameIndex + 1) % m_maxNumFramesInFlight;
}

void
quartz::rendering::Context::recreateSwapchain() {
    LOG_FUNCTION_SCOPE_INFOthis("");
    m_renderingDevice.waitIdle();

    m_renderingSwapchain.reset();
    m_doodadRenderingPipeline.reset();
    m_renderingRenderPass.reset();
    m_renderingWindow.reset();

    m_renderingWindow.recreate(
        m_renderingInstance,
        m_renderingDevice
    );
    m_renderingRenderPass.recreate(
        m_renderingDevice,
        m_renderingWindow
    );
    m_skyBoxRenderingPipeline.recreate(
        m_renderingDevice,
        m_renderingRenderPass
    );
    m_doodadRenderingPipeline.recreate(
        m_renderingDevice,
        m_renderingRenderPass
    );
    m_renderingSwapchain.recreate(
        m_renderingDevice,
        m_renderingWindow,
        m_renderingRenderPass,
        m_maxNumFramesInFlight
    );
}

void
quartz::rendering::Context::finish() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
    m_renderingDevice.waitIdle();
}

