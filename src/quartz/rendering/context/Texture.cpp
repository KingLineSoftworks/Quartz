#include <vulkan/vulkan.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Texture.hpp"

uint32_t quartz::rendering::Texture::getWidth(
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "{}", filepath);

    int width;
    if (!stbi_info(filepath.c_str(), &width, nullptr, nullptr)) {
        LOG_CRITICAL(TEXTURE, "Failed to retrieve image information for {}", filepath);
        throw std::runtime_error("");
    }

    LOG_TRACE(TEXTURE, "Got texture width of {}", width);

    return static_cast<uint32_t>(width);
}

uint32_t quartz::rendering::Texture::getHeight(
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "{}", filepath);

    int height;
    if (!stbi_info(filepath.c_str(), nullptr, &height, nullptr)) {
        LOG_CRITICAL(TEXTURE, "Failed to retrieve image information for {}", filepath);
        throw std::runtime_error("");
    }

    LOG_TRACE(TEXTURE, "Got texture height of {}", height);

    return static_cast<uint32_t>(height);
}

uint32_t quartz::rendering::Texture::getChannelCount(
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "{}", filepath);

    int channelCount;
    if (!stbi_info(filepath.c_str(), nullptr, nullptr, &channelCount)) {
        LOG_CRITICAL(TEXTURE, "Failed to retrieve image information for {}", filepath);
        throw std::runtime_error("");
    }

    LOG_TRACE(TEXTURE, "Got texture channel count of {}", channelCount);

    return static_cast<uint32_t>(channelCount);
}

quartz::rendering::ImageBuffer quartz::rendering::Texture::createImageBuffer(
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
    LOG_TRACE(TEXTURE, "Successfully loaded {}x{} texture with {} channels "
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

    LOG_TRACE(TEXTURE, "Freeing stbi texture");
    stbi_image_free(p_texturePixels);

    return imageBuffer;
}

vk::UniqueImageView quartz::rendering::Texture::createVulkanImageViewPtr(
    const quartz::rendering::Device& renderingDevice,
    const quartz::rendering::ImageBuffer& imageBuffer
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "");

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

    LOG_TRACE(TEXTURE, "Attempting to crate vk::ImageView");
    vk::UniqueImageView p_imageView =
        renderingDevice.getVulkanLogicalDevicePtr()->createImageViewUnique(
            imageViewCreateInfo
        );

    if (!p_imageView) {
        LOG_CRITICAL(TEXTURE, "Failed to create vk::ImageView");
        throw std::runtime_error("");
    }
    LOG_TRACE(TEXTURE, "Successfully created vk::ImageView");

    return p_imageView;
}

vk::UniqueSampler quartz::rendering::Texture::createVulkanSamplerPtr(
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

    LOG_TRACE(TEXTURE, "Attempting to create vk::Sampler");
    vk::UniqueSampler p_sampler =
        renderingDevice.getVulkanLogicalDevicePtr()->createSamplerUnique(
            samplerCreateInfo
        );

    if (!p_sampler) {
        LOG_CRITICAL(TEXTURE, "Failed to create vk::Sampler");
        throw std::runtime_error("");
    }
    LOG_TRACE(TEXTURE, "Successfully created vk::Sampler");

    return p_sampler;
}

quartz::rendering::Texture::Texture(
    const quartz::rendering::Device& renderingDevice,
    const std::string& filepath
) :
    m_filepath(filepath),
    m_width(quartz::rendering::Texture::getWidth(
        m_filepath
    )),
    m_height(quartz::rendering::Texture::getHeight(
        m_filepath
    )),
    m_channelCount(quartz::rendering::Texture::getChannelCount(
        m_filepath
    )),
    m_imageBuffer(quartz::rendering::Texture::createImageBuffer(
        renderingDevice,
        m_filepath
    )),
    mp_vulkanImageView(quartz::rendering::Texture::createVulkanImageViewPtr(
        renderingDevice,
        m_imageBuffer
    )),
    mp_vulkanSampler(quartz::rendering::Texture::createVulkanSamplerPtr(
        renderingDevice
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Texture::Texture(quartz::rendering::Texture&& other) :
    m_filepath(other.m_filepath),
    m_width(other.m_width),
    m_height(other.m_height),
    m_channelCount(other.m_channelCount),
    m_imageBuffer(std::move(other.m_imageBuffer)),
    mp_vulkanImageView(std::move(other.mp_vulkanImageView)),
    mp_vulkanSampler(std::move(other.mp_vulkanSampler))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Texture::~Texture() {
    LOG_FUNCTION_CALL_TRACEthis("");
}