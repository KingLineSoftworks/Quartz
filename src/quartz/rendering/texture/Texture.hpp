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
        Diffuse = 0,
        Normal = 1,
        Occlusion = 2,
        Emissive = 3
    };

public: // static functions

    /**
     * @todo 2023/11/17 Make a "create" function that constructs a texture and inserts
     *   it into the list if it hasn't already been inserted. Return the index at which
     *   the texture was inserted
     *
     * @todo 2023/11/17 When the list is instantiated we should reserve enough space for
     *   QUARTZ_MAX_NUM_TEXTURES
     *
     * @todo 2023/11/17 If the list is currently empty then we should construct a default
     *   texture for each of the texture types to put at the beginning of the list
     */

    /**
     * @todo 2023/11/17 Make getter functionality to get the default of each of the texture
     *   types based on the private static variables we construct when the master list is initialized
     *   for the first time
     */

    /**
     * @todo 2023/11/17 Make getter functionality that gets a std::shared_ptr to a texture
     *   from the master list when given an index
     */

public: // member functions

    /**
     * @todo 2023/11/17 Make these constructors private so we can only construct
     *   them within the "create" static function that is used to put a texture into
     *   the master list
     */

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

    /**
     * @todo 2023/11/17 Create a variable representing the index of each of the default textures
     *   so after the list is created we can retrieve these externally with an accessor function
     */

    /**
     * @todo 2023/11/17 Create a master list of std::shared_ptr to textures that we maintain
     *   and that other things can access based on index
     */

private: // member variables

    /**
     * @todo We probably only want to have one sampler for each type of texture. It seems
     *   to be the case that all base color textures will have the same type of sampler,
     *   likewise with all normal textures, etc ...
     */

    quartz::rendering::StagedImageBuffer m_stagedImageBuffer;
    vk::UniqueImageView mp_vulkanImageView;
    vk::UniqueSampler mp_vulkanSampler;
};