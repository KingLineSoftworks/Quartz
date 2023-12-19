#include <glm/vec3.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/model/Vertex.hpp"

std::string
quartz::rendering::Vertex::getAttributeNameString(
    const quartz::rendering::Vertex::AttributeType attributeType
) {
    switch (attributeType) {
        case quartz::rendering::Vertex::AttributeType::Position:
            return "Position";
        case quartz::rendering::Vertex::AttributeType::Normal:
            return "Normal";
        case quartz::rendering::Vertex::AttributeType::Tangent:
            return "Tangent";
        case quartz::rendering::Vertex::AttributeType::Color:
            return "Color";
        case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
            return "Base Color Texture Coordinate";
        case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate:
            return "Normal Texture Coordinate";
        case quartz::rendering::Vertex::AttributeType::EmissiveTextureCoordinate:
            return "Emissive Texture Coordinate";
    }
}

std::string
quartz::rendering::Vertex::getAttributeGLTFString(
    const quartz::rendering::Vertex::AttributeType attributeType
) {
    switch (attributeType) {
        case quartz::rendering::Vertex::AttributeType::Position:
            return "POSITION";
        case quartz::rendering::Vertex::AttributeType::Normal:
            return "NORMAL";
        case quartz::rendering::Vertex::AttributeType::Tangent:
            return "TANGENT";
        case quartz::rendering::Vertex::AttributeType::Color:
            return "COLOR_0";
        case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
        case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate:
        case quartz::rendering::Vertex::AttributeType::EmissiveTextureCoordinate:
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

std::vector<vk::VertexInputAttributeDescription>
quartz::rendering::Vertex::getVulkanVertexInputAttributeDescriptions() {
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions = {
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
            offsetof(quartz::rendering::Vertex, tangent)
        ),
        vk::VertexInputAttributeDescription(
            3,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, color)
        ),
        vk::VertexInputAttributeDescription(
            4,
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, baseColorTextureCoordinate)
        ),
        vk::VertexInputAttributeDescription(
            5,
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, normalTextureCoordinate)
        ),
        vk::VertexInputAttributeDescription(
            6,
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, emissiveTextureCoordinate)
        )
    };

    return vertexInputAttributeDescriptions;
}

quartz::rendering::Vertex::Vertex() :
    position(0.0f, 0.0f, 0.0f),
    normal(1.0f, 0.0f, 0.0f),
    tangent(0.0f, 0.0f, 1.0f),
    color(1.0f, 1.0f, 1.0f),
    baseColorTextureCoordinate(0.0f, 0.0f),
    normalTextureCoordinate(0.0f, 0.0f),
    emissiveTextureCoordinate(0.0f, 0.0f)
{}

quartz::rendering::Vertex::Vertex(
    const glm::vec3& position_,
    const glm::vec3& normal_,
    const glm::vec3& tangent_,
    const glm::vec3& color_,
    const glm::vec2& baseColorTextureCoordinate_,
    const glm::vec2& normalTextureCoordinate_,
    const glm::vec2& emissiveTextureCoordinate_
) :
    position(position_),
    normal(normal_),
    tangent(tangent_),
    color(color_),
    baseColorTextureCoordinate(baseColorTextureCoordinate_),
    normalTextureCoordinate(normalTextureCoordinate_),
    emissiveTextureCoordinate(emissiveTextureCoordinate_)
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

        tangent.x == other.tangent.x &&
        tangent.y == other.tangent.y &&
        tangent.z == other.tangent.z &&

        color.x == other.color.x &&
        color.y == other.color.y &&
        color.z == other.color.z &&

        baseColorTextureCoordinate.x == other.baseColorTextureCoordinate.x &&
        baseColorTextureCoordinate.y == other.baseColorTextureCoordinate.y &&

        normalTextureCoordinate.x == other.normalTextureCoordinate.x &&
        normalTextureCoordinate.y == other.normalTextureCoordinate.y &&

        emissiveTextureCoordinate.x == other.emissiveTextureCoordinate.x &&
        emissiveTextureCoordinate.y == other.emissiveTextureCoordinate.y
    );
}