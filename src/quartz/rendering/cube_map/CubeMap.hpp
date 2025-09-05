#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class CubeMap;
}
}

class quartz::rendering::CubeMap {
public: // member functions
    CubeMap();
    CubeMap(
        const quartz::rendering::Device& renderingDevice,
        const std::string& frontFilepath,
        const std::string& backFilepath,
        const std::string& upFilepath,
        const std::string& downFilepath,
        const std::string& rightFilepath,
        const std::string& leftFilepath
    );
    CubeMap(CubeMap&& other);
    ~CubeMap();

    CubeMap& operator=(CubeMap&& other);

    USE_LOGGER(CUBEMAP);

    const quartz::rendering::StagedImageBuffer& getStagedImageBuffer() const { return m_stagedImageBuffer; }
    const vk::UniqueImageView& getVulkanImageViewPtr() const { return mp_vulkanImageView; }
    const vk::UniqueSampler& getVulkanSamplerPtr() const { return mp_vulkanCombinedImageSampler; }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }

public: // static functions
    static vk::VertexInputBindingDescription getVulkanVertexInputBindingDescription();
    static std::vector<vk::VertexInputAttributeDescription> getVulkanVertexInputAttributeDescriptions();
    static uint32_t getIndexCount() { return 6 * 6; }

private: // static functions
    quartz::rendering::StagedImageBuffer createStagedImageBufferFromFilepaths (
        const quartz::rendering::Device& renderingDevice,
        const std::string& frontFilepath,
        const std::string& backFilepath,
        const std::string& upFilepath,
        const std::string& downFilepath,
        const std::string& rightFilepath,
        const std::string& leftFilepath
    );
    quartz::rendering::StagedBuffer createStagedVertexBuffer(const quartz::rendering::Device& renderingDevice);
    quartz::rendering::StagedBuffer createStagedIndexBuffer(const quartz::rendering::Device& renderingDevice);

private: // member variables
    quartz::rendering::StagedImageBuffer m_stagedImageBuffer;
    vk::UniqueImageView mp_vulkanImageView;
    vk::UniqueSampler mp_vulkanCombinedImageSampler;
    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
};
