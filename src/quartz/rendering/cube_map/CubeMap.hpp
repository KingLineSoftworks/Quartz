#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

namespace quartz {
namespace rendering {
    class CubeMap;
}
}

class quartz::rendering::CubeMap {
public: // member functions
    CubeMap();

public: // static functions
    static vk::VertexInputBindingDescription getVulkanVertexInputBindingDescription();
    static std::vector<vk::VertexInputAttributeDescription> getVulkanVertexInputAttributeDescriptions();

private: // member variables

};
