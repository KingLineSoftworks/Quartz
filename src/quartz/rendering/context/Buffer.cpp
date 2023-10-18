#include <vulkan/vulkan.hpp>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"

std::string quartz::rendering::BufferHelper::getUsageFlagsString(const vk::BufferUsageFlags bufferUsageFlags) {
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

vk::UniqueBuffer quartz::rendering::BufferHelper::createVulkanBufferUniquePtr(
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

    LOG_TRACE(quartz::loggers::BUFFER, "Attempting to create vk::Buffer ({} buffer)", quartz::rendering::BufferHelper::getUsageFlagsString(bufferUsageFlags));
    vk::UniqueBuffer p_buffer = p_logicalDevice->createBufferUnique(bufferCreateInfo);

    if (!p_buffer) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to create vk::Buffer");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully created vk::Buffer instance at {}", static_cast<void*>(&(*p_buffer)));

    return p_buffer;
}

vk::UniqueDeviceMemory quartz::rendering::BufferHelper::allocateVulkanPhysicalDeviceStagingMemoryUniquePtr(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const void* p_bufferData,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags memoryPropertyFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "{} bytes", sizeBytes);

    vk::MemoryRequirements memoryRequirements = p_logicalDevice->getBufferMemoryRequirements(*p_logicalBuffer);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();
    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) && // if it has our desired memory type bit set
            (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags)
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
    vk::UniqueDeviceMemory p_bufferMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!p_bufferMemory) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully created vk::DeviceMemory");

    p_logicalDevice->bindBufferMemory(
        *p_logicalBuffer,
        *p_bufferMemory,
        0
    );

    LOG_TRACE(quartz::loggers::BUFFER, "Memory *IS* allocated for a source buffer. Populating device's buffer memory with input raw data");

    LOG_TRACE(quartz::loggers::BUFFER, "  - Creating mapping for allocated device memory of size {}", sizeBytes);
    void* p_mappedDestinationDeviceMemory = p_logicalDevice->mapMemory(
        *p_bufferMemory,
        0,
        sizeBytes
    );

    LOG_TRACE(quartz::loggers::BUFFER, "  - Copying {} bytes to mapped device memory at {} from buffer at {}", sizeBytes, p_mappedDestinationDeviceMemory, p_bufferData);
    memcpy(
        p_mappedDestinationDeviceMemory,
        p_bufferData,
        sizeBytes
    );

    LOG_TRACE(quartz::loggers::BUFFER, "  - Unmapping device memory");
    p_logicalDevice->unmapMemory(*p_bufferMemory);

    LOG_TRACE(quartz::loggers::BUFFER, "Successfully copied input data to device's buffer memory");
    return p_bufferMemory;
}

