#include <vulkan/vulkan.hpp>

#include <stb_image.h>

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Texture.hpp"

quartz::rendering::Texture::Texture(
    const quartz::rendering::Device& renderingDevice
)
{
    LOG_FUNCTION_CALL_TRACEthis("");

    // ----- load the texture ----- //

    const std::string filepath = util::FileSystem::getAbsoluteFilepathInProject("texture.jpg");

    int textureWidth;
    int textureHeight;
    int textureChannelCount;
    // stbi_set_flip_vertically_on_load(true);
    uint8_t* p_texturePixels = stbi_load(
        filepath.c_str(),
        &textureWidth,
        &textureHeight,
        &textureChannelCount,
        STBI_rgb_alpha
    );
    if (!p_texturePixels) {
        LOG_CRITICALthis("Failed to load texture from {}", filepath);
        throw std::runtime_error("");
    }
    LOG_TRACEthis("Successfully loaded {}x{} texture with {} channels from {}", textureWidth, textureHeight, textureChannelCount, filepath);

    // ----- copy the texture into a buffer ----- //

    uint32_t imageSizeBytes = textureWidth * textureHeight * 4; // x4 for rgba (32 bits = 4 bytes)
    quartz::rendering::StagedBuffer stagedBuffer(
        renderingDevice,
        imageSizeBytes,
        {},
        p_texturePixels
    );

    LOG_TRACEthis("Freeing stbi texture");
    stbi_image_free(p_texturePixels);

    // ----- create a vulkan image for the texture ----- //

    vk::ImageCreateInfo imageCreateInfo(
        {},
        vk::ImageType::e2D,
        vk::Format::eR8G8B8A8Srgb,
        {
            static_cast<uint32_t>(textureWidth),
            static_cast<uint32_t>(textureHeight),
            1
        },
        1,
        1,
        vk::SampleCountFlagBits::e1,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::SharingMode::eExclusive
    );

    LOG_TRACEthis("Attempting to create vk::Image");
    vk::UniqueImage p_vulkanTextureImage = renderingDevice.getVulkanLogicalDevicePtr()->createImageUnique(imageCreateInfo);
    if (!p_vulkanTextureImage) {
        LOG_CRITICALthis("Failed to create vk::Image");
        throw std::runtime_error("");
    }
    LOG_TRACEthis("Successfully created vk::Image");

    // ----- allocate memory for the image on the device ----- //

    vk::MemoryRequirements memoryRequirements = renderingDevice.getVulkanLogicalDevicePtr()->getImageMemoryRequirements(*p_vulkanTextureImage);

    vk::PhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = renderingDevice.getVulkanPhysicalDevice().getMemoryProperties();
    std::optional<uint32_t> chosenMemoryTypeIndex;
    for (uint32_t i = 0; i < physicalDeviceMemoryProperties.memoryTypeCount; ++i) {
        if (
            (memoryRequirements.memoryTypeBits & (1 << i)) && // if it has our desired memory type bit set
            (physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
        ) {
            chosenMemoryTypeIndex = i;
            break;
        }
    }
    if (!chosenMemoryTypeIndex.has_value()) {
        LOG_CRITICALthis("Failed to find a suitable memory type");
        throw std::runtime_error("");
    }

    vk::MemoryAllocateInfo memoryAllocateInfo(
        memoryRequirements.size,
        chosenMemoryTypeIndex.value()
    );

    LOG_TRACEthis("Attempting to create vk::DeviceMemory");
    vk::UniqueDeviceMemory p_vulkanPhysicalDeviceTextureMemory = renderingDevice.getVulkanLogicalDevicePtr()->allocateMemoryUnique(memoryAllocateInfo);
    if (!p_vulkanTextureImage) {
        LOG_CRITICALthis("Failed to create vk::DeviceMemory");
        throw std::runtime_error("");
    }
    LOG_TRACEthis("Successfully created vk::DeviceMemory");

    LOG_TRACEthis("Binding memory to logical device");
    renderingDevice.getVulkanLogicalDevicePtr()->bindImageMemory(
        *p_vulkanTextureImage,
        *p_vulkanPhysicalDeviceTextureMemory,
        0
    );
}

quartz::rendering::Texture::~Texture() {
    LOG_FUNCTION_CALL_TRACEthis("");
}