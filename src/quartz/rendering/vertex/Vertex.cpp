#include "quartz/rendering/vertex/Vertex.hpp"
#include <glm/vec3.hpp>

#include <vulkan/vulkan.hpp>

quartz::rendering::Vertex::Vertex(
    const glm::vec3& worldPosition_,
    const glm::vec3& color_
) :
    worldPosition(worldPosition_),
    color(color_)
{}

vk::VertexInputBindingDescription quartz::rendering::Vertex::getVulkanVertexInputBindingDescription() {
    vk::VertexInputBindingDescription vertexInputBindingDescription(
        0,
        sizeof(quartz::rendering::Vertex),
        vk::VertexInputRate::eVertex
    );

    return vertexInputBindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 2> quartz::rendering::Vertex::getVulkanVertexInputAttributeDescriptions() {
    std::array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescriptions = {
        vk::VertexInputAttributeDescription(
            0,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, worldPosition)
        ),
        vk::VertexInputAttributeDescription(
            1,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, color)
        )
    };

    return vertexInputAttributeDescriptions;
}