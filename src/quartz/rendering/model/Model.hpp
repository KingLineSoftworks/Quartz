#pragma once

#include <queue>
#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/material/Material.hpp"
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

    const std::vector<quartz::rendering::Mesh>& getMeshes() const { return m_meshes; }
    const quartz::rendering::Material& getMaterial() const { return m_material; }

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

    static std::vector<quartz::rendering::Vertex> loadModelVertices(
        const tinygltf::Model& gltfModel
    );

    static std::vector<uint32_t> loadModelIndices(
        const tinygltf::Model& gltfModel
    );

    static std::vector<quartz::rendering::Vertex> loadMeshVertices(
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh
    );

    static std::vector<uint32_t> loadMeshIndices(
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh
    );

    static std::vector<quartz::rendering::Primitive> loadMeshPrimitives(
        const tinygltf::Model& gltfModel,
        const tinygltf::Mesh& gltfMesh
    );

    static std::vector<quartz::rendering::Mesh> loadMeshes(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel
    );

    static std::vector<uint32_t> loadTextures(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel
    );

    static quartz::rendering::Material loadMaterial(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel
    );

private: // member variables
    const tinygltf::Model m_gltfModel;

    /**
     * @todo 2023/11/26 Should contain all of the vertices and indices here. All
     *   of the primitives should just contain a "master start index" and a
     *   "master end index" which reference the list of vertices stored here. If
     *   we have a "master index list" containing indices [ 5 6 9 2 4 9 2 1 0 ]
     *   and our primitive contains "master start index" of 2 and "master end
     *   index" of 6 then it will contain a view of the indices [ 9 2 4 9 2 ]
     *
     * @todo 2023/11/26 The buffers (vertices and indices) should be stored at
     *   the model level and everything should reference into them
     */

    std::vector<quartz::rendering::Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    std::vector<quartz::rendering::Mesh> m_meshes;

    /**
     * @todo 2023/11/21 Should contain a vector of shared pointers to materials
     *   so it can give weak pointers to the meshes and indices so they know
     *   which materials they should be using.
     *
     * @todo 2023/11/21 The materials should be loaded before the meshes (and consequently
     *   before the primitives) so we can give weak pointers to them
     */

    quartz::rendering::Material m_material;
};