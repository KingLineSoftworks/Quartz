#include <vulkan/vulkan.hpp>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/BufferUtil.hpp"

std::string
quartz::rendering::BufferUtil::getUsageFlagsString(
    const vk::BufferUsageFlags bufferUsageFlags
) {
    std::string usageFlagsString = "[ ";

    if (bufferUsageFlags & vk::BufferUsageFlagBits::eUniformBuffer) {
        usageFlagsString += "UniformBuffer ";
    }
    if (bufferUsageFlags & vk::BufferUsageFlagBits::eUniformTexelBuffer) {
        usageFlagsString += "UniformTexelBuffer ";
    }
    if (bufferUsageFlags & vk::BufferUsageFlagBits::eIndexBuffer) {
        usageFlagsString += "IndexBuffer ";
    }
    if (bufferUsageFlags & vk::BufferUsageFlagBits::eVertexBuffer) {
        usageFlagsString += "VertexBuffer ";
    }

    if (bufferUsageFlags & vk::BufferUsageFlagBits::eStorageBuffer) {
        usageFlagsString += "StorageBuffer ";
    }
    if (bufferUsageFlags & vk::BufferUsageFlagBits::eStorageTexelBuffer) {
        usageFlagsString += "StorageTexelBuffer ";
    }

    if (bufferUsageFlags & vk::BufferUsageFlagBits::eTransferSrc) {
        usageFlagsString += "TransferSrc ";
    }
    if (bufferUsageFlags & vk::BufferUsageFlagBits::eTransferDst) {
        usageFlagsString += "TransferDst ";
    }

    usageFlagsString += "]";

    return usageFlagsString;
}

vk::UniqueBuffer
quartz::rendering::BufferUtil::createVulkanBufferPtr(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::BufferUsageFlags bufferUsageFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    vk::BufferCreateInfo bufferCreateInfo(
        {},
        sizeBytes,
        bufferUsageFlags,
        vk::SharingMode::eExclusive
    );

    LOG_TRACE(BUFFER, "Attempting to create vk::Buffer ({} buffer)", quartz::rendering::BufferUtil::getUsageFlagsString(bufferUsageFlags));
    vk::UniqueBuffer p_buffer = p_logicalDevice->createBufferUnique(bufferCreateInfo);

    if (!p_buffer) {
        LOG_THROW(BUFFER, util::VulkanCreationFailedError, "Failed to create vk::Buffer");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::Buffer instance at {}", static_cast<void*>(&(*p_buffer)));

    return p_buffer;
}

uint32_t
quartz::rendering::BufferUtil::chooseMemoryTypeIndex(
    const vk::PhysicalDevice& physicalDevice,
    const vk::MemoryPropertyFlags requiredMemoryProperties,
    const vk::MemoryRequirements& memoryRequirements
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();

    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) &&
            physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requiredMemoryProperties
        ) {
            chosenMemoryTypeIndex = i;
            break;
        }
    }
    if (!chosenMemoryTypeIndex.has_value()) {
        LOG_THROW(BUFFER, util::VulkanFeatureNotSupportedError, "Failed to find a suitable memory type");
    }

    return chosenMemoryTypeIndex.value();
}


vk::UniqueDeviceMemory
quartz::rendering::BufferUtil::allocateVulkanPhysicalDeviceMemoryPtr(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags requiredMemoryProperties
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    vk::MemoryRequirements memoryRequirements = p_logicalDevice->getBufferMemoryRequirements(*p_logicalBuffer);

    uint32_t chosenMemoryTypeIndex = quartz::rendering::BufferUtil::chooseMemoryTypeIndex(
        physicalDevice,
        requiredMemoryProperties,
        memoryRequirements
    );

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex
    );

    LOG_TRACE(BUFFER, "Attempting to allocate vk::DeviceMemory");
    vk::UniqueDeviceMemory p_logicalBufferPhysicalMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!p_logicalBufferPhysicalMemory) {
        LOG_THROW(BUFFER, util::VulkanCreationFailedError, "Failed to allocated vk::DeviceMemory");
    }
    LOG_TRACE(BUFFER, "Successfully allocated vk::DeviceMemory instance at {}", static_cast<void*>(&(*p_logicalBufferPhysicalMemory)));

    LOG_TRACE(BUFFER, "Binding memory to logical device");
    p_logicalDevice->bindBufferMemory(
        *p_logicalBuffer,
        *p_logicalBufferPhysicalMemory,
        0
    );

    return p_logicalBufferPhysicalMemory;
}

