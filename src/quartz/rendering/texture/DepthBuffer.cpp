#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/ImageBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/DepthBuffer.hpp"

vk::UniqueImageView
quartz::rendering::DepthBuffer::createVulkanImageViewPtr(
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

quartz::rendering::DepthBuffer::DepthBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::ImageUsageFlags usageFlags,
    const vk::Format format,
    const vk::ImageTiling tiling
) :
    m_imageWidth(imageWidth),
    m_imageHeight(imageHeight),
    m_imageBuffer(
        renderingDevice,
        m_imageWidth,
        m_imageHeight,
        usageFlags,
        format,
        tiling
    ),
    mp_vulkanImageView(
        quartz::rendering::DepthBuffer::createVulkanImageViewPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_imageBuffer.getVulkanFormat(),
            m_imageBuffer.getVulkanImagePtr()
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::DepthBuffer::DepthBuffer(
    quartz::rendering::DepthBuffer&& other
) :
    m_imageWidth(other.m_imageWidth),
    m_imageHeight(other.m_imageHeight),
    m_imageBuffer(std::move(other.m_imageBuffer)),
    mp_vulkanImageView(std::move(other.mp_vulkanImageView))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::DepthBuffer::~DepthBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::DepthBuffer&
quartz::rendering::DepthBuffer::operator=(
    quartz::rendering::DepthBuffer&& other
) {
    LOG_FUNCTION_CALL_TRACEthis("");

    if (this == &other) {
        return *this;
    }

    m_imageWidth = other.m_imageWidth;
    m_imageHeight = other.m_imageHeight;
    m_imageBuffer = std::move(other.m_imageBuffer);
    mp_vulkanImageView = std::move(other.mp_vulkanImageView);

    return *this;
}

void
quartz::rendering::DepthBuffer::reset() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    m_imageBuffer.reset();
    mp_vulkanImageView.reset();
}
