#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferUtil.hpp"
#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

void
quartz::rendering::StagedImageBuffer::transitionImageLayout(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const vk::UniqueImage& p_image,
    const vk::ImageLayout inputLayout,
    const vk::ImageLayout outputLayout
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::UniqueCommandPool p_commandPool =
        quartz::rendering::VulkanUtil::createVulkanCommandPoolPtr(
            graphicsQueueFamilyIndex,
            p_logicalDevice,
            vk::CommandPoolCreateFlagBits::eTransient
        );

    vk::UniqueCommandBuffer p_commandBuffer = std::move(
        quartz::rendering::VulkanUtil::allocateVulkanCommandBufferPtr(
            p_logicalDevice,
            p_commandPool,
            1
        )[0]
    );

    LOG_TRACE(
        BUFFER,
        "Recording commands to newly created command buffer for transitioning "
        "image's layout"
    );

    vk::CommandBufferBeginInfo commandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    );
    p_commandBuffer->begin(commandBufferBeginInfo);

    vk::AccessFlags sourceAccessMask;
    vk::AccessFlags destinationAccessMask;
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;
    if (
        inputLayout == vk::ImageLayout::eUndefined &&
        outputLayout == vk::ImageLayout::eTransferDstOptimal
    ) {
        LOG_TRACE(
            BUFFER,
            "Transferring image from undefined layout to optimal transfer "
            "destination layout"
        );

        destinationAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (
        inputLayout == vk::ImageLayout::eTransferDstOptimal &&
        outputLayout == vk::ImageLayout::eShaderReadOnlyOptimal
    ) {
        LOG_TRACE(
            BUFFER,
            "Transferring image from optimal transfer destination layout to "
            "optimal shader read only format"
        );

        sourceAccessMask = vk::AccessFlagBits::eTransferWrite;
        destinationAccessMask = vk::AccessFlagBits::eShaderRead;

        sourceStage = vk::PipelineStageFlagBits::eTransfer;
        destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
    } else {
        LOG_CRITICAL(BUFFER, "Unsupported image layout transition");
        throw std::runtime_error("");
    }

    vk::ImageMemoryBarrier imageMemoryBarrier(
        sourceAccessMask,
        destinationAccessMask,
        inputLayout,
        outputLayout,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        *p_image,
        {
            vk::ImageAspectFlagBits::eColor,
            0,
            1,
            0,
            1
        }
    );

    p_commandBuffer->pipelineBarrier(
        sourceStage,
        destinationStage,
        {},
        {},
        {},
        imageMemoryBarrier
    );

    p_commandBuffer->end();

    quartz::rendering::BufferUtil::submitVulkanCommandBufferPtr(
        graphicsQueue,
        p_commandBuffer
    );

    LOG_TRACE(BUFFER, "Successfully transitioned image's layout");
}

void
quartz::rendering::StagedImageBuffer::populateVulkanImageWithStagedData(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::UniqueBuffer& p_stagingBuffer,
    const vk::UniqueImage& p_image
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::UniqueCommandPool p_commandPool =
        quartz::rendering::VulkanUtil::createVulkanCommandPoolPtr(
            graphicsQueueFamilyIndex,
            p_logicalDevice,
            vk::CommandPoolCreateFlagBits::eTransient
        );

    vk::UniqueCommandBuffer p_commandBuffer = std::move(
        quartz::rendering::VulkanUtil::allocateVulkanCommandBufferPtr(
            p_logicalDevice,
            p_commandPool,
            1
        )[0]
    );

    LOG_TRACE(
        BUFFER,
        "Recording commands to newly created command buffer for copying data "
        "from staged buffer to image"
    );

    vk::CommandBufferBeginInfo commandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    );
    p_commandBuffer->begin(commandBufferBeginInfo);

    vk::BufferImageCopy bufferImageCopy(
        0,
        0,
        0,
        {
            vk::ImageAspectFlagBits::eColor,
            0,
            0,
            1
        },
        {
            0,
            0,
            0
        },
        {
            imageWidth,
            imageHeight,
            1
        }
    );

    p_commandBuffer->copyBufferToImage(
        *p_stagingBuffer,
        *p_image,
        vk::ImageLayout::eTransferDstOptimal,
        bufferImageCopy
    );

    p_commandBuffer->end();

    quartz::rendering::BufferUtil::submitVulkanCommandBufferPtr(
        graphicsQueue,
        p_commandBuffer
    );

    LOG_TRACE(BUFFER, "Successfully copied data");
}

