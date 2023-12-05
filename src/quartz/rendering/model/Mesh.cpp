#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/model/Mesh.hpp"
#include "quartz/rendering/model/Primitive.hpp"

std::vector<quartz::rendering::Primitive>
quartz::rendering::Mesh::loadPrimitives(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh,
    const std::vector<quartz::rendering::Material>& materials
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_MESH, "");

    std::vector<quartz::rendering::Primitive> primitives;
    primitives.reserve(gltfMesh.primitives.size());

    LOG_TRACE(MODEL_MESH, "Considering {} primitives", gltfMesh.primitives.size());
    for (uint32_t i = 0; i < gltfMesh.primitives.size(); ++i) {
        const tinygltf::Primitive& gltfPrimitive = gltfMesh.primitives[i];

        if (gltfPrimitive.indices <= -1) {
            LOG_TRACE(MODEL_MESH, "Primitive {} does not contain any indices", i);
            continue;
        }

        primitives.emplace_back(
            renderingDevice,
            gltfModel,
            gltfPrimitive,
            materials
        );
    }

    return primitives;
}

quartz::rendering::Mesh::Mesh(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh,
    const std::vector<quartz::rendering::Material>& materials
) :
    m_primitives(
        quartz::rendering::Mesh::loadPrimitives(
            renderingDevice,
            gltfModel,
            gltfMesh,
            materials
        )
    )
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
}

quartz::rendering::Mesh::Mesh(
    quartz::rendering::Mesh&& other
) :
    m_primitives(std::move(other.m_primitives))
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
}

quartz::rendering::Mesh::~Mesh() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
}