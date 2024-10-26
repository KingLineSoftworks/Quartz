#include <stb_image.h>

#include "math/transform/Vec3.hpp"

#include "quartz/rendering/cube_map/CubeMap.hpp"
#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"

vk::VertexInputBindingDescription
quartz::rendering::CubeMap::getVulkanVertexInputBindingDescription() {
    vk::VertexInputBindingDescription vertexInputBindingDescription(
        0,
        sizeof(math::Vec3),
        vk::VertexInputRate::eVertex
    );

    return vertexInputBindingDescription;
}

std::vector<vk::VertexInputAttributeDescription>
quartz::rendering::CubeMap::getVulkanVertexInputAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions = {
        vk::VertexInputAttributeDescription(
            0,
            0,
            vk::Format::eR32G32B32Sfloat,
            0
        )
    };

    return vertexInputAttributeDescriptions;
}

quartz::rendering::StagedImageBuffer
quartz::rendering::CubeMap::createStagedImageBufferFromFilepaths(
    const quartz::rendering::Device& renderingDevice,
    const std::string& frontFilepath,
    const std::string& backFilepath,
    const std::string& upFilepath,
    const std::string& downFilepath,
    const std::string& rightFilepath,
    const std::string& leftFilepath
) {
    LOG_FUNCTION_SCOPE_TRACE(CUBEMAP, "");

    LOG_TRACE(CUBEMAP, "Front filepath: {}", frontFilepath);
    LOG_TRACE(CUBEMAP, "Back  filepath: {}", backFilepath);
    LOG_TRACE(CUBEMAP, "Up    filepath: {}", upFilepath);
    LOG_TRACE(CUBEMAP, "Down  filepath: {}", downFilepath);
    LOG_TRACE(CUBEMAP, "Right filepath: {}", rightFilepath);
    LOG_TRACE(CUBEMAP, "Left  filepath: {}", leftFilepath);

    std::array<std::string, 6> imageFilepaths = {
        frontFilepath,
        backFilepath,
        upFilepath,
        downFilepath,
        rightFilepath,
        leftFilepath
    };

    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t imageChannelCount;
    uint32_t imageSize;
    std::vector<uint8_t> pixelBuffer;

    for (uint32_t i = 0; i < imageFilepaths.size(); ++i) {
        const std::string& imageFilepath = imageFilepaths[i];
        LOG_TRACE(CUBEMAP, "Loading image from {}", imageFilepath);

        int32_t currWidth;
        int32_t currHeight;
        int32_t currChannelCount;
        uint8_t* p_currPixels = stbi_load(
            imageFilepath.c_str(),
            &currWidth,
            &currHeight,
            &currChannelCount,
            STBI_rgb_alpha
        );
        if (!p_currPixels) {
            LOG_THROW(CUBEMAP, util::AssetLoadFailedError, "Failed to load texture from {}", imageFilepath);
        }

        // x4 for rgba (32 bits = 4 bytes)
        uint32_t currSize = currWidth * currHeight * 4;
        LOG_TRACE(CUBEMAP, "  Successfully loaded {}x{} image with {} channels ( {} bytes, because WxHx4 for rgba ) from {}", currWidth, currHeight, currChannelCount, currSize, imageFilepath);
        LOG_TRACE(CUBEMAP, "  Loaded pixel data at {}", reinterpret_cast<void*>(p_currPixels));

        if (i == 0) {
            imageWidth = currWidth;
            imageHeight = currHeight;
            imageChannelCount = currChannelCount;
            imageSize = currSize;

            const uint32_t totalSize = imageSize * 6;
            LOG_TRACE(CUBEMAP, "  Allocating buffer of {} * {} = {} bytes to hold all image data", imageSize, 6, totalSize);
            pixelBuffer.reserve(totalSize); // reserve first so we don't over allocate space
            pixelBuffer.resize(totalSize, 0x00);
            LOG_TRACE(CUBEMAP, "  Buffer starting at {}", reinterpret_cast<void*>(pixelBuffer.data()));
        } else if (
            static_cast<uint32_t>(currWidth) != imageWidth ||
            static_cast<uint32_t>(currHeight) != imageHeight ||
            static_cast<uint32_t>(currChannelCount) != imageChannelCount ||
            static_cast<uint32_t>(currSize) != imageSize
        ) {
            LOG_THROW(CUBEMAP, util::AssetInsufficientError, "Loaded image does not match {}x{} image with {} channels ( {} bytes )", imageWidth, imageHeight, imageChannelCount, imageSize);
        }

        const uint32_t pixelBufferOffset = i * imageSize;
        uint8_t* p_copyDestination = pixelBuffer.data() + pixelBufferOffset;

        LOG_TRACE(CUBEMAP, "  Copying data for image {}", i);
        LOG_TRACE(CUBEMAP, "    Using offset of {} bytes", pixelBufferOffset);
        LOG_TRACE(CUBEMAP, "    Copying to destination {}", reinterpret_cast<void*>(p_copyDestination));
        LOG_TRACE(CUBEMAP, "    Copying from source    {}", reinterpret_cast<void*>(p_currPixels));
        memcpy(p_copyDestination, p_currPixels, imageSize);

        LOG_TRACE(CUBEMAP, "  Freeing stbi image");
        stbi_image_free(p_currPixels);
    }

    return {
        renderingDevice,
        static_cast<uint32_t>(imageWidth),
        static_cast<uint32_t>(imageHeight),
        static_cast<uint32_t>(imageChannelCount),
        imageSize,
        6,
        vk::ImageUsageFlagBits::eSampled,
        vk::ImageCreateFlagBits::eCubeCompatible,
        vk::Format::eR8G8B8A8Srgb,
        vk::ImageTiling::eOptimal,
        pixelBuffer.data()
    };
}