vk::UniqueDeviceMemory quartz::rendering::LocallyMappedBuffer::allocateVulkanPhysicalDeviceMemoryUniquePtr(
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

void* quartz::rendering::LocallyMappedBuffer::mapVulkanPhysicalDeviceMemoryToLocalMemory(
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::UniqueDeviceMemory& p_physicalDeviceMemory
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "");

    LOG_TRACE(quartz::loggers::BUFFER, "Mapping memory from physical device memory to local memory instance {}", static_cast<const void*>(&(*p_physicalDeviceMemory)));
    void* p_mappedLocalMemory = p_logicalDevice->mapMemory(
        *(p_physicalDeviceMemory),
        0,
        sizeBytes
    );

    LOG_TRACE(quartz::loggers::BUFFER, "Mapped to {}", p_mappedLocalMemory);
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
    mp_vulkanLogicalBuffer(quartz::rendering::BufferHelper::createVulkanBufferUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        m_usageFlags
    )),
    mp_vulkanPhysicalDeviceMemory(quartz::rendering::LocallyMappedBuffer::allocateVulkanPhysicalDeviceMemoryUniquePtr(
        renderingDevice.getVulkanPhysicalDevice(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        mp_vulkanLogicalBuffer,
        memoryPropertyFlags
    )),
    mp_mappedLocalMemory(quartz::rendering::LocallyMappedBuffer::mapVulkanPhysicalDeviceMemoryToLocalMemory(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        mp_vulkanPhysicalDeviceMemory
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::LocallyMappedBuffer::LocallyMappedBuffer(quartz::rendering::LocallyMappedBuffer&& other) :
    m_sizeBytes(other.m_sizeBytes),
    m_usageFlags(other.m_usageFlags),
    m_memoryPropertyFlags(other.m_memoryPropertyFlags),
    mp_vulkanLogicalBuffer(std::move(other.mp_vulkanLogicalBuffer)),
    mp_vulkanPhysicalDeviceMemory(std::move(other.mp_vulkanPhysicalDeviceMemory)),
    mp_mappedLocalMemory(std::move(other.mp_mappedLocalMemory))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::LocallyMappedBuffer::~LocallyMappedBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

vk::UniqueDeviceMemory quartz::rendering::StagedBuffer::allocateVulkanPhysicalDeviceDestinationMemoryUniquePtr(
    const vk::PhysicalDevice& physicalDevice,
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const uint32_t sizeBytes,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags memoryPropertyFlags,
    const vk::UniqueBuffer& p_logicalStagingBuffer
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "{} bytes", sizeBytes);

    vk::MemoryRequirements memoryRequirements = p_logicalDevice->getBufferMemoryRequirements(*p_logicalBuffer);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();
    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) && // if it has our desired memory type bit set
            (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags)
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
    vk::UniqueDeviceMemory uniqueDestinationBufferMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!uniqueDestinationBufferMemory) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully created vk::DeviceMemory");

    p_logicalDevice->bindBufferMemory(
        *p_logicalBuffer,
        *uniqueDestinationBufferMemory,
        0
    );

    LOG_TRACE(quartz::loggers::BUFFER, "Memory is *NOT* allocated for a source buffer. Populating with input source buffer instead");

    LOG_TRACE(quartz::loggers::BUFFER, "Attempting to create vk::CommandPool");

    vk::CommandPoolCreateInfo commandPoolCreateInfo(
        vk::CommandPoolCreateFlagBits::eTransient,
        graphicsQueueFamilyIndex
    );

    vk::UniqueCommandPool p_commandPool = p_logicalDevice->createCommandPoolUnique(commandPoolCreateInfo);

    if (!p_commandPool) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to create vk::CommandPool");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully created vk::CommandPool");

    LOG_TRACE(quartz::loggers::BUFFER, "Attempting to allocate vk::CommandBuffer for copying data");

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        *p_commandPool,
        vk::CommandBufferLevel::ePrimary,
        1
    );

    std::vector<vk::UniqueCommandBuffer> commandBufferPtrs = p_logicalDevice->allocateCommandBuffersUnique(commandBufferAllocateInfo);

    if (!(commandBufferPtrs[0])) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to allocate vk::CommandBuffer for copying data");
        throw std::runtime_error("");
    }

    LOG_TRACE(quartz::loggers::BUFFER, "Recording commands to newly created command buffer");

    vk::CommandBufferBeginInfo commandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBufferPtrs[0]->begin(commandBufferBeginInfo);

    vk::BufferCopy bufferCopyRegion(
        0,
        0,
        sizeBytes
    );

    commandBufferPtrs[0]->copyBuffer(
        *p_logicalStagingBuffer,
        *p_logicalBuffer,
        bufferCopyRegion
    );

    commandBufferPtrs[0]->end();

    LOG_TRACE(quartz::loggers::BUFFER, "Submitting command buffer and waiting idly for it to complete the transfer of data into this buffer");

    vk::SubmitInfo submitInfo(
        0,
        nullptr,
        nullptr,
        1,
        &(*(commandBufferPtrs[0])),
        0,
        nullptr
    );
    graphicsQueue.submit(submitInfo, VK_NULL_HANDLE);
    graphicsQueue.waitIdle();

    LOG_TRACE(quartz::loggers::BUFFER, "Successfully copied data from source buffer to this buffer's memory");
    return uniqueDestinationBufferMemory;
}

