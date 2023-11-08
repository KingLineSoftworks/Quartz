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
public: // member functions
    Vertex() = default;
    Vertex(
        const glm::vec3& worldPosition_,
        const glm::vec3& color_,
        const glm::vec2& textureCoordinate_
    );
    bool operator==(const Vertex& other) const;

public: // static functions
    static vk::VertexInputBindingDescription getVulkanVertexInputBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 3> getVulkanVertexInputAttributeDescriptions();

public: // member variables
    glm::vec3 worldPosition;
    glm::vec3 color;
    glm::vec2 textureCoordinate;
};

template <> struct std::hash<quartz::rendering::Vertex> {
    std::size_t operator()(const quartz::rendering::Vertex& vertex) const {
        std::size_t seed = 0;

        std::vector<float> values = {
            vertex.worldPosition.x, vertex.worldPosition.y, vertex.worldPosition.z,
            vertex.color.x, vertex.color.y, vertex.color.z,
            vertex.textureCoordinate.x, vertex.textureCoordinate.y
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
