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
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

    vk::BufferCreateInfo bufferCreateInfo(
        {},
        sizeBytes,
        bufferUsageFlags,
        vk::SharingMode::eExclusive
    );

    LOG_TRACE(BUFFER, "Attempting to create vk::Buffer ({} buffer)", quartz::rendering::BufferHelper::getUsageFlagsString(bufferUsageFlags));
    vk::UniqueBuffer p_buffer = p_logicalDevice->createBufferUnique(bufferCreateInfo);

    if (!p_buffer) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::Buffer");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::Buffer instance at {}", static_cast<void*>(&(*p_buffer)));

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
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

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
        LOG_CRITICAL(BUFFER, "Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACE(BUFFER, "Attempting to allocate vk::DeviceMemory");
    vk::UniqueDeviceMemory p_bufferMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

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

    LOG_TRACE(BUFFER, "Memory *IS* allocated for a source buffer. Populating device's buffer memory with input raw data");

    LOG_TRACE(BUFFER, "  - Creating mapping for allocated device memory of size {}", sizeBytes);
    void* p_mappedDestinationDeviceMemory = p_logicalDevice->mapMemory(
        *p_bufferMemory,
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
    p_logicalDevice->unmapMemory(*p_bufferMemory);

    LOG_TRACE(BUFFER, "Successfully copied input data to device's buffer memory");
    return p_bufferMemory;
}

vk::UniqueDeviceMemory quartz::rendering::LocallyMappedBuffer::allocateVulkanPhysicalDeviceMemoryUniquePtr(
    const vk::PhysicalDevice& physicalDevice,
    const vk::UniqueDevice& p_logicalDevice,
    const uint32_t sizeBytes,
    const vk::UniqueBuffer& p_logicalBuffer,
    const vk::MemoryPropertyFlags requiredMemoryProperties
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

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
        LOG_CRITICAL(BUFFER, "Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACE(BUFFER, "Attempting to allocate vk::DeviceMemory");
    vk::UniqueDeviceMemory p_logicalBufferMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!p_logicalBufferMemory) {
        LOG_CRITICAL(BUFFER, "Failed to allocated vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully allocated vk::DeviceMemory instance at {}", static_cast<void*>(&(*p_logicalBufferMemory)));

    LOG_TRACE(BUFFER, "Binding memory to logical device");
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
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    LOG_TRACE(BUFFER, "Mapping memory from physical device memory to local memory instance {}", static_cast<const void*>(&(*p_physicalDeviceMemory)));
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
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "{} bytes", sizeBytes);

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
        LOG_CRITICAL(BUFFER, "Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACE(BUFFER, "Attempting to allocate vk::DeviceMemory");
    vk::UniqueDeviceMemory uniqueDestinationBufferMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);

    if (!uniqueDestinationBufferMemory) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::DeviceMemory");

    p_logicalDevice->bindBufferMemory(
        *p_logicalBuffer,
        *uniqueDestinationBufferMemory,
        0
    );

    LOG_TRACE(BUFFER, "Memory is *NOT* allocated for a source buffer. Populating with input source buffer instead");

    LOG_TRACE(BUFFER, "Attempting to create vk::CommandPool");

    vk::CommandPoolCreateInfo commandPoolCreateInfo(
        vk::CommandPoolCreateFlagBits::eTransient,
        graphicsQueueFamilyIndex
    );

    vk::UniqueCommandPool p_commandPool = p_logicalDevice->createCommandPoolUnique(commandPoolCreateInfo);

    if (!p_commandPool) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::CommandPool");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::CommandPool");

    LOG_TRACE(BUFFER, "Attempting to allocate vk::CommandBuffer for copying data");

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        *p_commandPool,
        vk::CommandBufferLevel::ePrimary,
        1
    );

    std::vector<vk::UniqueCommandBuffer> commandBufferPtrs = p_logicalDevice->allocateCommandBuffersUnique(commandBufferAllocateInfo);

    if (!(commandBufferPtrs[0])) {
        LOG_CRITICAL(BUFFER, "Failed to allocate vk::CommandBuffer for copying data");
        throw std::runtime_error("");
    }

    LOG_TRACE(BUFFER, "Recording commands to newly created command buffer");

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

    LOG_TRACE(BUFFER, "Submitting command buffer and waiting idly for it to complete the transfer of data into this buffer");

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

    LOG_TRACE(BUFFER, "Successfully copied data from source buffer to this buffer's memory");

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

