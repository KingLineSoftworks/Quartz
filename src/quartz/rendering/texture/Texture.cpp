#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT

//#include <tiny_gltf.h>

//#include <stb_image.h>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

quartz::rendering::StagedImageBuffer
quartz::rendering::Texture::createImageBufferFromFilepath(
    const quartz::rendering::Device& renderingDevice,
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "{}", filepath);

    int32_t textureWidth;
    int32_t textureHeight;
    int32_t textureChannelCount;
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
    uint32_t textureSizeBytes = textureWidth * textureHeight * 4;
    LOG_TRACE(
        TEXTURE,
        "Successfully loaded {}x{} texture with {} channels ( {} bytes ) from {}",
        textureWidth,
        textureHeight,
        textureChannelCount,
        textureSizeBytes,
        filepath
    );

    quartz::rendering::StagedImageBuffer stagedImageBuffer(
        renderingDevice,
        static_cast<uint32_t>(textureWidth),
        static_cast<uint32_t>(textureHeight),
        static_cast<uint32_t>(textureChannelCount),
        textureSizeBytes,
        vk::ImageUsageFlagBits::eSampled,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        p_texturePixels
    );

    LOG_TRACE(TEXTURE, "Freeing stbi texture");
    stbi_image_free(p_texturePixels);

    return stagedImageBuffer;
}

quartz::rendering::StagedImageBuffer
quartz::rendering::Texture::createImageBufferFromGLTFImage(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Image& gltfImage
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "");

    int32_t textureWidth = gltfImage.width;
    int32_t textureHeight = gltfImage.height;
    int32_t textureChannelCount = gltfImage.component;
    uint32_t textureSizeBytes;

    uint8_t* p_intermediateRGBAPixels = nullptr;
    const uint8_t* p_texturePixels;

    LOG_TRACE(
        TEXTURE, "gltf image is {}x{} with {} channels",
        textureWidth, textureHeight, textureChannelCount
    );

    if (textureChannelCount == 3) {
        /// @todo Check if we actually need to convert based on device support
        LOG_DEBUG(TEXTURE, "Converting image data from 3 channels (RGB) to 4 channels (RGBA)");
        LOG_DEBUG(TEXTURE, "  - We are assuming the current device doesn't support RGB only");

        textureSizeBytes = textureWidth * textureHeight * 4; // rgba
        p_intermediateRGBAPixels = new uint8_t[textureSizeBytes];
        const uint8_t* p_sourceRGBPixels = &gltfImage.image[0];

        // Copy every pixel from source rgb buffer to intermediate rgba buffer
        for (uint32_t i = 0; i < static_cast<uint32_t>(gltfImage.height * gltfImage.width); ++i) {
            const uint32_t baseIntermediateIndex = i * 4;
            const uint32_t baseSourceIndex = i * textureChannelCount;

            // Copy over the 3 bytes (one each for rgb) from source buffer
            for (uint32_t j = 0; j < static_cast<uint32_t>(textureChannelCount); ++j) {
                p_intermediateRGBAPixels[baseIntermediateIndex + j] =
                    p_sourceRGBPixels[baseSourceIndex + j];
            }
        }

        p_texturePixels = p_intermediateRGBAPixels;
    } else {
        LOG_DEBUG(TEXTURE, "Image data contains valid channel count of {}", textureChannelCount);
        p_texturePixels = &gltfImage.image[0];
        textureSizeBytes = gltfImage.image.size();
    }
    LOG_DEBUG(
        TEXTURE, "Got pixel data at {} with size of {} bytes",
        static_cast<const void*>(p_texturePixels), textureSizeBytes
    );

    if (!p_texturePixels) {
        LOG_CRITICAL(TEXTURE, "Failed to load texture from gltfImage with name \"\"", gltfImage.name);
        throw std::runtime_error("");
    }

    // x4 for rgba (32 bits = 4 bytes)
    LOG_TRACE(
        TEXTURE,
        "Successfully loaded {}x{} texture with {} channels ( {} bytes ) from gltf image \"{}\"",
        textureWidth,
        textureHeight,
        textureChannelCount,
        textureSizeBytes,
        gltfImage.name
    );

    quartz::rendering::StagedImageBuffer stagedImageBuffer(
        renderingDevice,
        static_cast<uint32_t>(textureWidth),
        static_cast<uint32_t>(textureHeight),
        static_cast<uint32_t>(textureChannelCount),
        textureSizeBytes,
        vk::ImageUsageFlagBits::eSampled,
//        vk::Format::eR8G8B8A8Srgb,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        p_texturePixels
    );

    if (p_intermediateRGBAPixels) {
        LOG_TRACE(TEXTURE, "Used intermediate pixel buffer. Freeing it");
        delete[] p_intermediateRGBAPixels;
    }

    return stagedImageBuffer;
}

