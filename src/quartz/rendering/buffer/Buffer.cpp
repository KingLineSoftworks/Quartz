#include <vulkan/vulkan.hpp>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/Buffer.hpp"

std::string
quartz::rendering::BufferHelper::getUsageFlagsString(
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
quartz::rendering::BufferHelper::createVulkanBufferUniquePtr(
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

    LOG_TRACE(
        BUFFER, "Attempting to create vk::Buffer ({} buffer)",
        quartz::rendering::BufferHelper::getUsageFlagsString(bufferUsageFlags)
    );
    vk::UniqueBuffer p_buffer = p_logicalDevice->createBufferUnique(
        bufferCreateInfo
    );

    if (!p_buffer) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::Buffer");
        throw std::runtime_error("");
    }
    LOG_TRACE(
        BUFFER, "Successfully created vk::Buffer instance at {}",
        static_cast<void*>(&(*p_buffer))
    );

    return p_buffer;
}

vk::UniqueDeviceMemory
quartz::rendering::BufferHelper::allocateVulkanPhysicalDeviceStagingMemoryUniquePtr(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const void* p_bufferData,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags memoryPropertyFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    vk::MemoryRequirements memoryRequirements =
        p_logicalDevice->getBufferMemoryRequirements(*p_logicalBuffer);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties =
        physicalDevice.getMemoryProperties();
    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (
        uint32_t i = 0;
        i < physicalDeviceMemoryProperties.memoryTypeCount;
        ++i
    ) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) &&
            (
                physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags &
                memoryPropertyFlags
            )
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

    LOG_TRACE(BUFFER, "Attempting to allocate vk::DeviceMemory");
    vk::UniqueDeviceMemory p_bufferMemory =
        p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!p_bufferMemory) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::DeviceMemory");

    p_logicalDevice->bindBufferMemory(
        *p_logicalBuffer,
        *p_bufferMemory,
        0
    );

    LOG_TRACE(
        BUFFER,
        "Memory *IS* allocated for a source buffer. Populating device's buffer "
        "memory with input raw data"
    );

    LOG_TRACE(
        BUFFER,
        "  - Creating mapping for allocated device memory of size {}",
        sizeBytes
    );
    void* p_mappedDestinationDeviceMemory = p_logicalDevice->mapMemory(
        *p_bufferMemory,
        0,
        sizeBytes
    );

    LOG_TRACE(
        BUFFER,
        "  - Copying {} bytes to mapped device memory at {} from buffer at {}",
        sizeBytes, p_mappedDestinationDeviceMemory, p_bufferData
    );
    memcpy(
        p_mappedDestinationDeviceMemory,
        p_bufferData,
        sizeBytes
    );

    LOG_TRACE(BUFFER, "  - Unmapping device memory");
    p_logicalDevice->unmapMemory(*p_bufferMemory);

    LOG_TRACE(BUFFER, "Successfully copied input data to device buffer memory");
    return p_bufferMemory;
}
