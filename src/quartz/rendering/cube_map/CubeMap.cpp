#include "CubeMap.hpp"

#include <glm/vec3.hpp>

vk::VertexInputBindingDescription
quartz::rendering::CubeMap::getVulkanVertexInputBindingDescription() {
    vk::VertexInputBindingDescription vertexInputBindingDescription(
        0,
        sizeof(glm::vec3),
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


quartz::rendering::CubeMap::CubeMap() {}