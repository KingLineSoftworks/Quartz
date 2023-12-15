#pragma once

#include <string>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class Texture;
}
}

class quartz::rendering::Texture {
public: // enums
    enum class Type {
        BaseColor = 0,
        Normal = 1,
        Emission = 2,
        MetallicRoughness = 3
    };

public: // static functions
    static uint32_t createTexture(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Image& gltfImage,
        const tinygltf::Sampler& gltfSampler
    );
    static void initializeMasterList(
        const quartz::rendering::Device& renderingDevice
    );
    static void cleanUpAllTextures();

    static std::string getTextureTypeGLTFString(const quartz::rendering::Texture::Type type);

    static uint32_t getBaseColorDefaultIndex() { return quartz::rendering::Texture::baseColorDefaultIndex; }
    static uint32_t getNormalDefaultIndex() { return quartz::rendering::Texture::normalDefaultIndex; }
    static uint32_t getEmissionDefaultIndex() { return quartz::rendering::Texture::emissionDefaultIndex; }
    static uint32_t getMetallicRoughnessDefaultIndex() { return quartz::rendering::Texture::metallicRoughnessDefaultIndex; }

    static std::weak_ptr<Texture> getTexture(const uint32_t index) { return quartz::rendering::Texture::masterList[index]; }
    static const std::vector<std::shared_ptr<quartz::rendering::Texture>>& getMasterList() { return quartz::rendering::Texture::masterList; }

public: // member functions
    Texture(
        const quartz::rendering::Device& renderingDevice,
        const uint32_t imageWidth,
        const uint32_t imageHeight,
        const uint32_t channelCount,
        const void* p_pixels
    );
    Texture(
        const quartz::rendering::Device& renderingDevice,
        const std::string& filepath
    );
    Texture(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Image& gltfImage,
        const tinygltf::Sampler& gltfSampler
    );
    Texture(Texture&& other);
    ~Texture();

    USE_LOGGER(TEXTURE);

    const vk::UniqueImageView& getVulkanImageViewPtr() const { return mp_vulkanImageView; }
    const vk::UniqueSampler& getVulkanSamplerPtr() const { return mp_vulkanSampler; }

private: // static functions
    static quartz::rendering::StagedImageBuffer createImageBufferFromFilepath(
        const quartz::rendering::Device& renderingDevice,
        const std::string& filepath
    );
    static quartz::rendering::StagedImageBuffer createImageBufferFromGLTFImage(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Image& gltfImage
    );
    static vk::Filter getVulkanFilterMode(const int32_t filterMode);
    static vk::SamplerAddressMode getVulkanSamplerAddressMode(const int32_t addressMode);
    static vk::UniqueSampler createVulkanSamplerPtr(
        const quartz::rendering::Device& renderingDevice,
        const vk::Filter magFilter,
        const vk::Filter minFilter,
        const vk::SamplerAddressMode addressModeU,
        const vk::SamplerAddressMode addressModeV,
        const vk::SamplerAddressMode addressModeW
    );

private: // static variables
    static uint32_t baseColorDefaultIndex;
    static uint32_t normalDefaultIndex;
    static uint32_t emissionDefaultIndex;
    static uint32_t metallicRoughnessDefaultIndex;
    static std::vector<std::shared_ptr<Texture>> masterList;

private: // member variables
    /**
     * @todo 2023/11/10 We probably only want to have one sampler for each type of texture. It seems
     *   to be the case that all base color textures will have the same type of sampler,
     *   likewise with all normal textures, etc ...
     */
    quartz::rendering::StagedImageBuffer m_stagedImageBuffer;
    vk::UniqueImageView mp_vulkanImageView;
    vk::UniqueSampler mp_vulkanSampler;
};