#pragma once

#include <vector>

#include <tiny_gltf.h>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/mesh/Primitive.hpp"
#include "quartz/rendering/mesh/Vertex.hpp"

namespace quartz {
namespace rendering {
    class Mesh;
}
}

class quartz::rendering::Mesh {
public: // member functions

    /**
     * @todo 2023/11/21 This should take in weak pointers to materials for the primitive
     *   to track
     */

    Mesh(
        const quartz::rendering::Device& renderingDevice,
        const std::vector<quartz::rendering::Vertex>& vertices,
        const std::vector<uint32_t>& indices,
        const std::vector<quartz::rendering::Primitive>& primitives
    );
    Mesh(Mesh&& other);
    ~Mesh();

    USE_LOGGER(MESH);

    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    const std::vector<quartz::rendering::Primitive>& getPrimitives() const { return m_primitives; }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }

private: // static functions

    static std::vector<uint32_t> loadIndicesFromPrimitives(
        const std::vector<quartz::rendering::Primitive>& primitives
    );

private: // member variables

    /**
     * @todo 2023/11/21 Instantiate the primitives with the correct material and indices view
     */

    std::vector<quartz::rendering::Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
    std::vector<quartz::rendering::Primitive> m_primitives;

    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
};