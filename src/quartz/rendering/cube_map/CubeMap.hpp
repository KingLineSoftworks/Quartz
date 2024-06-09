#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
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

public: // static functions
    static vk::VertexInputBindingDescription getVulkanVertexInputBindingDescription();
    static std::vector<vk::VertexInputAttributeDescription> getVulkanVertexInputAttributeDescriptions();

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

private: // member variables
    quartz::rendering::StagedImageBuffer m_stagedImageBuffer;
    vk::UniqueImageView mp_vulkanImageView;

};
