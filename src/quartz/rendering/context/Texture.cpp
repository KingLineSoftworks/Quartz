#include <vulkan/vulkan.hpp>

#include <stb_image.h>

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Texture.hpp"

quartz::rendering::Texture::Texture(
    const quartz::rendering::Device& renderingDevice
) {
    LOG_FUNCTION_CALL_TRACEthis("");

    const std::string filepath = util::FileSystem::getAbsoluteFilepathInProject(
        "texture.jpg"
    );

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

    // x4 for rgba (32 bits = 4 bytes)
    uint32_t imageSizeBytes = textureWidth * textureHeight * 4;
    LOG_TRACEthis("Successfully loaded {}x{} texture with {} channels "
                  "( {} bytes ) from {}",
                  textureWidth,
                  textureHeight,
                  textureChannelCount,
                  imageSizeBytes,
                  filepath);

    quartz::rendering::ImageBuffer imageBuffer(
        renderingDevice,
        static_cast<uint32_t>(textureWidth),
        static_cast<uint32_t>(textureHeight),
        imageSizeBytes,
        vk::ImageUsageFlagBits::eSampled,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        p_texturePixels
    );

    LOG_TRACEthis("Freeing stbi texture");
    stbi_image_free(p_texturePixels);

    vk::ImageViewCreateInfo imageViewCreateInfo(
        {},
        *imageBuffer.getVulkanImagePtr(),
        vk::ImageViewType::e2D,
        vk::Format::eR8G8B8A8Srgb,
        {},
        {
            vk::ImageAspectFlagBits::eColor,
            0,
            1,
            0,
            1
        }
    );

    LOG_TRACEthis("Attempting to crate vk::ImageView");
    vk::UniqueImageView p_imageView =
        renderingDevice.getVulkanLogicalDevicePtr()->createImageViewUnique(
            imageViewCreateInfo
        );

    if (!p_imageView) {
        LOG_CRITICALthis("Failed to create vk::ImageView");
        throw std::runtime_error("");
    }
    LOG_TRACEthis("Successfully created vk::ImageView");

    vk::PhysicalDeviceProperties physicalDeviceProperties =
        renderingDevice.getVulkanPhysicalDevice().getProperties();

    vk::SamplerCreateInfo samplerCreateInfo(
        {},
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        0.0f,
        true,
        physicalDeviceProperties.limits.maxSamplerAnisotropy,
        false,
        vk::CompareOp::eAlways,
        0.0f,
        0.0f,
        vk::BorderColor::eIntOpaqueBlack,
        false
    );

    LOG_TRACEthis("Attempting to create vk::Sampler");
    vk::UniqueSampler p_sampler =
        renderingDevice.getVulkanLogicalDevicePtr()->createSamplerUnique(
            samplerCreateInfo
        );

    if (!p_sampler) {
        LOG_CRITICALthis("Failed to create vk::Sampler");
        throw std::runtime_error("");
    }
    LOG_TRACEthis("Successfully created vk::Sampler");
}

quartz::rendering::Texture::~Texture() {
    LOG_FUNCTION_CALL_TRACEthis("");
}