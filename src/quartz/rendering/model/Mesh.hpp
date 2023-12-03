#pragma once

#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/model/Primitive.hpp"

namespace quartz {
namespace rendering {
    class NewMesh;
}
}

class quartz::rendering::NewMesh {
public: // member functions
    NewMesh(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh
    );
    NewMesh(NewMesh&& other);
    ~NewMesh();

    USE_LOGGER(MODEL_MESH);

    const std::vector<quartz::rendering::NewPrimitive>& getPrimitives() const { return m_primitives; }

private: // static functions
    std::vector<quartz::rendering::NewPrimitive> loadPrimitives(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh
    );

private: // member variables
    std::vector<quartz::rendering::NewPrimitive> m_primitives;
};