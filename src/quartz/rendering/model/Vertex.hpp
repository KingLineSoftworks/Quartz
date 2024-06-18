#pragma once

#include "math/transform/Vec2.hpp"
#include "math/transform/Vec3.hpp"

#include <vulkan/vulkan.hpp>

namespace quartz {
namespace rendering {
    struct Vertex;
}
}

struct quartz::rendering::Vertex {
public: // enums
    enum class AttributeType : uint32_t {
        Position    = 0,
        Normal      = 1,
        Tangent     = 2,
        Color       = 3,

        BaseColorTextureCoordinate          = 4,
        MetallicRoughnessTextureCoordinate  = 5,
        NormalTextureCoordinate             = 6,
        EmissionTextureCoordinate           = 7,
        OcclusionTextureCoordinate          = 8
    };

public: // member functions
    Vertex();
    Vertex(
        const math::Vec3& position_,
        const math::Vec3& normal_,
        const math::Vec3& color_,
        const math::Vec2& baseColorTextureCoordinate_,
        const math::Vec2& metallicRoughnessTextureCoordinate_,
        const math::Vec2& normalTextureCoordinate_,
        const math::Vec2& emissionTextureCoordinate_,
        const math::Vec2& occlusionTextureCoordinate_
    );
    bool operator==(const Vertex& other) const;

public: // static functions
    static std::string getAttributeNameString(const quartz::rendering::Vertex::AttributeType attributeType);
    static std::string getAttributeGLTFString(const quartz::rendering::Vertex::AttributeType type);
    static vk::VertexInputBindingDescription getVulkanVertexInputBindingDescription();
    static std::vector<vk::VertexInputAttributeDescription> getVulkanVertexInputAttributeDescriptions();

public: // member variables
    math::Vec3 position;
    math::Vec3 normal;
    math::Vec3 tangent;
    math::Vec3 color;

    math::Vec2 baseColorTextureCoordinate;
    math::Vec2 metallicRoughnessTextureCoordinate;
    math::Vec2 normalTextureCoordinate;
    math::Vec2 emissionTextureCoordinate;
    math::Vec2 occlusionTextureCoordinate;
};

template <> struct std::hash<quartz::rendering::Vertex> {
    std::size_t operator()(const quartz::rendering::Vertex& vertex) const {
        std::size_t seed = 0;

        std::vector<float> values = {
            vertex.position.x, vertex.position.y, vertex.position.z,
            vertex.normal.x,   vertex.normal.y,   vertex.normal.z,
            vertex.tangent.x,  vertex.tangent.y,  vertex.tangent.z,
            vertex.color.x,    vertex.color.y,    vertex.color.z,
            vertex.baseColorTextureCoordinate.x,         vertex.baseColorTextureCoordinate.y,
            vertex.metallicRoughnessTextureCoordinate.x, vertex.metallicRoughnessTextureCoordinate.y,
            vertex.normalTextureCoordinate.x,            vertex.normalTextureCoordinate.y,
            vertex.emissionTextureCoordinate.x,          vertex.emissionTextureCoordinate.y,
            vertex.occlusionTextureCoordinate.x,         vertex.occlusionTextureCoordinate.y
        };

        for (const float value : values) {
            seed ^=
                std::hash<float>{}(value) +
                0x9e3779b9 +
                (seed << 6) +
                (seed >> 2)
            ;
        }

        return seed;
    }
};
