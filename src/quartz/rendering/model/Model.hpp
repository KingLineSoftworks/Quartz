#pragma once

#include <queue>
#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/mesh/Mesh.hpp"
#include "quartz/rendering/texture/Texture.hpp"

namespace quartz {
namespace rendering {
    class Model;
}
}

class quartz::rendering::Model {
public: // member functions
    Model(
        const quartz::rendering::Device& renderingDevice,
        const std::string& objectFilepath
    );
    Model(Model&& other);
    ~Model();

    USE_LOGGER(MODEL);

    const quartz::rendering::Mesh& getMesh() const { return m_mesh; }
    const quartz::rendering::Texture& getTexture() const { return m_texture; }

private: // static functions

    static tinygltf::Model loadGLTFModel(const std::string& filepath);

    static const tinygltf::Scene& loadDefaultScene(const tinygltf::Model& gltfModel);

    static std::queue<const tinygltf::Node*> loadNodePtrQueue(
        const tinygltf::Model& gltfModel,
        const tinygltf::Scene& gltfScene
    );

    static void populateVerticesWithAttribute(
        std::vector<quartz::rendering::Vertex>& verticesToPopulate,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const quartz::rendering::Vertex::AttributeType attributeType
    );

    static std::vector<quartz::rendering::Vertex> loadMeshVertices(
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh
    );

    static std::vector<uint32_t> loadMeshIndices(
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh
    );

    static std::vector<quartz::rendering::Mesh> loadMeshes(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel
    );

    static std::vector<quartz::rendering::Texture> loadTextures(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel
    );

private: // member variables
    const tinygltf::Model m_gltfModel;

    quartz::rendering::Mesh m_mesh;
    quartz::rendering::Texture m_texture;
};