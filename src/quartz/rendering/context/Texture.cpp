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

    uint32_t imageSizeBytes = textureWidth * textureHeight * 4; // x4 for rgba (32 bits = 4 bytes)
    LOG_TRACEthis("Successfully loaded {}x{} texture with {} channels ( {} bytes ) from {}", textureWidth, textureHeight, textureChannelCount, imageSizeBytes, filepath);

    quartz::rendering::ImageBuffer imageBuffer(
        renderingDevice,
        static_cast<uint32_t>(textureWidth),
        static_cast<uint32_t>(textureHeight),
        imageSizeBytes,
        vk::ImageUsageFlagBits::eSampled,
        p_texturePixels
    );

    LOG_TRACEthis("Freeing stbi texture");
    stbi_image_free(p_texturePixels);
}

quartz::rendering::Texture::~Texture() {
    LOG_FUNCTION_CALL_TRACEthis("");
}