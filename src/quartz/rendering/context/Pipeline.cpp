#include <vulkan/vulkan.hpp>

#include "util/file_system/FileSystem.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Pipeline.hpp"
#include "quartz/rendering/context/Window2.hpp"
#include "quartz/rendering/model/Vertex.hpp"

vk::UniqueShaderModule quartz::rendering::Pipeline::createVulkanShaderModuleUniquePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::PIPELINE, "{}", filepath);

    const std::vector<char> shaderBytes = util::FileSystem::readBytesFromFile(filepath);

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo(
        {},
        shaderBytes.size(),
        reinterpret_cast<const uint32_t*>(shaderBytes.data())
    );

    LOG_TRACE(quartz::loggers::PIPELINE, "Attempting to create vk::ShaderModule");
    vk::UniqueShaderModule p_shaderModule = p_logicalDevice->createShaderModuleUnique(shaderModuleCreateInfo);

    if (!p_shaderModule) {
        LOG_CRITICAL(quartz::loggers::PIPELINE, "Failed to create vk::ShaderModule");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::PIPELINE, "Successfully created vk::ShaderModule");

    return p_shaderModule;
}

std::vector<quartz::rendering::Buffer> quartz::rendering::Pipeline::createUniformBuffers(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t numBuffers,
    const uint32_t bufferSizeBytes
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::PIPELINE, "{} buffers", numBuffers);

    std::vector<quartz::rendering::Buffer> buffers;

    for (uint32_t i = 0; i < numBuffers; ++i) {
        LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::PIPELINE, "Creating uniform buffer {}", i);
        buffers.emplace_back(
            renderingDevice,
            bufferSizeBytes,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        );
    }

    return buffers;
}

vk::UniqueDescriptorSetLayout quartz::rendering::Pipeline::createVulkanDescriptorSetLayoutUniquePtr(
    const vk::UniqueDevice& p_logicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::PIPELINE, "");

    vk::DescriptorSetLayoutBinding layoutBinding(
        0,
        vk::DescriptorType::eUniformBuffer,
        1, vk::ShaderStageFlagBits::eVertex,
        {}
    );

    vk::DescriptorSetLayoutCreateInfo layoutCreateInfo(
        {},
        layoutBinding
    );

    LOG_TRACE(quartz::loggers::PIPELINE, "Attempting to create vk::DescriptorSetLayout");
    vk::UniqueDescriptorSetLayout p_descriptorSetLayout = p_logicalDevice->createDescriptorSetLayoutUnique(
        layoutCreateInfo
    );

    if (!p_descriptorSetLayout) {
        LOG_CRITICAL(quartz::loggers::PIPELINE, "Failed to create vk::DescriptorSetLayout");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::PIPELINE, "Successfully created vk::DescriptorSetLayout");

    return p_descriptorSetLayout;
}

vk::UniqueRenderPass quartz::rendering::Pipeline::createVulkanRenderPassUniquePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::SurfaceFormatKHR& surfaceFormat
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::PIPELINE, "");

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

    vk::SubpassDescription subpassDescription(
        {},
        vk::PipelineBindPoint::eGraphics,
        {},
        colorAttachmentRef,
        {},
        {},
        {}
    );

    vk::SubpassDependency subpassDependency(
        VK_SUBPASS_EXTERNAL,
        0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {},
        vk::AccessFlagBits::eColorAttachmentWrite,
        {}
    );

    vk::RenderPassCreateInfo renderPassCreateInfo(
        {},
        colorAttachment,
        subpassDescription,
        subpassDependency
    );

    LOG_TRACE(quartz::loggers::PIPELINE, "Attempting to create vk::RenderPass");
    vk::UniqueRenderPass p_renderPass = p_logicalDevice->createRenderPassUnique(renderPassCreateInfo);

    if (!p_renderPass) {
        LOG_CRITICAL(quartz::loggers::PIPELINE, "Failed to create vk::RenderPass");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::PIPELINE, "Successfully created vk::RenderPass");

    return p_renderPass;
}

vk::UniquePipelineLayout quartz::rendering::Pipeline::createVulkanPipelineLayoutUniquePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueDescriptorSetLayout& p_descriptorSetLayout
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::PIPELINE, "");

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        {},
        1,
        &(*p_descriptorSetLayout),
        0,
        nullptr
    );

    LOG_TRACE(quartz::loggers::PIPELINE, "Attempting to create vk::PipelineLayout");
    vk::UniquePipelineLayout p_pipelineLayout = p_logicalDevice->createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    if (!p_pipelineLayout) {
        LOG_CRITICAL(quartz::loggers::PIPELINE, "Failed to create vk::PipelineLayout");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::PIPELINE, "Successfully created vk::PipelineLayout");

    return p_pipelineLayout;
}

