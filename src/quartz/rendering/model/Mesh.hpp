#pragma once

#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/model/Primitive.hpp"

namespace quartz {
namespace rendering {
    class Mesh;
}
}

class quartz::rendering::Mesh {
public: // member functions
    Mesh(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh,
        const std::vector<uint32_t>& masterMaterialIndices
    );
    Mesh(Mesh&& other);
    ~Mesh();

    USE_LOGGER(MODEL_MESH);

    const std::vector<quartz::rendering::Primitive>& getPrimitives() const { return m_primitives; }

private: // static functions
    std::vector<quartz::rendering::Primitive> loadPrimitives(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh,
        const std::vector<uint32_t>& masterMaterialIndices
    );

private: // member variables
    std::vector<quartz::rendering::Primitive> m_primitives;
};