vk::Filter
quartz::rendering::Texture::getVulkanFilterMode(const int32_t filterMode) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "{}", filterMode);

    switch (filterMode) {
        case -1:
        case 9728:
        case 9984:
        case 9985:
            LOG_TRACE(TEXTURE, "{} = Nearest", filterMode);
            return vk::Filter::eNearest;
        case 9729:
        case 9986:
        case 9987:
            LOG_TRACE(TEXTURE, "{} = Linear", filterMode);
            return vk::Filter::eLinear;
    }

    LOG_WARNING(TEXTURE, "Unknown filter mode for {}, using nearest", filterMode);
    return vk::Filter::eNearest;
}

vk::SamplerAddressMode
quartz::rendering::Texture::getVulkanSamplerAddressMode(const int32_t addressMode) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "{}", addressMode);

    switch (addressMode) {
        case -1:
        case 10497:
            LOG_TRACE(TEXTURE, "{} = Repeat", addressMode);
            return vk::SamplerAddressMode::eRepeat;
        case 33071:
            LOG_TRACE(TEXTURE, "{} = Clamp to edge", addressMode);
            return vk::SamplerAddressMode::eClampToEdge;
        case 33648:
            LOG_TRACE(TEXTURE, "{} = Mirrored repeat", addressMode);
            return vk::SamplerAddressMode::eMirroredRepeat;
    }

    LOG_WARNING(TEXTURE, "Unknown sampler address mode for {}, using repeat", addressMode);
    return vk::SamplerAddressMode::eRepeat;
}

vk::UniqueSampler
quartz::rendering::Texture::createVulkanSamplerPtr(
    const quartz::rendering::Device& renderingDevice,
    const vk::Filter magFilter,
    const vk::Filter minFilter,
    const vk::SamplerAddressMode addressModeU,
    const vk::SamplerAddressMode addressModeV,
    const vk::SamplerAddressMode addressModeW
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "");

    vk::PhysicalDeviceProperties physicalDeviceProperties =
        renderingDevice.getVulkanPhysicalDevice().getProperties();

    vk::SamplerCreateInfo samplerCreateInfo(
        {},
        magFilter,
        minFilter,
        vk::SamplerMipmapMode::eLinear,
        addressModeU,
        addressModeV,
        addressModeW,
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
    m_stagedImageBuffer(quartz::rendering::Texture::createImageBufferFromFilepath(
        renderingDevice,
        filepath
    )),
    mp_vulkanImageView(quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        *(m_stagedImageBuffer.getVulkanImagePtr()),
        m_stagedImageBuffer.getVulkanFormat(),
        {},
        vk::ImageAspectFlagBits::eColor
    )),
    mp_vulkanSampler(quartz::rendering::Texture::createVulkanSamplerPtr(
        renderingDevice,
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat,
        vk::SamplerAddressMode::eRepeat
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Texture::Texture(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Image& gltfImage,
    const tinygltf::Sampler& gltfSampler
) :
    m_stagedImageBuffer(quartz::rendering::Texture::createImageBufferFromGLTFImage(
        renderingDevice,
        gltfImage
    )),
    mp_vulkanImageView(quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
        renderingDevice.getVulkanLogicalDevicePtr(),
        *(m_stagedImageBuffer.getVulkanImagePtr()),
        m_stagedImageBuffer.getVulkanFormat(),
        {},
        vk::ImageAspectFlagBits::eColor
    )),
    mp_vulkanSampler(quartz::rendering::Texture::createVulkanSamplerPtr(
        renderingDevice,
        quartz::rendering::Texture::getVulkanFilterMode(gltfSampler.minFilter),
        quartz::rendering::Texture::getVulkanFilterMode(gltfSampler.magFilter),
        quartz::rendering::Texture::getVulkanSamplerAddressMode(gltfSampler.wrapS),
        quartz::rendering::Texture::getVulkanSamplerAddressMode(gltfSampler.wrapT),
        quartz::rendering::Texture::getVulkanSamplerAddressMode(gltfSampler.wrapT)
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Texture::Texture(quartz::rendering::Texture&& other) :
    m_stagedImageBuffer(std::move(other.m_stagedImageBuffer)),
    mp_vulkanImageView(std::move(other.mp_vulkanImageView)),
    mp_vulkanSampler(std::move(other.mp_vulkanSampler))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Texture::~Texture() {
    LOG_FUNCTION_CALL_TRACEthis("");
}