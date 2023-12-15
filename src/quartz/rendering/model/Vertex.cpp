#include <glm/vec3.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/model/Vertex.hpp"

std::string
quartz::rendering::Vertex::getAttributeGLTFString(
    const quartz::rendering::Vertex::AttributeType type
) {
    switch (type) {
        case quartz::rendering::Vertex::AttributeType::Position:
            return "POSITION";
        case quartz::rendering::Vertex::AttributeType::Normal:
            return "NORMAL";
        case quartz::rendering::Vertex::AttributeType::Color:
            return "COLOR_0";
        case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
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

std::array<vk::VertexInputAttributeDescription, 4>
quartz::rendering::Vertex::getVulkanVertexInputAttributeDescriptions() {
    std::array<
        vk::VertexInputAttributeDescription,
        4
    > vertexInputAttributeDescriptions = {
        vk::VertexInputAttributeDescription(
            0,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, position)
        ),
        vk::VertexInputAttributeDescription(
            1,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, normal)
        ),
        vk::VertexInputAttributeDescription(
            2,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, color)
        ),
        vk::VertexInputAttributeDescription(
            3,
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, baseColorTextureCoordinate)
        )
    };

    return vertexInputAttributeDescriptions;
}

quartz::rendering::Vertex::Vertex() :
    position(0.0f, 0.0f, 0.0f),
    normal(0.0f, 0.0f, 0.0f),
    color(1.0f, 1.0f, 1.0f),
    baseColorTextureCoordinate(0.0f, 0.0f)
{}

quartz::rendering::Vertex::Vertex(
    const glm::vec3& position_,
    const glm::vec3& normal_,
    const glm::vec3& color_,
    const glm::vec2& baseColorTextureCoordinate_
) :
    position(position_),
    normal(normal_),
    color(color_),
    baseColorTextureCoordinate(baseColorTextureCoordinate_)
{}

bool
quartz::rendering::Vertex::operator==(
    const quartz::rendering::Vertex& other
) const {
    return (
        position.x == other.position.x &&
        position.y == other.position.y &&
        position.z == other.position.z &&

        normal.x == other.normal.x &&
        normal.y == other.normal.y &&
        normal.z == other.normal.z &&

        color.x == other.color.x &&
        color.y == other.color.y &&
        color.z == other.color.z &&

        baseColorTextureCoordinate.x == other.baseColorTextureCoordinate.x &&
        baseColorTextureCoordinate.y == other.baseColorTextureCoordinate.y
    );
}