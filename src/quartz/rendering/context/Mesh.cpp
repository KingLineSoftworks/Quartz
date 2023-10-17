#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Vertex.hpp"
#include "quartz/rendering/context/Mesh.hpp"

std::vector<quartz::rendering::Vertex> quartz::rendering::Mesh::loadVertices() {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::MESH, "");

    std::vector<quartz::rendering::Vertex> vertices = {
        quartz::rendering::Vertex({-0.5f, -0.5f,  0.0f}, {1.0f, 0.0f, 0.0f}),
        quartz::rendering::Vertex({ 0.5f, -0.5f,  0.0f}, {0.0f, 1.0f, 0.0f}),
        quartz::rendering::Vertex({ 0.5f, 0.5f,  0.0f}, {0.0f, 0.0f, 1.0f}),
        quartz::rendering::Vertex({-0.5f,  0.5f,  0.0f}, {1.0f, 1.0f, 1.0f})
    };

    LOG_TRACE(quartz::loggers::MESH, "Loaded {} vertices", vertices.size());

    return vertices;
}

std::vector<uint32_t> quartz::rendering::Mesh::loadIndices() {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::MESH, "");

    std::vector<uint32_t> indices = {
        0, 1, 2,
        2, 3, 0
    };

    LOG_TRACE(quartz::loggers::MESH, "Loaded {} indices", indices.size());

    return indices;
}

quartz::rendering::Mesh::Mesh(const quartz::rendering::Device& renderingDevice) :
    m_vertices(quartz::rendering::Mesh::loadVertices()),
    m_indices(quartz::rendering::Mesh::loadIndices()),
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