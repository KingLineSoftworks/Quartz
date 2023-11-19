#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/mesh/Vertex.hpp"
#include "quartz/rendering/mesh/Mesh.hpp"

quartz::rendering::Mesh::Mesh(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<quartz::rendering::Vertex>& vertices,
    const std::vector<uint32_t>& indices
) :
    m_vertices(vertices),
    m_indices(indices),
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
    m_vertices(other.m_vertices),
    m_indices(other.m_indices),
    m_stagedVertexBuffer(std::move(other.m_stagedVertexBuffer)),
    m_stagedIndexBuffer(std::move(other.m_stagedIndexBuffer))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Mesh::~Mesh() {
    LOG_FUNCTION_CALL_TRACEthis("");
}