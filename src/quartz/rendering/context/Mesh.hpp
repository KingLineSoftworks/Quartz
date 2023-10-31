#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include <tiny_obj_loader.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Vertex.hpp"

namespace quartz {
namespace rendering {
    class Mesh;
}
}

class quartz::rendering::Mesh {
public: // member functions
    Mesh(
        const quartz::rendering::Device& renderingDevice,
        const tinyobj::attrib_t& tinyobjAttribute,
        const std::vector<tinyobj::shape_t>& tinyobjShapes
    );
    Mesh(Mesh&& other);
    ~Mesh();

    USE_LOGGER(MESH);

    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }

private: // static functions
    static bool loadVerticesAndIndices(
        const tinyobj::attrib_t& tinyobjAttribute,
        const std::vector<tinyobj::shape_t>& tinyobjShapes,
        std::vector<quartz::rendering::Vertex>& vertices,
        std::vector<uint32_t>& indices
    );

private: // member variables
    std::vector<quartz::rendering::Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    bool m_loadedSuccessfully;

    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
};