vk::UniqueDeviceMemory
quartz::rendering::StagedImageBuffer::allocateVulkanPhysicalDeviceImageMemoryAndPopulateWithStagedData(
    const vk::PhysicalDevice& physicalDevice,
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::UniqueBuffer& p_stagingBuffer,
    const vk::UniqueImage& p_image,
    const vk::MemoryPropertyFlags requiredMemoryProperties
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::UniqueDeviceMemory p_vulkanPhysicalDeviceTextureMemory =
        quartz::rendering::ImageBufferUtil::allocateVulkanPhysicalDeviceImageMemory(
            physicalDevice,
            p_logicalDevice,
            p_image,
            requiredMemoryProperties
        );

    LOG_TRACE(BUFFER, "Transitioning layout and populating memory from buffer");

    quartz::rendering::StagedImageBuffer::transitionImageLayout(
        graphicsQueueFamilyIndex,
        p_logicalDevice,
        graphicsQueue,
        p_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );
    quartz::rendering::StagedImageBuffer::populateVulkanImageWithStagedData(
        graphicsQueueFamilyIndex,
        p_logicalDevice,
        graphicsQueue,
        imageWidth,
        imageHeight,
        p_stagingBuffer,
        p_image
    );
    quartz::rendering::StagedImageBuffer::transitionImageLayout(
        graphicsQueueFamilyIndex,
        p_logicalDevice,
        graphicsQueue,
        p_image,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    return p_vulkanPhysicalDeviceTextureMemory;
}

quartz::rendering::StagedImageBuffer::StagedImageBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const uint32_t channelCount,
    const uint32_t sizeBytes,
    const vk::ImageUsageFlags usageFlags,
    const vk::Format format,
    const vk::ImageTiling tiling,
    const void* p_bufferData
) :
    m_imageWidth(imageWidth),
    m_imageHeight(imageHeight),
    m_channelCount(channelCount),
    m_sizeBytes(sizeBytes),
    m_usageFlags(usageFlags),
    m_format(format),
    m_tiling(tiling),
    mp_vulkanLogicalStagingBuffer(
        quartz::rendering::BufferUtil::createVulkanBufferPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_sizeBytes,
            vk::BufferUsageFlagBits::eTransferSrc
        )
    ),
    mp_vulkanPhysicalDeviceStagingMemory(
        quartz::rendering::BufferUtil::allocateVulkanPhysicalDeviceStagingMemoryPtr(
            renderingDevice.getVulkanPhysicalDevice(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_sizeBytes,
            p_bufferData,
            mp_vulkanLogicalStagingBuffer,
            {
                vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent
            }
        )
    ),
    mp_vulkanImage(
        quartz::rendering::ImageBufferUtil::createVulkanImagePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_imageWidth,
            m_imageHeight,
            vk::ImageUsageFlagBits::eTransferDst | m_usageFlags,
            m_format,
            m_tiling
        )
    ),
    mp_vulkanPhysicalDeviceMemory(
        quartz::rendering::StagedImageBuffer::allocateVulkanPhysicalDeviceImageMemoryAndPopulateWithStagedData(
            renderingDevice.getVulkanPhysicalDevice(),
            renderingDevice.getGraphicsQueueFamilyIndex(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingDevice.getVulkanGraphicsQueue(),
            m_imageWidth,
            m_imageHeight,
            mp_vulkanLogicalStagingBuffer,
            mp_vulkanImage,
            vk::MemoryPropertyFlagBits::eDeviceLocal
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::StagedImageBuffer::StagedImageBuffer(
    quartz::rendering::StagedImageBuffer&& other
) :
    m_imageWidth(
        other.m_imageWidth
    ),
    m_imageHeight(
        other.m_imageHeight
    ),
    m_channelCount(
        other.m_channelCount
    ),
    m_sizeBytes(
        other.m_sizeBytes
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
    mp_vulkanLogicalStagingBuffer(std::move(
        other.mp_vulkanLogicalStagingBuffer
    )),
    mp_vulkanPhysicalDeviceStagingMemory(std::move(
        other.mp_vulkanPhysicalDeviceStagingMemory
    )),
    mp_vulkanImage(std::move(
        other.mp_vulkanImage
    )),
    mp_vulkanPhysicalDeviceMemory(std::move(
        other.mp_vulkanPhysicalDeviceMemory
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::StagedImageBuffer::~StagedImageBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}