#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/DepthBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/DepthBuffer.hpp"

vk::UniqueImageView
quartz::rendering::DepthBufferTex::createVulkanImageViewPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Format format,
    const vk::UniqueImage& p_image
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::ImageViewCreateInfo imageViewCreateInfo(
        {},
        *p_image,
        vk::ImageViewType::e2D,
        format,
        {},
        {
            vk::ImageAspectFlagBits::eDepth,
            0,
            1,
            0,
            1
        }
    );

    LOG_TRACE(TEXTURE, "Attempting to create vk::ImageView");
    vk::UniqueImageView p_imageView =
        p_logicalDevice->createImageViewUnique(imageViewCreateInfo);

    if (!p_imageView) {
        LOG_CRITICAL(TEXTURE, "Failed to create vk::ImageView");
        throw std::runtime_error("");
    }
    LOG_TRACE(TEXTURE, "Successfully created vk::ImageView");

    return p_imageView;
}

quartz::rendering::DepthBufferTex::DepthBufferTex(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::ImageUsageFlags usageFlags,
    const vk::Format format,
    const vk::ImageTiling tiling
) :
    m_imageWidth(imageWidth),
    m_imageHeight(imageHeight),
    m_depthBuffer(
        renderingDevice,
        m_imageWidth,
        m_imageHeight,
        usageFlags,
        format,
        tiling
    ),
    mp_vulkanImageView(
        quartz::rendering::DepthBufferTex::createVulkanImageViewPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_depthBuffer.getVulkanFormat(),
            m_depthBuffer.getVulkanImagePtr()
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::DepthBufferTex::DepthBufferTex(
    quartz::rendering::DepthBufferTex&& other
) :
    m_imageWidth(other.m_imageWidth),
    m_imageHeight(other.m_imageHeight),
    m_depthBuffer(std::move(other.m_depthBuffer)),
    mp_vulkanImageView(std::move(other.mp_vulkanImageView))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::DepthBufferTex::~DepthBufferTex() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::DepthBufferTex&
quartz::rendering::DepthBufferTex::operator=(
    quartz::rendering::DepthBufferTex&& other
) {
    LOG_FUNCTION_CALL_TRACEthis("");

    if (this == &other) {
        return *this;
    }

    m_imageWidth = other.m_imageWidth;
    m_imageHeight = other.m_imageHeight;
    m_depthBuffer = std::move(other.m_depthBuffer);
    mp_vulkanImageView = std::move(other.mp_vulkanImageView);

    return *this;
}

void
quartz::rendering::DepthBufferTex::reset() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    m_depthBuffer.reset();
    mp_vulkanImageView.reset();
}
