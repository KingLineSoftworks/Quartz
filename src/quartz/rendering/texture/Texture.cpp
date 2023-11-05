#include <vulkan/vulkan.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

quartz::rendering::StagedImageBuffer
quartz::rendering::Texture::createImageBuffer(
    const quartz::rendering::Device& renderingDevice,
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "{}", filepath);

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
        LOG_CRITICAL(TEXTURE, "Failed to load texture from {}", filepath);
        throw std::runtime_error("");
    }

    // x4 for rgba (32 bits = 4 bytes)
    uint32_t imageSizeBytes = textureWidth * textureHeight * 4;
    LOG_TRACE(
        TEXTURE,
        "Successfully loaded {}x{} texture with {} channels ( {} bytes ) from {}",
        textureWidth,
        textureHeight,
        textureChannelCount,
        imageSizeBytes,
        filepath
    );

    quartz::rendering::StagedImageBuffer stagedImageBuffer(
        renderingDevice,
        static_cast<uint32_t>(textureWidth),
        static_cast<uint32_t>(textureHeight),
        static_cast<uint32_t>(textureChannelCount),
        imageSizeBytes,
        vk::ImageUsageFlagBits::eSampled,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        p_texturePixels
    );

    LOG_TRACE(TEXTURE, "Freeing stbi texture");
    stbi_image_free(p_texturePixels);

    return stagedImageBuffer;
}

vk::UniqueSampler
quartz::rendering::Texture::createVulkanSamplerPtr(
    const quartz::rendering::Device& renderingDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "");

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

    vk::UniqueSampler p_sampler =
        renderingDevice.getVulkanLogicalDevicePtr()->createSamplerUnique(
            samplerCreateInfo
        );

    if (!p_sampler) {
        LOG_CRITICAL(TEXTURE, "Failed to create vk::Sampler");
        throw std::runtime_error("");
    }

    return p_sampler;
}

quartz::rendering::Texture::Texture(
    const quartz::rendering::Device& renderingDevice,
    const std::string& filepath
) :
    m_filepath(filepath),
    m_stagedImageBuffer(quartz::rendering::Texture::createImageBuffer(
        renderingDevice,
        m_filepath
    )),
    mp_vulkanImageView(quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        *(m_stagedImageBuffer.getVulkanImagePtr()),
        m_stagedImageBuffer.getVulkanFormat(),
        {},
        vk::ImageAspectFlagBits::eColor
    )),
    mp_vulkanSampler(quartz::rendering::Texture::createVulkanSamplerPtr(
        renderingDevice
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Texture::Texture(quartz::rendering::Texture&& other) :
    m_filepath(other.m_filepath),
    m_stagedImageBuffer(std::move(other.m_stagedImageBuffer)),
    mp_vulkanImageView(std::move(other.mp_vulkanImageView)),
    mp_vulkanSampler(std::move(other.mp_vulkanSampler))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Texture::~Texture() {
    LOG_FUNCTION_CALL_TRACEthis("");
}