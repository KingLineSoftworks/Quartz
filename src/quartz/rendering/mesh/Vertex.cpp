#include <glm/vec3.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/mesh/Vertex.hpp"

std::string
quartz::rendering::Vertex::getAttributeGLTFString(
    const quartz::rendering::Vertex::AttributeType type
) {
    switch (type) {
        case quartz::rendering::Vertex::AttributeType::Position:
            return "POSITION";
        case quartz::rendering::Vertex::AttributeType::Color:
            return "COLOR_0";
        case quartz::rendering::Vertex::AttributeType::TextureCoordinate:
            return "TEXCOORD_0";
    }
}

vk::VertexInputBindingDescription
quartz::rendering::Vertex::getVulkanVertexInputBindingDescription() {
    vk::VertexInputBindingDescription vertexInputBindingDescription(
        0,
        sizeof(quartz::rendering::Vertex),
        vk::VertexInputRate::eVertex
    );

    return vertexInputBindingDescription;
}

std::array<vk::VertexInputAttributeDescription, 3>
quartz::rendering::Vertex::getVulkanVertexInputAttributeDescriptions() {
    std::array<
        vk::VertexInputAttributeDescription,
        3
    > vertexInputAttributeDescriptions = {
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
        ),
        vk::VertexInputAttributeDescription(
            2,
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, textureCoordinate)
        )
    };

    return vertexInputAttributeDescriptions;
}

quartz::rendering::Vertex::Vertex() :
    worldPosition(0.0f, 0.0f, 0.0f),
    color(1.0f, 1.0f, 1.0f),
    textureCoordinate(0.0f, 0.0f)
{}

quartz::rendering::Vertex::Vertex(
    const glm::vec3& worldPosition_,
    const glm::vec3& color_,
    const glm::vec2& textureCoordinate_
) :
    worldPosition(worldPosition_),
    color(color_),
    textureCoordinate(textureCoordinate_)
{}

bool
quartz::rendering::Vertex::operator==(
    const quartz::rendering::Vertex& other
) const {
    return (
        worldPosition.x == other.worldPosition.x &&
        worldPosition.y == other.worldPosition.y &&
        worldPosition.z == other.worldPosition.z &&

        color.x == other.color.x &&
        color.y == other.color.y &&
        color.z == other.color.z &&

        textureCoordinate.x == other.textureCoordinate.x &&
        textureCoordinate.y == other.textureCoordinate.y
    );
}