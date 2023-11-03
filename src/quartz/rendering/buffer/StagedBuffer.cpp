#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferUtil.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

void
quartz::rendering::StagedBuffer::populateVulkanLogicalBufferWithStagedData(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const uint32_t sizeBytes,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::UniqueBuffer& p_logicalStagingBuffer
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    vk::UniqueCommandPool p_commandPool =
        quartz::rendering::VulkanUtil::createVulkanCommandPoolUniquePtr(
            graphicsQueueFamilyIndex,
            p_logicalDevice,
            vk::CommandPoolCreateFlagBits::eTransient
        );

    vk::UniqueCommandBuffer p_commandBuffer = std::move(
        quartz::rendering::VulkanUtil::allocateVulkanCommandBufferUniquePtr(
            p_logicalDevice,
            p_commandPool,
            1
        )[0]
    );

    LOG_TRACE(
        BUFFER,
        "Memory is *NOT* allocated for a source buffer. Populating with data "
        "from staged buffer instead"
    );

    vk::CommandBufferBeginInfo commandBufferBeginInfo(
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit
    );
    p_commandBuffer->begin(commandBufferBeginInfo);

    vk::BufferCopy bufferCopyRegion(
        0,
        0,
        sizeBytes
    );

    p_commandBuffer->copyBuffer(
        *p_logicalStagingBuffer,
        *p_logicalBuffer,
        bufferCopyRegion
    );

    p_commandBuffer->end();

    quartz::rendering::BufferUtil::submitVulkanCommandBufferUniquePtr(
        graphicsQueue,
        p_commandBuffer
    );

    LOG_TRACE(BUFFER, "Successfully copied data from staging buffer");
}

vk::UniqueDeviceMemory
quartz::rendering::StagedBuffer::allocateVulkanPhysicalDeviceDestinationMemoryUniquePtr(
    const vk::PhysicalDevice& physicalDevice,
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const uint32_t sizeBytes,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags requiredMemoryProperties,
    const vk::UniqueBuffer& p_logicalStagingBuffer
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);
    
    vk::UniqueDeviceMemory p_logicalBufferPhysicalMemory =
        quartz::rendering::BufferUtil::allocateVulkanPhysicalDeviceMemoryUniquePtr(
            physicalDevice,
            p_logicalDevice,
            sizeBytes,
            p_logicalBuffer,
            requiredMemoryProperties
        );

    quartz::rendering::StagedBuffer::populateVulkanLogicalBufferWithStagedData(
        graphicsQueueFamilyIndex,
        p_logicalDevice,
        graphicsQueue,
        sizeBytes,
        p_logicalBuffer,
        p_logicalStagingBuffer
    );

    return p_logicalBufferPhysicalMemory;
}

quartz::rendering::StagedBuffer::StagedBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t sizeBytes,
    const vk::BufferUsageFlags usageFlags,
    const void* p_bufferData
) :
    m_sizeBytes(sizeBytes),
    m_usageFlags(usageFlags),
    mp_vulkanLogicalStagingBuffer(
        quartz::rendering::BufferUtil::createVulkanBufferUniquePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_sizeBytes,
            vk::BufferUsageFlagBits::eTransferSrc
        )
    ),
    mp_vulkanPhysicalDeviceStagingMemory(
        quartz::rendering::BufferUtil::allocateVulkanPhysicalDeviceStagingMemoryUniquePtr(
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
    mp_vulkanLogicalBuffer(
        quartz::rendering::BufferUtil::createVulkanBufferUniquePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_sizeBytes,
            vk::BufferUsageFlagBits::eTransferDst | m_usageFlags
        )
    ),
    mp_vulkanPhysicalDeviceMemory(
        quartz::rendering::StagedBuffer::allocateVulkanPhysicalDeviceDestinationMemoryUniquePtr(
            renderingDevice.getVulkanPhysicalDevice(),
            renderingDevice.getGraphicsQueueFamilyIndex(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            renderingDevice.getVulkanGraphicsQueue(),
            m_sizeBytes,
            mp_vulkanLogicalBuffer,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            mp_vulkanLogicalStagingBuffer
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::StagedBuffer::StagedBuffer(
    quartz::rendering::StagedBuffer&& other
) :
    m_sizeBytes(
        other.m_sizeBytes
    ),
    m_usageFlags(
        other.m_usageFlags
    ),
    mp_vulkanLogicalStagingBuffer(std::move(
        other.mp_vulkanLogicalStagingBuffer
    )),
    mp_vulkanPhysicalDeviceStagingMemory(std::move(
        other.mp_vulkanPhysicalDeviceStagingMemory
    )),
    mp_vulkanLogicalBuffer(std::move(
        other.mp_vulkanLogicalBuffer
    )),
    mp_vulkanPhysicalDeviceMemory(std::move(
        other.mp_vulkanPhysicalDeviceMemory
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::StagedBuffer::~StagedBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}
