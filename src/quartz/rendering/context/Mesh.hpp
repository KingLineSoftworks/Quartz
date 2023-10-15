#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

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
    Mesh(const quartz::rendering::Device& renderingDevice);
    Mesh(Mesh&& other);
    ~Mesh();

    USE_LOGGER(MESH);

private: // static functions
    static std::vector<quartz::rendering::Vertex> loadVertices();
    static std::vector<uint32_t> loadIndices();

private: // member variables
    std::vector<quartz::rendering::Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
};