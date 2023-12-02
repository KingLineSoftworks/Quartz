#pragma once

#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"

namespace quartz {
namespace rendering {
    class Node;
}
}

class quartz::rendering::Node {
public: // member functions
    Node(
        const tinygltf::Model& gltfModel,
        const tinygltf::Node& gltfNode,
        const Node* p_parent
    );
    Node(const Node& other);
    Node(Node&& other);
    ~Node();

    USE_LOGGER(MODEL_NODE);

private: // static functions
    std::vector<std::shared_ptr<quartz::rendering::Node>> loadChildrenNodePtrs(
        const tinygltf::Model& gltfModel,
        const tinygltf::Node& gltfNode
    );

    glm::mat4 loadTransformationMatrix(
        const tinygltf::Node& gltfNode
    );

private: // member functions
    void setParentPtr(const Node* p_parent) { mp_parent = p_parent; }

private: // member variables
    const Node* mp_parent;
    std::vector<std::shared_ptr<Node>> m_childrenPtrs;

    glm::mat4 m_transformationMatrix;

};