#include <vector>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/mesh/Vertex.hpp"
#include "quartz/rendering/mesh/Mesh.hpp"

bool
quartz::rendering::Mesh::loadVerticesAndIndices(
    const tinyobj::attrib_t& tinyobjAttribute,
    const std::vector<tinyobj::shape_t>& tinyobjShapes,
    std::vector<quartz::rendering::Vertex>& vertices,
    std::vector<uint32_t>& indices
) {
    LOG_FUNCTION_SCOPE_TRACE(MESH, "");

    std::unordered_map<quartz::rendering::Vertex, uint32_t> uniqueVertices;

    for (const tinyobj::shape_t& shape : tinyobjShapes) {
        for (const tinyobj::index_t& index : shape.mesh.indices) {
            glm::vec3 position = {
                tinyobjAttribute.vertices[3 * index.vertex_index + 0],
                tinyobjAttribute.vertices[3 * index.vertex_index + 1],
                tinyobjAttribute.vertices[3 * index.vertex_index + 2]
            };

            glm::vec3 color = {
                1.0f,
                1.0f,
                1.0f
            };

            glm::vec2 textureCoordinate = {
                0.0f + tinyobjAttribute.texcoords[2 * index.texcoord_index + 0],
                1.0f - tinyobjAttribute.texcoords[2 * index.texcoord_index + 1]
            };

            quartz::rendering::Vertex vertex(
                position,
                color,
                textureCoordinate
            );

            if (uniqueVertices.count(vertex) == 0) {
                const uint32_t currentIndex = vertices.size();
                uniqueVertices[vertex] = currentIndex;
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    LOG_TRACE(MESH, "Loaded {} unique vertices", vertices.size());
    LOG_TRACE(MESH, "Loaded {} indices", indices.size());

    return true;
}

quartz::rendering::Mesh::Mesh(
    const quartz::rendering::Device& renderingDevice,
    const tinyobj::attrib_t& tinyobjAttribute,
    const std::vector<tinyobj::shape_t>& tinyobjShapes
) :
    m_vertices(),
    m_indices(),
    m_loadedSuccessfully(
        quartz::rendering::Mesh::loadVerticesAndIndices(
            tinyobjAttribute,
            tinyobjShapes,
            m_vertices,
            m_indices
        )
    ),
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

quartz::rendering::Mesh::Mesh(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<quartz::rendering::Vertex>& vertices,
    const std::vector<uint32_t>& indices
) :
    m_vertices(vertices),
    m_indices(indices),
    m_loadedSuccessfully(true),
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