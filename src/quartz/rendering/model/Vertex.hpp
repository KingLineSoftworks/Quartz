#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vulkan/vulkan.hpp>

namespace quartz {
namespace rendering {
    struct Vertex;
}
}

struct quartz::rendering::Vertex {
public: // enums
    enum class AttributeType {
        Position,
        Normal,
        Tangent,
        Color,
        BaseColorTextureCoordinate,
        NormalTextureCoordinate
    };

public: // member functions
    Vertex();
    Vertex(
        const glm::vec3& position_,
        const glm::vec3& normal_,
        const glm::vec3& tangent_,
        const glm::vec3& color_,
        const glm::vec2& baseColorTextureCoordinate_,
        const glm::vec2& normalTextureCoordinate_
    );
    bool operator==(const Vertex& other) const;

public: // static functions
    static std::string getAttributeGLTFString(const quartz::rendering::Vertex::AttributeType type);
    static vk::VertexInputBindingDescription getVulkanVertexInputBindingDescription();
    static std::vector<vk::VertexInputAttributeDescription> getVulkanVertexInputAttributeDescriptions();

public: // member variables
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 color;
    glm::vec2 baseColorTextureCoordinate;
    glm::vec2 normalTextureCoordinate;
    /**
     * @todo 2023/12/18 Add a normal map texture coordinate attribute. We are curently using the base color
     *   texture coordinate for our normal map coordinates and that doesn't work when we have to use the
     *   default normal map or a normal map of a different size / layout.
     */
};

template <> struct std::hash<quartz::rendering::Vertex> {
    std::size_t operator()(const quartz::rendering::Vertex& vertex) const {
        std::size_t seed = 0;

        std::vector<float> values = {
            vertex.position.x, vertex.position.y, vertex.position.z,
            vertex.normal.x, vertex.normal.y, vertex.normal.z,
            vertex.tangent.x, vertex.tangent.y, vertex.tangent.z,
            vertex.color.x, vertex.color.y, vertex.color.z,
            vertex.baseColorTextureCoordinate.x, vertex.baseColorTextureCoordinate.y,
            vertex.normalTextureCoordinate.x, vertex.normalTextureCoordinate.y
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