void
quartz::rendering::BufferUtil::populateVulkanPhysicalDeviceMemoryWithLocalData(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const void* p_bufferData,
    vk::UniqueDeviceMemory& p_physicalMemory
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    LOG_TRACE(BUFFER, "Memory *IS* allocated for a source buffer. Populating device's buffer memory with input raw data");

    LOG_TRACE(BUFFER, "  - Creating mapping for allocated device memory of size {}", sizeBytes);
    void* p_mappedDestinationDeviceMemory = p_logicalDevice->mapMemory(
        *p_physicalMemory,
        0,
        sizeBytes
    );

    LOG_TRACE(BUFFER, "  - Copying {} bytes to mapped device memory at {} from buffer at {}", sizeBytes, p_mappedDestinationDeviceMemory, p_bufferData);
    memcpy(
        p_mappedDestinationDeviceMemory,
        p_bufferData,
        sizeBytes
    );

    LOG_TRACE(BUFFER, "  - Unmapping device memory");
    p_logicalDevice->unmapMemory(*p_physicalMemory);

    LOG_TRACE(BUFFER, "Successfully copied input data to device buffer memory");
}

vk::UniqueDeviceMemory
quartz::rendering::BufferUtil::allocateVulkanPhysicalDeviceStagingMemoryPtr(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const void* p_bufferData,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags requiredMemoryProperties
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    vk::UniqueDeviceMemory p_logicalBufferPhysicalMemory = quartz::rendering::BufferUtil::allocateVulkanPhysicalDeviceMemoryPtr(
        physicalDevice,
        p_logicalDevice,
        sizeBytes,
        p_logicalBuffer,
        requiredMemoryProperties
    );

    quartz::rendering::BufferUtil::populateVulkanPhysicalDeviceMemoryWithLocalData(
        p_logicalDevice,
        sizeBytes,
        p_bufferData,
        p_logicalBufferPhysicalMemory
    );

    return p_logicalBufferPhysicalMemory;
}

void
quartz::rendering::BufferUtil::submitVulkanCommandBufferPtr(
    const vk::Queue& graphicsQueue,
    const vk::UniqueCommandBuffer& p_commandBuffer
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::SubmitInfo submitInfo(
        0,
        nullptr,
        nullptr,
        1,
        &(*p_commandBuffer),
        0,
        nullptr
    );
    graphicsQueue.submit(submitInfo, VK_NULL_HANDLE);
    graphicsQueue.waitIdle();
}

vk::UniqueImage
quartz::rendering::ImageBufferUtil::createVulkanImagePtr(
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

    vk::UniqueImage p_vulkanImage = p_logicalDevice->createImageUnique(imageCreateInfo);
    if (!p_vulkanImage) {
        LOG_THROW(BUFFER, util::VulkanCreationFailedError, "Failed to create vk::Image");
    }

    return p_vulkanImage;
}

vk::UniqueDeviceMemory
quartz::rendering::ImageBufferUtil::allocateVulkanPhysicalDeviceImageMemory(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueImage& p_image,
    const vk::MemoryPropertyFlags requiredMemoryProperties
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    vk::MemoryRequirements memoryRequirements = p_logicalDevice->getImageMemoryRequirements(*p_image);

    uint32_t chosenMemoryTypeIndex = quartz::rendering::BufferUtil::chooseMemoryTypeIndex(
        physicalDevice,
        requiredMemoryProperties,
        memoryRequirements
    );

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex
    );

    vk::UniqueDeviceMemory p_vulkanPhysicalDeviceImageMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);
    if (!p_vulkanPhysicalDeviceImageMemory) {
        LOG_THROW(BUFFER, util::VulkanCreationFailedError, "Failed to create vk::DeviceMemory for vk::Image");
    }

    p_logicalDevice->bindImageMemory(
        *p_image,
        *p_vulkanPhysicalDeviceImageMemory,
        0
    );

    return p_vulkanPhysicalDeviceImageMemory;
}