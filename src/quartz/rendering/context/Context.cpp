#include <string>

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Context.hpp"
#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/pipeline/Pipeline.hpp"
#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"
#include "quartz/rendering/pipeline/UniformSamplerInfo.hpp"
#include "quartz/rendering/pipeline/UniformTextureArrayInfo.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"
#include "quartz/scene/light/PointLight.hpp"
#include "quartz/scene/light/SpotLight.hpp"

quartz::rendering::Pipeline
quartz::rendering::Context::createDoodadRenderingPipeline(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow,
    const quartz::rendering::RenderPass& renderingRenderPass,
    const uint32_t maxNumFramesInFlight
) {
    std::vector<quartz::rendering::UniformBufferInfo> uniformBufferInfos;

    uniformBufferInfos.emplace_back(
        renderingDevice,
        sizeof(quartz::scene::Camera::UniformBufferObject),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        0,
        1,
        sizeof(quartz::scene::Camera::UniformBufferObject),
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
    );

    uniformBufferInfos.emplace_back(
        renderingDevice,
        sizeof(quartz::scene::AmbientLight),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        1,
        1,
        sizeof(quartz::scene::AmbientLight),
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eFragment
    );

    uniformBufferInfos.emplace_back(
        renderingDevice,
        sizeof(quartz::scene::DirectionalLight),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        2,
        1,
        sizeof(quartz::scene::DirectionalLight),
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eFragment
    );

    uniformBufferInfos.emplace_back(
        renderingDevice,
        sizeof(uint32_t),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        3,
        1,
        sizeof(uint32_t),
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eFragment
    );

    uniformBufferInfos.emplace_back(
        renderingDevice,
        sizeof(quartz::scene::PointLight) * QUARTZ_MAX_NUMBER_POINT_LIGHTS,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        4,
        1,
        sizeof(quartz::scene::PointLight),
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eFragment
    );

    uniformBufferInfos.emplace_back(
        renderingDevice,
        sizeof(uint32_t),
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        5,
        1,
        sizeof(uint32_t),
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eFragment
    );

    uniformBufferInfos.emplace_back(
        renderingDevice,
        sizeof(quartz::scene::SpotLight) * QUARTZ_MAX_NUMBER_SPOT_LIGHTS,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        6,
        1,
        sizeof(quartz::scene::SpotLight),
        vk::DescriptorType::eUniformBuffer,
        vk::ShaderStageFlagBits::eFragment
    );

    const uint32_t materialByteStride = quartz::rendering::UniformBufferInfo::calculateDynamicUniformBufferByteStride(renderingDevice, sizeof(quartz::rendering::Material::UniformBufferObject));
    uniformBufferInfos.emplace_back(
        renderingDevice,
        materialByteStride * QUARTZ_MAX_NUMBER_MATERIALS,
        vk::BufferUsageFlagBits::eUniformBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible,
        9,
        1,
        materialByteStride,
        vk::DescriptorType::eUniformBufferDynamic,
        vk::ShaderStageFlagBits::eFragment
    );

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

    return {
        renderingDevice,
        renderingWindow,
        renderingRenderPass,
        util::FileSystem::getCompiledShaderAbsoluteFilepath("shader.vert"),
        util::FileSystem::getCompiledShaderAbsoluteFilepath("shader.frag"),
        maxNumFramesInFlight,
        std::move(uniformBufferInfos),
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
    
    m_doodadRenderingPipeline.allocateVulkanDescriptorSets(
        m_renderingDevice,
        quartz::rendering::Texture::getMasterTextureList(),
        m_maxNumFramesInFlight
    );

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

    // update doodad drawing pipeline //

    m_doodadRenderingPipeline.updateCameraUniformBuffer(scene.getCamera(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateAmbientLightUniformBuffer(scene.getAmbientLight(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateDirectionalLightUniformBuffer(scene.getDirectionalLight(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updatePointLightUniformBuffer(scene.getPointLights(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateSpotLightUniformBuffer(scene.getSpotLights(), m_currentInFlightFrameIndex);
    m_doodadRenderingPipeline.updateMaterialArrayUniformBuffer(m_renderingDevice.getVulkanPhysicalDevice().getProperties().limits.minUniformBufferOffsetAlignment, m_currentInFlightFrameIndex);

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