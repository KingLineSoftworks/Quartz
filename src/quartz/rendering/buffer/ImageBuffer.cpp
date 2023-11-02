#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferHelper.hpp"
#include "quartz/rendering/buffer/ImageBuffer.hpp"

quartz::rendering::ImageBuffer::ImageBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::ImageUsageFlags usageFlags,
    const vk::Format format,
    const vk::ImageTiling tiling
) :
    m_imageWidth(imageWidth),
    m_imageHeight(imageHeight),
    m_usageFlags(usageFlags),
    m_format(format),
    m_tiling(tiling),
    mp_vulkanImage(
        quartz::rendering::ImageBufferHelper::createVulkanImagePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_imageWidth,
            m_imageHeight,
            m_usageFlags,
            m_format,
            m_tiling
        )
    ),
    mp_vulkanPhysicalDeviceMemory(
        quartz::rendering::ImageBufferHelper::allocateVulkanPhysicalDeviceImageMemory(
            renderingDevice.getVulkanPhysicalDevice(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            mp_vulkanImage,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::ImageBuffer::ImageBuffer(
    quartz::rendering::ImageBuffer&& other
) :
    m_imageWidth(
        other.m_imageWidth
    ),
    m_imageHeight(
        other.m_imageHeight
    ),
    m_usageFlags(
        other.m_usageFlags
    ),
    m_format(
        other.m_format
    ),
    m_tiling(
        other.m_tiling
    ),
    mp_vulkanImage(std::move(
        other.mp_vulkanImage
    )),
    mp_vulkanPhysicalDeviceMemory(std::move(
        other.mp_vulkanPhysicalDeviceMemory
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::ImageBuffer&
quartz::rendering::ImageBuffer::operator=(
    quartz::rendering::ImageBuffer&& other
) {
    LOG_FUNCTION_CALL_TRACEthis("");

    if (this == &other) {
        return *this;
    }

    m_imageWidth = other.m_imageWidth;
    m_imageHeight = other.m_imageHeight;
    m_usageFlags = other.m_usageFlags;
    m_format = other.m_format;
    m_tiling = other.m_tiling;

    mp_vulkanImage = std::move(
        other.mp_vulkanImage
    );
    mp_vulkanPhysicalDeviceMemory = std::move(
        other.mp_vulkanPhysicalDeviceMemory
    );

    return *this;
}

quartz::rendering::ImageBuffer::~ImageBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::rendering::ImageBuffer::reset() {
    LOG_FUNCTION_CALL_TRACEthis("");

    mp_vulkanPhysicalDeviceMemory.reset();
    mp_vulkanImage.reset();
}