quartz::rendering::StagedBuffer
quartz::rendering::CubeMap::createStagedVertexBuffer(
    const quartz::rendering::Device& renderingDevice
) {
    /**
     * @brief Taken from the vertices we load when we load glTF-Sample-Models/2.0/BoxVertexColors/glTF/BoxVertexColors.gltf
     */
    std::vector<math::Vec3> vertices = {
        {0.0f , 0.0f , 0.0f},
        {1.0f , 0.0f , 0.0f},
        {1.0f , 1.0f , 0.0f},
        {0.0f , 1.0f , 0.0f},
        {1.0f , 0.0f , 1.0f},
        {0.0f , 0.0f , 1.0f},
        {0.0f , 1.0f , 1.0f},
        {1.0f , 1.0f , 1.0f},
        {1.0f , 0.0f , 0.0f},
        {1.0f , 0.0f , 1.0f},
        {1.0f , 1.0f , 1.0f},
        {1.0f , 1.0f , 0.0f},
        {0.0f , 0.0f , 1.0f},
        {0.0f , 0.0f , 0.0f},
        {0.0f , 1.0f , 0.0f},
        {0.0f , 1.0f , 1.0f},
        {0.0f , 1.0f , 0.0f},
        {1.0f , 1.0f , 0.0f},
        {1.0f , 1.0f , 1.0f},
        {0.0f , 1.0f , 1.0f},
        {0.0f , 0.0f , 1.0f},
        {1.0f , 0.0f , 1.0f},
        {1.0f , 0.0f , 0.0f},
        {0.0f , 0.0f , 0.0f},
    };
    for (uint32_t i = 0; i < vertices.size(); ++i) {
        vertices[i] = (2.0f * vertices[i]) - math::Vec3(1.0f, 1.0f, 1.0f);
    }

    quartz::rendering::StagedBuffer stagedVertexBuffer(
        renderingDevice,
        sizeof(math::Vec3) * vertices.size(),
        vk::BufferUsageFlagBits::eVertexBuffer,
        vertices.data()
    );

    return stagedVertexBuffer;
}

