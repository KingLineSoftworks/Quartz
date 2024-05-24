#pragma once

#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/model/Mesh.hpp"

namespace quartz {
namespace rendering {
    class Node;
}
}

class quartz::rendering::Node {
public: // member functions
    Node(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Node& gltfNode,
        const Node* p_parent,
        const std::vector<uint32_t>& materialMasterIndices
    );
    Node(Node&& other);
    ~Node();

    USE_LOGGER(MODEL_NODE);

    const Node* getParentPtr() const { return mp_parent; }
    const std::vector<std::shared_ptr<Node>>& getChildrenNodePtrs() const { return m_childrenPtrs; }
    const glm::mat4& getLocalTransformationMatrix() const { return m_localTransformationMatrix; }
    const std::shared_ptr<quartz::rendering::Mesh>& getMeshPtr() const { return mp_mesh; }

    glm::mat4 getTransformationMatrix() const;

private: // static functions
    std::vector<std::shared_ptr<quartz::rendering::Node>> loadChildrenNodePtrs(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Node& gltfNode,
        const std::vector<uint32_t>& materialMasterIndices
    );

    glm::mat4 loadLocalTransformationMatrix(
        const tinygltf::Node& gltfNode
    );

    std::shared_ptr<quartz::rendering::Mesh> loadMeshPtr(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Node& gltfNode,
        const std::vector<uint32_t>& materialMasterIndices
    );

private: // member functions
    void setParentPtr(const Node* p_parent) { mp_parent = p_parent; }

private: // member variables

    /**
     * @todo 2023/12/08 Track a private static variable which tells us how many nodes
     *   (with meshes) in total have been created so we can give each node (with a
     *   mesh) a unique identifier. This will allow it to put its transformation
     *   matrix into the dynamic uniform buffer at the correct index. We only need to
     *   track the number of nodes with meshes because if a node doesn't have a mesh
     *   then we won't be rendering it, so we don't care about putting its model
     *   matrix into the dynamic uniform buffer.
     */

    const Node* mp_parent;
    std::vector<std::shared_ptr<Node>> m_childrenPtrs;
    glm::mat4 m_localTransformationMatrix;

    std::shared_ptr<quartz::rendering::Mesh> mp_mesh;
};