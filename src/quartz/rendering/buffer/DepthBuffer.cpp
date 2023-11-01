#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/Buffer.hpp"
#include "quartz/rendering/buffer/DepthBuffer.hpp"

vk::UniqueImage
quartz::rendering::DepthBuffer::createVulkanImagePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::ImageUsageFlags usageFlags,
    const vk::Format format,
    const vk::ImageTiling tiling
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::ImageCreateInfo imageCreateInfo(
        {},
        vk::ImageType::e2D,
        format,
        {
            static_cast<uint32_t>(imageWidth),
            static_cast<uint32_t>(imageHeight),
            1
        },
        1,
        1,
        vk::SampleCountFlagBits::e1,
        tiling,
        usageFlags,
        vk::SharingMode::eExclusive
    );

    LOG_TRACE(BUFFER, "Attempting to create vk::Image");
    vk::UniqueImage p_vulkanImage = p_logicalDevice->createImageUnique(imageCreateInfo);
    if (!p_vulkanImage) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::Image");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::Image");

    return p_vulkanImage;
}


vk::UniqueDeviceMemory
quartz::rendering::DepthBuffer::allocateVulkanPhysicalDeviceImageMemory(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueImage& p_image,
    const vk::MemoryPropertyFlags memoryPropertyFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::MemoryRequirements memoryRequirements =
        p_logicalDevice->getImageMemoryRequirements(*p_image);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties =
        physicalDevice.getMemoryProperties();

    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) &&
            physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags
        ) {
            chosenMemoryTypeIndex = i;
            break;
        }
    }
    if (!chosenMemoryTypeIndex.has_value()) {
        LOG_CRITICAL(BUFFER, "Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACE(BUFFER, "Attempting to create vk::DeviceMemory");
    vk::UniqueDeviceMemory p_vulkanPhysicalDeviceDepthMemory =
        p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);
    if (!p_vulkanPhysicalDeviceDepthMemory) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::DeviceMemory");

    p_logicalDevice->bindImageMemory(
        *p_image,
        *p_vulkanPhysicalDeviceDepthMemory,
        0
    );

    return p_vulkanPhysicalDeviceDepthMemory;
}

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

    LOG_TRACE(TEXTURE, "Attempting to crate vk::ImageView");
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
    m_usageFlags(usageFlags),
    m_format(format),
    m_tiling(tiling),
    mp_vulkanImage(
        quartz::rendering::DepthBuffer::createVulkanImagePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_imageWidth,
            m_imageHeight,
            m_usageFlags,
            m_format,
            m_tiling
        )
    ),
    mp_vulkanPhysicalDeviceMemory(
        quartz::rendering::DepthBuffer::allocateVulkanPhysicalDeviceImageMemory(
            renderingDevice.getVulkanPhysicalDevice(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            mp_vulkanImage,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        )
    ),
    mp_vulkanImageView(
        quartz::rendering::DepthBuffer::createVulkanImageViewPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_format,
            mp_vulkanImage
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::DepthBuffer::DepthBuffer(
    quartz::rendering::DepthBuffer&& other
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
    )),
    mp_vulkanImageView(std::move(
        other.mp_vulkanImageView
    ))
{
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
    m_usageFlags = other.m_usageFlags;
    m_format = other.m_format;
    m_tiling = other.m_tiling;

    mp_vulkanImage = std::move(
        other.mp_vulkanImage
    );
    mp_vulkanPhysicalDeviceMemory = std::move(
        other.mp_vulkanPhysicalDeviceMemory
    );
    mp_vulkanImageView = std::move(
        other.mp_vulkanImageView
    );

    return *this;
}

quartz::rendering::DepthBuffer::~DepthBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::rendering::DepthBuffer::reset() {
    LOG_FUNCTION_CALL_TRACEthis("");

    mp_vulkanImageView.reset();
    mp_vulkanPhysicalDeviceMemory.reset();
    mp_vulkanImage.reset();
}