vk::UniqueDeviceMemory quartz::rendering::ImageBuffer::allocateVulkanPhysicalDeviceImageMemory(
    const vk::PhysicalDevice& physicalDevice,
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::UniqueBuffer& p_stagingBuffer,
    const vk::UniqueImage& p_image,
    const vk::MemoryPropertyFlags memoryPropertyFlags
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

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
        LOG_CRITICAL(BUFFER, "Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACE(BUFFER, "Attempting to create vk::DeviceMemory");
    vk::UniqueDeviceMemory p_vulkanPhysicalDeviceTextureMemory = p_logicalDevice->allocateMemoryUnique(memoryAllocateInfo);
    if (!p_vulkanPhysicalDeviceTextureMemory) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::DeviceMemory");

    p_logicalDevice->bindImageMemory(
        *p_image,
        *p_vulkanPhysicalDeviceTextureMemory,
        0
    );

    LOG_TRACE(BUFFER, "Transitioning image layout and populating memory with input buffer data");

    quartz::rendering::ImageBuffer::transitionImageLayout(
        graphicsQueueFamilyIndex,
        p_logicalDevice,
        graphicsQueue,
        p_image,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );
    quartz::rendering::ImageBuffer::copyStagedBufferToImage(
        graphicsQueueFamilyIndex,
        p_logicalDevice,
        graphicsQueue,
        imageWidth,
        imageHeight,
        p_stagingBuffer,
        p_image
    );
    quartz::rendering::ImageBuffer::transitionImageLayout(
        graphicsQueueFamilyIndex,
        p_logicalDevice,
        graphicsQueue,
        p_image,
        vk::ImageLayout::eTransferDstOptimal,
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    return p_vulkanPhysicalDeviceTextureMemory;
}

void quartz::rendering::ImageBuffer::transitionImageLayout(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const vk::UniqueImage& p_image,
    const vk::ImageLayout inputLayout,
    const vk::ImageLayout outputLayout
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    LOG_TRACE(BUFFER, "Attempting to create vk::CommandPool");

    vk::CommandPoolCreateInfo commandPoolCreateInfo(
        vk::CommandPoolCreateFlagBits::eTransient,
        graphicsQueueFamilyIndex
    );

    vk::UniqueCommandPool p_commandPool = p_logicalDevice->createCommandPoolUnique(commandPoolCreateInfo);

    if (!p_commandPool) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::CommandPool");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::CommandPool");

    LOG_TRACE(BUFFER, "Attempting to allocate vk::CommandBuffer for copying data");

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        *p_commandPool,
        vk::CommandBufferLevel::ePrimary,
        1
    );

    std::vector<vk::UniqueCommandBuffer> commandBufferPtrs = p_logicalDevice->allocateCommandBuffersUnique(commandBufferAllocateInfo);

    if (!(commandBufferPtrs[0])) {
        LOG_CRITICAL(BUFFER, "Failed to allocate vk::CommandBuffer for transitioning image's layout");
        throw std::runtime_error("");
    }

    LOG_TRACE(BUFFER, "Recording commands to newly created command buffer for transitioning image's layout");

    vk::CommandBufferBeginInfo commandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBufferPtrs[0]->begin(commandBufferBeginInfo);

    vk::AccessFlags sourceAccessMask;
    vk::AccessFlags destinationAccessMask;
    vk::PipelineStageFlags sourceStage;
    vk::PipelineStageFlags destinationStage;
    if (
        inputLayout == vk::ImageLayout::eUndefined &&
        outputLayout == vk::ImageLayout::eTransferDstOptimal
    ) {
        LOG_TRACE(BUFFER, "Transferring image from undefined layout to optimal transfer destination layout");

        destinationAccessMask = vk::AccessFlagBits::eTransferWrite;

        sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
        destinationStage = vk::PipelineStageFlagBits::eTransfer;
    } else if (
        inputLayout == vk::ImageLayout::eTransferDstOptimal &&
        outputLayout == vk::ImageLayout::eShaderReadOnlyOptimal
    ) {
        LOG_TRACE(BUFFER, "Transferring image from optimal transfer destination layout to optimal shader read only format");

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

    commandBufferPtrs[0]->pipelineBarrier(
        sourceStage,
        destinationStage,
        {},
        {},
        {},
        imageMemoryBarrier
    );

    commandBufferPtrs[0]->end();

    LOG_TRACE(BUFFER, "Submitting command buffer and waiting idly for it to complete the transition of image's layout");

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

    LOG_TRACE(BUFFER, "Successfully transitioned image's layout");
}

void quartz::rendering::ImageBuffer::copyStagedBufferToImage(
    const uint32_t graphicsQueueFamilyIndex,
    const vk::UniqueDevice& p_logicalDevice,
    const vk::Queue& graphicsQueue,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const vk::UniqueBuffer& p_stagingBuffer,
    const vk::UniqueImage& p_image
) {
    LOG_FUNCTION_SCOPE_TRACE(BUFFER, "");

    LOG_TRACE(BUFFER, "Attempting to create vk::CommandPool");

    vk::CommandPoolCreateInfo commandPoolCreateInfo(
        vk::CommandPoolCreateFlagBits::eTransient,
        graphicsQueueFamilyIndex
    );

    vk::UniqueCommandPool p_commandPool = p_logicalDevice->createCommandPoolUnique(commandPoolCreateInfo);

    if (!p_commandPool) {
        LOG_CRITICAL(BUFFER, "Failed to create vk::CommandPool");
        throw std::runtime_error("");
    }
    LOG_TRACE(BUFFER, "Successfully created vk::CommandPool");

    LOG_TRACE(BUFFER, "Attempting to allocate vk::CommandBuffer for copying data");

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        *p_commandPool,
        vk::CommandBufferLevel::ePrimary,
        1
    );

    std::vector<vk::UniqueCommandBuffer> commandBufferPtrs = p_logicalDevice->allocateCommandBuffersUnique(commandBufferAllocateInfo);

    if (!(commandBufferPtrs[0])) {
        LOG_CRITICAL(BUFFER, "Failed to allocate vk::CommandBuffer for transitioning image's layout");
        throw std::runtime_error("");
    }

    LOG_TRACE(BUFFER, "Recording commands to newly created command buffer for copying data from staged buffer to image");

    vk::CommandBufferBeginInfo commandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBufferPtrs[0]->begin(commandBufferBeginInfo);

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

    commandBufferPtrs[0]->copyBufferToImage(
        *p_stagingBuffer,
        *p_image,
        vk::ImageLayout::eTransferDstOptimal,
        bufferImageCopy
    );

    commandBufferPtrs[0]->end();

    LOG_TRACE(BUFFER, "Submitting command buffer and waiting idly for it to complete the copy of data from staging buffer to image");

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

    LOG_TRACE(BUFFER, "Successfully copied data");
}

quartz::rendering::ImageBuffer::ImageBuffer(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const uint32_t sizeBytes,
    const vk::ImageUsageFlags usageFlags,
    const vk::Format format,
    const vk::ImageTiling tiling,
    const void* p_bufferData
) :
    m_imageWidth(imageWidth),
    m_imageHeight(imageHeight),
    m_sizeBytes(sizeBytes),
    m_usageFlags(usageFlags),
    m_format(format),
    m_tiling(tiling),
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
        m_imageWidth,
        m_imageHeight,
        vk::ImageUsageFlagBits::eTransferDst | m_usageFlags,
        m_format,
        m_tiling
    )),
    mp_vulkanPhysicalDeviceMemory(quartz::rendering::ImageBuffer::allocateVulkanPhysicalDeviceImageMemory(
        renderingDevice.getVulkanPhysicalDevice(),
        renderingDevice.getGraphicsQueueFamilyIndex(),
        renderingDevice.getVulkanLogicalDevicePtr(),
        renderingDevice.getVulkanGraphicsQueue(),
        m_imageWidth,
        m_imageHeight,
        mp_vulkanLogicalStagingBuffer,
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
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::ImageBuffer::~ImageBuffer() {
    LOG_FUNCTION_CALL_TRACEthis("");
}