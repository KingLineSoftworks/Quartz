#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/ImageBuffer.hpp"
#include "quartz/rendering/depth_buffer/DepthBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

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
        quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            *(m_imageBuffer.getVulkanImagePtr()),
            m_imageBuffer.getVulkanFormat(),
            {},
            vk::ImageAspectFlagBits::eDepth
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