quartz::rendering::StagedBuffer::StagedBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t sizeBytes,
    const vk::BufferUsageFlags usageFlags,
    const void* p_bufferData
) :
    m_sizeBytes(sizeBytes),
    m_usageFlags(usageFlags),
    mp_vulkanLogicalStagingBuffer(quartz::rendering::BufferHelper::createVulkanBufferUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        vk::BufferUsageFlagBits::eTransferSrc
    )),
    mp_vulkanPhysicalDeviceStagingMemory(quartz::rendering::BufferHelper::allocateVulkanPhysicalDeviceStagingMemoryUniquePtr(
        renderingDevice.getVulkanPhysicalDevice(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        p_bufferData,
        mp_vulkanLogicalStagingBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    )),
    mp_vulkanLogicalBuffer(quartz::rendering::BufferHelper::createVulkanBufferUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        vk::BufferUsageFlagBits::eTransferDst | m_usageFlags
    )),
    mp_vulkanPhysicalDeviceMemory(quartz::rendering::StagedBuffer::allocateVulkanPhysicalDeviceDestinationMemoryUniquePtr(
        renderingDevice.getVulkanPhysicalDevice(),
        renderingDevice.getGraphicsQueueFamilyIndex(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingDevice.getVulkanGraphicsQueue(),
        m_sizeBytes,
        mp_vulkanLogicalBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mp_vulkanLogicalStagingBuffer
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::StagedBuffer::StagedBuffer(quartz::rendering::StagedBuffer&& other) :
    m_sizeBytes(other.m_sizeBytes),
    m_usageFlags(other.m_usageFlags),
    mp_vulkanLogicalStagingBuffer(std::move(other.mp_vulkanLogicalStagingBuffer)),
    mp_vulkanPhysicalDeviceStagingMemory(std::move(other.mp_vulkanPhysicalDeviceStagingMemory)),
    mp_vulkanLogicalBuffer(std::move(other.mp_vulkanLogicalBuffer)),
    mp_vulkanPhysicalDeviceMemory(std::move(other.mp_vulkanPhysicalDeviceMemory))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::StagedBuffer::~StagedBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

vk::UniqueImage quartz::rendering::ImageBuffer::createVulkanImagePtr(
    const vk::UniqueDevice& p_logicalDevice,
    const vk::ImageUsageFlags usageFlags,
    const uint32_t imageWidth,
    const uint32_t imageHeight
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "");

    vk::ImageCreateInfo imageCreateInfo(
        {},
        vk::ImageType::e2D,
        vk::Format::eR8G8B8A8Srgb,
        {
            static_cast<uint32_t>(imageWidth),
            static_cast<uint32_t>(imageHeight),
            1
        },
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        usageFlags,
        vk::SharingMode::eExclusive
    );

    LOG_TRACE(quartz::loggers::BUFFER, "Attempting to create vk::Image");
    vk::UniqueImage p_vulkanImage = p_logicalDevice->createImageUnique(imageCreateInfo);
    if (!p_vulkanImage) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to create vk::Image");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully created vk::Image");

    return p_vulkanImage;
}

vk::UniqueDeviceMemory quartz::rendering::ImageBuffer::allocateVulkanPhysicalDeviceImageMemory(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::UniqueImage& p_image,
    const vk::MemoryPropertyFlags memoryPropertyFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::BUFFER, "");

    vk::MemoryRequirements memoryRequirements = p_logicalDevice->getImageMemoryRequirements(*p_image);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = physicalDevice.getMemoryProperties();
    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) && // if it has our desired memory type bit set
            (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags)
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

    LOG_TRACE(quartz::loggers::BUFFER, "Attempting to create vk::DeviceMemory");
    vk::UniqueDeviceMemory p_vulkanPhysicalDeviceTextureMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);
    if (!p_vulkanPhysicalDeviceTextureMemory) {
        LOG_CRITICAL(quartz::loggers::BUFFER, "Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::BUFFER, "Successfully created vk::DeviceMemory");

    LOG_TRACE(quartz::loggers::BUFFER, "Binding memory to logical device");
    p_logicalDevice->bindImageMemory(
        *p_image,
        *p_vulkanPhysicalDeviceTextureMemory,
        0
    );

    return p_vulkanPhysicalDeviceTextureMemory;
}

quartz::rendering::ImageBuffer::ImageBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const uint32_t sizeBytes,
    const vk::ImageUsageFlags usageFlags,
    const void* p_bufferData
) :
    m_imageWidth(imageWidth),
    m_imageHeight(imageHeight),
    m_sizeBytes(sizeBytes),
    m_usageFlags(usageFlags),
    mp_vulkanLogicalStagingBuffer(quartz::rendering::BufferHelper::createVulkanBufferUniquePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        vk::BufferUsageFlagBits::eTransferSrc
    )),
    mp_vulkanPhysicalDeviceStagingMemory(quartz::rendering::BufferHelper::allocateVulkanPhysicalDeviceStagingMemoryUniquePtr(
        renderingDevice.getVulkanPhysicalDevice(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        m_sizeBytes,
        p_bufferData,
        mp_vulkanLogicalStagingBuffer,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    )),
    mp_vulkanImage(quartz::rendering::ImageBuffer::createVulkanImagePtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        vk::ImageUsageFlagBits::eTransferDst | m_usageFlags, // vk::ImageUsageFlagBits::eSampled
        m_imageWidth,
        m_imageHeight
    )),
    mp_vulkanPhysicalDeviceMemory(quartz::rendering::ImageBuffer::allocateVulkanPhysicalDeviceImageMemory(
        renderingDevice.getVulkanPhysicalDevice(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        mp_vulkanImage,
        vk::MemoryPropertyFlagBits::eDeviceLocal
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::ImageBuffer::ImageBuffer(quartz::rendering::ImageBuffer&& other) :
    m_sizeBytes(other.m_sizeBytes),
    m_usageFlags(other.m_usageFlags),
    mp_vulkanLogicalStagingBuffer(std::move(other.mp_vulkanLogicalStagingBuffer)),
    mp_vulkanPhysicalDeviceStagingMemory(std::move(other.mp_vulkanPhysicalDeviceStagingMemory)),
    mp_vulkanImage(std::move(other.mp_vulkanImage)),
    mp_vulkanPhysicalDeviceMemory(std::move(other.mp_vulkanPhysicalDeviceMemory))
{

}

quartz::rendering::ImageBuffer::~ImageBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}