quartz::rendering::StagedBuffer
quartz::rendering::CubeMap::createStagedIndexBuffer(
    const quartz::rendering::Device& renderingDevice
) {
    /**
     * @brief Taken from the indices when we load glTF-Sample-Models/2.0/BoxVertexColors/glTF/BoxVertexColors.gltf
     */
    const std::vector<uint32_t> indices = {
         0,  2,  1,
         0,  3,  2,

         4,  6,  5,
         4,  7,  6,

         8, 10,  9,
         8, 11, 10,

        12, 14, 13,
        12, 15, 14,

        16, 18, 17,
        16, 19, 18,

        20, 22, 21,
        20, 23, 22,
    };

    quartz::rendering::StagedBuffer indexBuffer(
        renderingDevice,
        sizeof(uint32_t) * indices.size(),
        vk::BufferUsageFlagBits::eIndexBuffer,
        indices.data()
    );

    return indexBuffer;
}

quartz::rendering::CubeMap::CubeMap() :
    m_stagedImageBuffer(),
    mp_vulkanImageView(),
    mp_vulkanCombinedImageSampler(),
    m_stagedVertexBuffer(),
    m_stagedIndexBuffer()
{}

quartz::rendering::CubeMap::CubeMap(
    const quartz::rendering::Device& renderingDevice,
    const std::string& frontFilepath,
    const std::string& backFilepath,
    const std::string& upFilepath,
    const std::string& downFilepath,
    const std::string& rightFilepath,
    const std::string& leftFilepath
) :
    m_stagedImageBuffer(
        quartz::rendering::CubeMap::createStagedImageBufferFromFilepaths(
            renderingDevice,
            frontFilepath,
            backFilepath,
            upFilepath,
            downFilepath,
            rightFilepath,
            leftFilepath
        )
    ),
    mp_vulkanImageView(
        quartz::rendering::VulkanUtil::createVulkanImageViewPtr(
            renderingDevice.getVulkanLogicalDevicePtr(),
            *(m_stagedImageBuffer.getVulkanImagePtr()),
            m_stagedImageBuffer.getVulkanFormat(),
            {},
            vk::ImageAspectFlagBits::eColor,
            vk::ImageViewType::eCube
        )
    ),
    mp_vulkanCombinedImageSampler(
        quartz::rendering::VulkanUtil::createVulkanSamplerPtr(
            renderingDevice.getVulkanPhysicalDevice(),
            renderingDevice.getVulkanLogicalDevicePtr(),
            vk::Filter::eLinear,
            vk::Filter::eLinear,
            vk::SamplerAddressMode::eRepeat,
            vk::SamplerAddressMode::eRepeat,
            vk::SamplerAddressMode::eRepeat
        )
    ),
    m_stagedVertexBuffer(quartz::rendering::CubeMap::createStagedVertexBuffer(renderingDevice)),
    m_stagedIndexBuffer(quartz::rendering::CubeMap::createStagedIndexBuffer(renderingDevice))
{}

quartz::rendering::CubeMap::CubeMap(
    quartz::rendering::CubeMap&& other
) :
    m_stagedImageBuffer(std::move(other.m_stagedImageBuffer)),
    mp_vulkanImageView(std::move(other.mp_vulkanImageView)),
    mp_vulkanCombinedImageSampler(std::move(other.mp_vulkanCombinedImageSampler)),
    m_stagedVertexBuffer(std::move(other.m_stagedVertexBuffer)),
    m_stagedIndexBuffer(std::move(other.m_stagedIndexBuffer))
{}

quartz::rendering::CubeMap::~CubeMap() {}

quartz::rendering::CubeMap&
quartz::rendering::CubeMap::operator=(
    quartz::rendering::CubeMap&& other
) {
    if (this == &other) {
        return *this;
    }

    m_stagedImageBuffer = std::move(other.m_stagedImageBuffer);
    mp_vulkanImageView = std::move(other.mp_vulkanImageView);
    mp_vulkanCombinedImageSampler = std::move(other.mp_vulkanCombinedImageSampler);
    m_stagedVertexBuffer = std::move(other.m_stagedVertexBuffer);
    m_stagedIndexBuffer = std::move(other.m_stagedIndexBuffer);

    return *this;
}
