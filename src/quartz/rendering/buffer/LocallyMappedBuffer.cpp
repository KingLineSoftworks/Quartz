#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/Buffer.hpp"
#include "quartz/rendering/buffer/LocallyMappedBuffer.hpp"

vk::UniqueDeviceMemory
quartz::rendering::LocallyMappedBuffer::allocateVulkanPhysicalDeviceMemoryUniquePtr(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags requiredMemoryProperties
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    vk::MemoryRequirements memoryRequirements =
        p_logicalDevice->getBufferMemoryRequirements(*p_logicalBuffer);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties =
        physicalDevice.getMemoryProperties();

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
        LOG_CRITICAL(BUFFER, "Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACE(BUFFER, "Attempting to allocate vk::DeviceMemory");
    vk::UniqueDeviceMemory p_logicalBufferMemory =
        p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!p_logicalBufferMemory) {
        LOG_CRITICAL(BUFFER, "Failed to allocated vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully allocated vk::DeviceMemory instance at {}",
        static_cast<void*>(&(*p_logicalBufferMemory))
    );

    LOG_TRACE(BUFFER, "Binding memory to logical device");
    p_logicalDevice->bindBufferMemory(
        *p_logicalBuffer,
        *p_logicalBufferMemory,
        0
    );

    return p_logicalBufferMemory;
}

void*
quartz::rendering::LocallyMappedBuffer::mapVulkanPhysicalDeviceMemoryToLocalMemory(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::UniqueDeviceMemory& p_physicalDeviceMemory
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    LOG_TRACE(BUFFER, "Mapping memory from physical device memory to logical memory instance {}",
        static_cast<const void*>(&(*p_physicalDeviceMemory))
    );
    void* p_mappedLocalMemory = p_logicalDevice->mapMemory(
        *(p_physicalDeviceMemory),
        0,
        sizeBytes
    );

    LOG_TRACE(BUFFER, "Mapped to {}", p_mappedLocalMemory);
    return p_mappedLocalMemory;
}

quartz::rendering::LocallyMappedBuffer::LocallyMappedBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t sizeBytes,
    const vk::BufferUsageFlags usageFlags,
    const vk::MemoryPropertyFlags memoryPropertyFlags
) :
    m_sizeBytes(sizeBytes),
    m_usageFlags(usageFlags),
    m_memoryPropertyFlags(memoryPropertyFlags),
    mp_vulkanLogicalBuffer(
        quartz::rendering::BufferHelper::createVulkanBufferUniquePtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_sizeBytes,
            m_usageFlags
        )
    ),
    mp_vulkanPhysicalDeviceMemory(
        quartz::rendering::LocallyMappedBuffer::allocateVulkanPhysicalDeviceMemoryUniquePtr(
            renderingDevice.getVulkanPhysicalDevice(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_sizeBytes,
            mp_vulkanLogicalBuffer,
            memoryPropertyFlags
        )
    ),
    mp_mappedLocalMemory(
        quartz::rendering::LocallyMappedBuffer::mapVulkanPhysicalDeviceMemoryToLocalMemory(
            renderingDevice.getVulkanLogicalDevicePtr(),
            m_sizeBytes,
            mp_vulkanPhysicalDeviceMemory
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::LocallyMappedBuffer::LocallyMappedBuffer(
    quartz::rendering::LocallyMappedBuffer&& other
) :
    m_sizeBytes(
        other.m_sizeBytes
    ),
    m_usageFlags(
        other.m_usageFlags
    ),
    m_memoryPropertyFlags(
        other.m_memoryPropertyFlags
    ),
    mp_vulkanLogicalBuffer(std::move(
        other.mp_vulkanLogicalBuffer
    )),
    mp_vulkanPhysicalDeviceMemory(std::move(
        other.mp_vulkanPhysicalDeviceMemory
    )),
    mp_mappedLocalMemory(std::move(
        other.mp_mappedLocalMemory
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::LocallyMappedBuffer::~LocallyMappedBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}