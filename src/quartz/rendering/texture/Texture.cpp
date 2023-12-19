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

uint32_t quartz::rendering::Texture::baseColorDefaultIndex = 0;
uint32_t quartz::rendering::Texture::normalDefaultIndex = 0;
uint32_t quartz::rendering::Texture::emissionDefaultIndex = 0;
uint32_t quartz::rendering::Texture::metallicRoughnessDefaultIndex = 0;
std::vector<std::shared_ptr<quartz::rendering::Texture>> quartz::rendering::Texture::masterList;

uint32_t
quartz::rendering::Texture::createTexture(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Image& gltfImage,
    const tinygltf::Sampler& gltfSampler
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "");

    if (quartz::rendering::Texture::masterList.empty()) {
        LOG_TRACE(TEXTURE, "Master list is empty, initializing");
        quartz::rendering::Texture::initializeMasterList(renderingDevice);
    }

    std::shared_ptr<quartz::rendering::Texture> p_texture =
        std::make_shared<quartz::rendering::Texture>(
            renderingDevice,
            gltfImage,
            gltfSampler
        );

    quartz::rendering::Texture::masterList.push_back(p_texture);

    uint32_t insertedIndex = quartz::rendering::Texture::masterList.size() - 1;
    LOG_TRACE(TEXTURE, "Texture was inserted into master list at index {}", insertedIndex);

    return insertedIndex;
}

void
quartz::rendering::Texture::initializeMasterList(
    const quartz::rendering::Device& renderingDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "");

    if (!quartz::rendering::Texture::masterList.empty()) {
        LOG_TRACE(TEXTURE, "Master list is already initialized. Not doing anything");
        return;
    }

    quartz::rendering::Texture::masterList.reserve(QUARTZ_MAX_NUMBER_BASE_COLOR_TEXTURES);

    LOG_TRACE(TEXTURE, "Creating base color default texture");
    const std::vector<uint8_t> baseColorPixel = { 0xFF, 0x00, 0xFF, 0xFF }; // Default to magenta (r=1.0, g=0.0, b=1.0, a=1.0)
    std::shared_ptr<quartz::rendering::Texture> p_baseColorDefault =
        std::make_shared<quartz::rendering::Texture>(
            renderingDevice,
            1,
            1,
            4,
            reinterpret_cast<const void*>(baseColorPixel.data())
        );
    quartz::rendering::Texture::masterList.push_back(p_baseColorDefault);
    quartz::rendering::Texture::baseColorDefaultIndex = quartz::rendering::Texture::masterList.size() - 1;
    LOG_INFO(TEXTURE, "Base color default at index {}", quartz::rendering::Texture::baseColorDefaultIndex);

    LOG_TRACE(TEXTURE, "Creating normal default texture");
    const std::vector<uint8_t> normalPixel = { 0x80, 0x80, 0xFF, 0xFF }; // Default to neutral (r=0.5, g=0.5, b=1.0, a=1.0)
    std::shared_ptr<quartz::rendering::Texture> p_normalDefault =
        std::make_shared<quartz::rendering::Texture>(
            renderingDevice,
            1,
            1,
            4,
            reinterpret_cast<const void*>(normalPixel.data())
        );
    quartz::rendering::Texture::masterList.push_back(p_normalDefault);
    quartz::rendering::Texture::normalDefaultIndex = quartz::rendering::Texture::masterList.size() - 1;
    LOG_INFO(TEXTURE, "Normal default at index {}", quartz::rendering::Texture::normalDefaultIndex);

    LOG_TRACE(TEXTURE, "Creating emissive default texture");
    const std::vector<uint8_t> emissivePixel = { 0x00, 0x00, 0x00, 0x00 };
    std::shared_ptr<quartz::rendering::Texture> p_emissiveDefault =
        std::make_shared<quartz::rendering::Texture>(
            renderingDevice,
            1,
            1,
            4,
            reinterpret_cast<const void*>(emissivePixel.data())
        );
    quartz::rendering::Texture::masterList.push_back(p_emissiveDefault);
    quartz::rendering::Texture::emissionDefaultIndex = quartz::rendering::Texture::masterList.size() - 1;
    LOG_INFO(TEXTURE, "Emissive default at index {}", quartz::rendering::Texture::emissionDefaultIndex);

    /**
     * @todo 2023/11/19 Create default textures for each of
     *   metallicRoughness
     */
}

void
quartz::rendering::Texture::cleanUpAllTextures() {
    LOG_FUNCTION_SCOPE_TRACE(TEXTURE, "");

    quartz::rendering::Texture::masterList.clear();
}

std::string
quartz::rendering::Texture::getTextureTypeGLTFString(
    const quartz::rendering::Texture::Type type
) {
    switch(type) {
        case quartz::rendering::Texture::Type::BaseColor:
            return "baseColorTexture";
        case quartz::rendering::Texture::Type::Normal:
            return "normalTexture";
        case quartz::rendering::Texture::Type::Emissive:
            return "emissiveTexture";
        case quartz::rendering::Texture::Type::MetallicRoughness:
            return "metallicRoughnessTexture";
    }
}

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
        /// @todo 2023/11/01 Check if we actually need to convert based on device support
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
    switch (filterMode) {
        case -1:
        case 9728:
        case 9984:
        case 9985:
            LOG_TRACE(TEXTURE, "filter mode {} = Nearest", filterMode);
            return vk::Filter::eNearest;
        case 9729:
        case 9986:
        case 9987:
            LOG_TRACE(TEXTURE, "filter mode {} = Linear", filterMode);
            return vk::Filter::eLinear;
        default:
            LOG_WARNING(TEXTURE, "filter mode {} = unknown, using Nearest", filterMode);
            return vk::Filter::eNearest;
    }
}

vk::SamplerAddressMode
quartz::rendering::Texture::getVulkanSamplerAddressMode(const int32_t addressMode) {
    switch (addressMode) {
        case -1:
        case 10497:
            LOG_TRACE(TEXTURE, "address address mode {} = Repeat", addressMode);
            return vk::SamplerAddressMode::eRepeat;
        case 33071:
            LOG_TRACE(TEXTURE, "address address mode {} = Clamp to edge", addressMode);
            return vk::SamplerAddressMode::eClampToEdge;
        case 33648:
            LOG_TRACE(TEXTURE, "address address mode {} = Mirrored repeat", addressMode);
            return vk::SamplerAddressMode::eMirroredRepeat;
        default:
            LOG_WARNING(TEXTURE, "sampler address mode {} = unknown, using Repeat", addressMode);
            return vk::SamplerAddressMode::eRepeat;
    }
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
    const uint32_t imageWidth,
    const uint32_t imageHeight,
    const uint32_t channelCount,
    const void* p_pixels
) :
    m_stagedImageBuffer(
        renderingDevice,
        imageWidth,
        imageHeight,
        channelCount,
        imageWidth * imageHeight * channelCount,
        vk::ImageUsageFlagBits::eSampled,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        p_pixels
    ),
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