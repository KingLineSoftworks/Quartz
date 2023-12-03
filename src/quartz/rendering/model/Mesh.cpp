#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/model/Mesh.hpp"
#include "quartz/rendering/model/Primitive.hpp"

std::vector<quartz::rendering::NewPrimitive>
quartz::rendering::NewMesh::loadPrimitives(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_MESH, "");

    std::vector<quartz::rendering::NewPrimitive> primitives;
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
            gltfPrimitive
        );
    }

    return primitives;
}

quartz::rendering::NewMesh::NewMesh(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Mesh& gltfMesh
) :
    m_primitives(
        quartz::rendering::NewMesh::loadPrimitives(
            renderingDevice,
            gltfModel,
            gltfMesh
        )
    )
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
}

quartz::rendering::NewMesh::NewMesh(
    quartz::rendering::NewMesh&& other
) :
    m_primitives(std::move(other.m_primitives))
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
}

quartz::rendering::NewMesh::~NewMesh() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
}