vk::UniquePipeline quartz::rendering::Pipeline::createVulkanGraphicsPipelineUniquePtr(
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
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::PIPELINE, "");

    // ----- shader stage tings ----- //

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineShaderStageCreateInfo(s)");
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

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineVertexInputStateCreateInfo");
    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo(
        {},
        vertexInputBindingDescriptions,
        vertexInputAttributeDescriptions
    );

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineInputAssemblyStateCreateInfo");
    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
        {},
        vk::PrimitiveTopology::eTriangleList,
        false
    );

    // ----- tessellation tings ----- //

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineTessellationStateCreateInfo");
    vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo = vk::PipelineTessellationStateCreateInfo();

    // ----- viewport and scissor tings ----- //

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineViewportStateCreateInfo");
    LOG_TRACE(quartz::loggers::PIPELINE, "  - using {} viewports", viewports.size());
    for (const vk::Viewport& viewport : viewports) {
        LOG_TRACE(quartz::loggers::PIPELINE, "    - {} x {}", viewport.width, viewport.height);
    }
    LOG_TRACE(quartz::loggers::PIPELINE, "  - using {} scissor rectangles", scissorRectangles.size());
    for (const vk::Rect2D& scissorRectangle : scissorRectangles) {
        LOG_TRACE(quartz::loggers::PIPELINE, "    - {} , {}", scissorRectangle.offset.x, scissorRectangle.offset.y);
    }
    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo(
        {},
        viewports,
        scissorRectangles
    );

    // ----- rasterizer tings ----- //

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineRasterizationStateCreateInfo");
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

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineMultisampleStateCreateInfo");
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

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineDepthStencilStateCreateInfo");
    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo();

    // ----- color blend tings ----- //

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineColorBlendStateCreateInfo");
    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo(
        {},
        false,
        vk::LogicOp::eCopy,
        colorBlendAttachmentStates,
        { 0.0f, 0.0f, 0.0f, 0.0f}
    );

    // ----- dynamic state tings ----- //

    LOG_TRACE(quartz::loggers::PIPELINE, "Creating vk::PipelineDynamicStateCreateInfo", dynamicStates.size());
    LOG_TRACE(quartz::loggers::PIPELINE, "  - using {} dynamic states", dynamicStates.size());
    for (const vk::DynamicState& dynamicState : dynamicStates) {
        LOG_TRACE(quartz::loggers::PIPELINE, "    - {}", static_cast<uint32_t>(dynamicState));
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

    LOG_TRACE(quartz::loggers::PIPELINE, "Attempting to create vk::Pipeline");
    vk::ResultValue<vk::UniquePipeline> graphicsPipelineCreationResult = p_logicalDevice->createGraphicsPipelineUnique(
        VK_NULL_HANDLE,
        graphicsPipelineCreateInfo
    );

    if (graphicsPipelineCreationResult.result != vk::Result::eSuccess) {
        LOG_CRITICAL(quartz::loggers::PIPELINE, "Failed to create vk::Pipeline");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::PIPELINE, "Successfully created vk::Pipeline");

    return std::move(graphicsPipelineCreationResult.value);
}

quartz::rendering::Pipeline::Pipeline(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window2& renderingWindow,
    const uint32_t maxNumFramesInFlight
) :
    m_maxNumFramesInFlight(maxNumFramesInFlight),
    m_vulkanVertexInputBindingDescriptions(quartz::rendering::Vertex::getVulkanVertexInputBindingDescription()),
    m_vulkanVertexInputAttributeDescriptions(quartz::rendering::Vertex::getVulkanVertexInputAttributeDescriptions()),
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
    mp_vulkanVertexShaderModule(quartz::rendering::Pipeline::createVulkanShaderModuleUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        util::FileSystem::getAbsoluteFilepathInProject("shader.vert.spv")
    )),
    mp_vulkanFragmentShaderModule(quartz::rendering::Pipeline::createVulkanShaderModuleUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        util::FileSystem::getAbsoluteFilepathInProject("shader.frag.spv")
    )),
    m_uniformBuffers(quartz::rendering::Pipeline::createUniformBuffers(
        renderingDevice,
        m_maxNumFramesInFlight,
        4
    )),
    mp_vulkanDescriptorSetLayout(quartz::rendering::Pipeline::createVulkanDescriptorSetLayoutUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr()
    )),
    m_vulkanDescriptorPoolPtr(),
    m_vulkanDescriptorSets(),
    mp_vulkanRenderPass(quartz::rendering::Pipeline::createVulkanRenderPassUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanSurfaceFormat()
    )),
    mp_vulkanPipelineLayout(quartz::rendering::Pipeline::createVulkanPipelineLayoutUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        mp_vulkanDescriptorSetLayout
    )),
    mp_vulkanUniqueGraphicsPipeline(quartz::rendering::Pipeline::createVulkanGraphicsPipelineUniquePtr(
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
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Pipeline::~Pipeline() {
    LOG_FUNCTION_CALL_TRACEthis("");
}