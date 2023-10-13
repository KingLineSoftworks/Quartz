#include <vulkan/vulkan.hpp>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"

std::string quartz::rendering::Buffer::getUsageFlagsString(const vk::BufferUsageFlags bufferUsageFlags) {
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

void quartz::rendering::Buffer::customMappedLocalMemoryDeleter(
    UNUSED void* p_mappedLocalMemory,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueDeviceMemory& p_physicalDeviceMemory
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "local memory at {}", p_mappedLocalMemory);

    LOG_TRACE(quartz::loggers::BUFFER, "Local memory pointer at {}", p_mappedLocalMemory);

    if (!p_logicalDevice) {
        LOG_ERROR(quartz::loggers::BUFFER, "Logical device instance is invalid");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Logical device instance at {}", static_cast<const void*>(&(*p_logicalDevice)));

    if (!p_physicalDeviceMemory) {
        LOG_ERROR(quartz::loggers::BUFFER, "Physical device memory instance is invalid");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Physical device memory instance at {}", static_cast<const void*>(&(*p_physicalDeviceMemory)));

    p_logicalDevice->unmapMemory(*p_physicalDeviceMemory);
}

vk::UniqueBuffer quartz::rendering::Buffer::createVulkanBufferUniquePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::BufferUsageFlags bufferUsageFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "{} bytes", sizeBytes);

    vk::BufferCreateInfo bufferCreateInfo(
        {},
        sizeBytes,
        bufferUsageFlags,
        vk::SharingMode::eExclusive
    );

    LOG_TRACE(quartz::loggers::BUFFER, "Attempting to create vk::Buffer ({} buffer)", quartz::rendering::Buffer::getUsageFlagsString(bufferUsageFlags));
    vk::UniqueBuffer p_buffer = p_logicalDevice->createBufferUnique(bufferCreateInfo);

    if (!p_buffer) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to create vk::Buffer");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully created vk::Buffer instance at {}", static_cast<void*>(&(*p_buffer)));

    return p_buffer;
}

vk::UniqueDeviceMemory quartz::rendering::Buffer::allocateVulkanPhysicalDeviceMemory(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags requiredMemoryProperties
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "{} bytes", sizeBytes);

    vk::MemoryRequirements memoryRequirements = p_logicalDevice->getBufferMemoryRequirements(*p_logicalBuffer);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();
    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) && // if it has our desired memory type bit set
            (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & requiredMemoryProperties)
            ) {
            chosenMemoryTypeIndex = i;
            break;
        }
    }
    if (!chosenMemoryTypeIndex.has_value()) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACE(quartz::loggers::BUFFER, "Attempting to allocate vk::DeviceMemory");
    vk::UniqueDeviceMemory p_logicalBufferMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!p_logicalBufferMemory) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to allocated vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully allocated vk::DeviceMemory instance at {}", static_cast<void*>(&(*p_logicalBufferMemory)));

    LOG_TRACE(quartz::loggers::BUFFER, "Binding memory to logical device");
    p_logicalDevice->bindBufferMemory(
        *p_logicalBuffer,
        *p_logicalBufferMemory,
        0
    );

    return p_logicalBufferMemory;
}

void* quartz::rendering::Buffer::mapVulkanPhysicalDeviceMemoryToLocalMemory(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::UniqueDeviceMemory& p_physicalDeviceMemory
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "");

//    std::function<void(void*)> customDeleter = [&p_logicalDevice, &p_physicalDeviceMemory] (void* p) {
//        quartz::rendering::Buffer::customMappedLocalMemoryDeleter(p, p_logicalDevice, p_physicalDeviceMemory);
//    };
//
//    LOG_TRACE(quartz::loggers::BUFFER, "Mapping memory from physical device memory to local memory instance {}", static_cast<const void*>(&(*p_physicalDeviceMemory)));
//    std::unique_ptr<void, std::function<void(void*)>> p_mappedLocalMemory(
//        p_logicalDevice->mapMemory(
//            *(p_physicalDeviceMemory),
//            0,
//            sizeBytes
//        ),
//        customDeleter
//    );

    LOG_TRACE(quartz::loggers::BUFFER, "Mapping memory from physical device memory to local memory instance {}", static_cast<const void*>(&(*p_physicalDeviceMemory)));
    void* p_mappedLocalMemory = p_logicalDevice->mapMemory(
        *(p_physicalDeviceMemory),
        0,
        sizeBytes
    );

    LOG_TRACE(quartz::loggers::BUFFER, "Mapped to {}", p_mappedLocalMemory);
    return p_mappedLocalMemory;
}

quartz::rendering::Buffer::Buffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t sizeBytes,
    const vk::BufferUsageFlags usageFlags,
    const vk::MemoryPropertyFlags memoryPropertyFlags
) :
    m_sizeBytes(sizeBytes),
    m_usageFlags(usageFlags),
    m_memoryPropertyFlags(memoryPropertyFlags),
    mp_vulkanLogicalBuffer(quartz::rendering::Buffer::createVulkanBufferUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        m_usageFlags
    )),
    mp_vulkanPhysicalDeviceMemory(quartz::rendering::Buffer::allocateVulkanPhysicalDeviceMemory(
        renderingDevice.getVulkanPhysicalDevice(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        mp_vulkanLogicalBuffer,
        memoryPropertyFlags
    )),
    mp_mappedLocalMemory(quartz::rendering::Buffer::mapVulkanPhysicalDeviceMemoryToLocalMemory(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        mp_vulkanPhysicalDeviceMemory
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Buffer::Buffer(quartz::rendering::Buffer&& other) :
    m_sizeBytes(other.m_sizeBytes),
    m_usageFlags(other.m_usageFlags),
    m_memoryPropertyFlags(other.m_memoryPropertyFlags),
    mp_vulkanLogicalBuffer(std::move(other.mp_vulkanLogicalBuffer)),
    mp_vulkanPhysicalDeviceMemory(std::move(other.mp_vulkanPhysicalDeviceMemory)),
    mp_mappedLocalMemory(std::move(other.mp_mappedLocalMemory))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Buffer::~Buffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}
