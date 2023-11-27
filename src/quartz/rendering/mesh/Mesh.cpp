#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/mesh/Vertex.hpp"
#include "quartz/rendering/mesh/Mesh.hpp"

//std::vector<uint32_t>
//quartz::rendering::Mesh::loadIndicesFromPrimitives(
//    const std::vector<quartz::rendering::Primitive>& primitives
//) {
//    LOG_FUNCTION_SCOPE_TRACE(MESH, "");
//
//    std::vector<uint32_t> indices;
//
//    for (uint32_t i = 0; i < primitives.size(); ++i) {
//        const quartz::rendering::Primitive& primitive = primitives[i];
//
//        LOG_TRACE(MESH, "Inserting {} indices from primitive {}", primitive.getIndices().size(), i);
//
//        indices.insert(
//            indices.end(),
//            primitive.getIndices().begin(),
//            primitive.getIndices().end()
//        );
//    }
//
//    LOG_TRACE(MESH, "Got {} total indices", indices.size());
//
//    return indices;
//}

quartz::rendering::Mesh::Mesh(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<quartz::rendering::Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    const std::vector<quartz::rendering::Primitive>& primitives
) :
    m_vertices(vertices),
    m_indices(indices),
    m_primitives(primitives),
    m_stagedVertexBuffer(
        renderingDevice,
        sizeof(quartz::rendering::Vertex) * m_vertices.size(),
        vk::BufferUsageFlagBits::eVertexBuffer,
        m_vertices.data()
    ),
    m_stagedIndexBuffer(
        renderingDevice,
        sizeof(uint32_t) * m_indices.size(),
        vk::BufferUsageFlagBits::eIndexBuffer,
        m_indices.data()
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Mesh::Mesh(quartz::rendering::Mesh&& other) :
    m_vertices(std::move(other.m_vertices)),
    m_indices(std::move(other.m_indices)),
    m_primitives(std::move(other.m_primitives)),
    m_stagedVertexBuffer(std::move(other.m_stagedVertexBuffer)),
    m_stagedIndexBuffer(std::move(other.m_stagedIndexBuffer))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Mesh::~Mesh() {
    LOG_FUNCTION_CALL_TRACEthis("");
}