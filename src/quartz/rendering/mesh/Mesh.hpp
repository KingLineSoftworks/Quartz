#pragma once

#include <vector>

#include <tiny_gltf.h>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/mesh/Vertex.hpp"

namespace quartz {
namespace rendering {
    class Mesh;
}
}

class quartz::rendering::Mesh {
public: // member functions
    Mesh(
        const quartz::rendering::Device& renderingDevice,
        const std::vector<quartz::rendering::Vertex>& vertices,
        const std::vector<uint32_t>& indices
    );
    Mesh(Mesh&& other);
    ~Mesh();

    USE_LOGGER(MESH);

    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }

private: // static functions

private: // member variables
    std::vector<quartz::rendering::Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
};