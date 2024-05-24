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
        case quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate:
            return "Metallic Roughness Texture Coordinate";
        case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate:
            return "Normal Texture Coordinate";
        case quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate:
            return "Emission Texture Coordinate";
        case quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate:
            return "Occlusion Texture Coordinate";
    }
}

std::string
quartz::rendering::Vertex::getAttributeGLTFString(
    const quartz::rendering::Vertex::AttributeType type
) {
    switch (type) {
        case quartz::rendering::Vertex::AttributeType::Position:
            return "POSITION";
        case quartz::rendering::Vertex::AttributeType::Normal:
            return "NORMAL";
        case quartz::rendering::Vertex::AttributeType::Tangent:
            return "TANGENT";
        case quartz::rendering::Vertex::AttributeType::Color:
            return "COLOR_0";
        case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
        case quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate:
        case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate:
        case quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate:
        case quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate:
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
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::Position),
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, position)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::Normal),
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, normal)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::Tangent),
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, tangent)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::Color),
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(quartz::rendering::Vertex, color)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate),
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, baseColorTextureCoordinate)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate),
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, metallicRoughnessTextureCoordinate)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate),
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, normalTextureCoordinate)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate),
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, emissionTextureCoordinate)
        ),
        vk::VertexInputAttributeDescription(
            static_cast<uint32_t>(quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate),
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(quartz::rendering::Vertex, occlusionTextureCoordinate)
        )
    };

    return vertexInputAttributeDescriptions;
}

quartz::rendering::Vertex::Vertex() :
    position(0.0f, 0.0f, 0.0f),
    normal(0.0f, 0.0f, 0.0f),
    color(1.0f, 1.0f, 1.0f),
    baseColorTextureCoordinate(0.0f, 0.0f),
    metallicRoughnessTextureCoordinate(0.0f, 0.0f),
    normalTextureCoordinate(0.0f, 0.0f),
    emissionTextureCoordinate(0.0f, 0.0f),
    occlusionTextureCoordinate(0.0f, 0.0f)
{}

quartz::rendering::Vertex::Vertex(
    const glm::vec3& position_,
    const glm::vec3& normal_,
    const glm::vec3& color_,
    const glm::vec2& baseColorTextureCoordinate_,
    const glm::vec2& metallicRoughnessTextureCoordinate_,
    const glm::vec2& normalTextureCoordinate_,
    const glm::vec2& emissionTextureCoordinate_,
    const glm::vec2& occlusionTextureCoordinate_
) :
    position(position_),
    normal(normal_),
    color(color_),
    baseColorTextureCoordinate(baseColorTextureCoordinate_),
    metallicRoughnessTextureCoordinate(metallicRoughnessTextureCoordinate_),
    normalTextureCoordinate(normalTextureCoordinate_),
    emissionTextureCoordinate(emissionTextureCoordinate_),
    occlusionTextureCoordinate(occlusionTextureCoordinate_)
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
        baseColorTextureCoordinate.y == other.baseColorTextureCoordinate.y &&

        metallicRoughnessTextureCoordinate.x == other.metallicRoughnessTextureCoordinate.x &&
        metallicRoughnessTextureCoordinate.y == other.metallicRoughnessTextureCoordinate.y &&

        normalTextureCoordinate.x == other.normalTextureCoordinate.x &&
        normalTextureCoordinate.y == other.normalTextureCoordinate.y &&

        emissionTextureCoordinate.x == other.emissionTextureCoordinate.x &&
        emissionTextureCoordinate.y == other.emissionTextureCoordinate.y &&

        occlusionTextureCoordinate.x == other.occlusionTextureCoordinate.x &&
        occlusionTextureCoordinate.y == other.occlusionTextureCoordinate.y
    );
}