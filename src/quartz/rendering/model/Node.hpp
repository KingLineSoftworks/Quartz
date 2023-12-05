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
        const std::vector<quartz::rendering::Material>& materials
    );
    Node(Node&& other);
    ~Node();

    USE_LOGGER(MODEL_NODE);

    const Node* getParentPtr() const { return mp_parent; }
    const std::vector<std::shared_ptr<Node>>& getChildrenNodePtrs() const { return m_childrenPtrs; }
    const glm::mat4& getTransformationMatrix() const { return m_transformationMatrix; }
    const std::shared_ptr<quartz::rendering::Mesh>& getMeshPtr() const { return mp_mesh; }

private: // static functions
    std::vector<std::shared_ptr<quartz::rendering::Node>> loadChildrenNodePtrs(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Node& gltfNode,
        const std::vector<quartz::rendering::Material>& materials
    );

    glm::mat4 loadTransformationMatrix(
        const tinygltf::Node& gltfNode
    );

    std::shared_ptr<quartz::rendering::Mesh> loadMeshPtr(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Node& gltfNode,
        const std::vector<quartz::rendering::Material>& materials
    );

private: // member functions
    void setParentPtr(const Node* p_parent) { mp_parent = p_parent; }

private: // member variables
    const Node* mp_parent;
    std::vector<std::shared_ptr<Node>> m_childrenPtrs;

    glm::mat4 m_transformationMatrix;

    std::shared_ptr<quartz::rendering::Mesh> mp_mesh;
};