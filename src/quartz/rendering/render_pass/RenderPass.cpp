#include "quartz/rendering/render_pass/RenderPass.hpp"

vk::UniqueRenderPass
quartz::rendering::RenderPass::createVulkanRenderPassPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const vk::Format& depthFormat
) {
    LOG_FUNCTION_CALL_TRACE(RENDERPASS, "");

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
        LOG_THROW(PIPELINE, util::RichException<vk::RenderPassCreateInfo>, renderPassCreateInfo, "Failed to create vk::RenderPass");
    }

    return p_renderPass;
}

quartz::rendering::RenderPass::RenderPass(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow
) :
    mp_vulkanRenderPass(
        quartz::rendering::RenderPass::createVulkanRenderPassPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingWindow.getVulkanSurfaceFormat(),
            renderingWindow.getVulkanDepthBufferFormat()
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::RenderPass::~RenderPass() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::rendering::RenderPass::reset() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanRenderPass.reset();
}

void
quartz::rendering::RenderPass::recreate(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::Window& renderingWindow
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanRenderPass = quartz::rendering::RenderPass::createVulkanRenderPassPtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingWindow.getVulkanSurfaceFormat(),
        renderingWindow.getVulkanDepthBufferFormat()
    );
}
