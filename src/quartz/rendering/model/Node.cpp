#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <tiny_gltf.h>

#include "quartz/rendering/model/Mesh.hpp"
#include "quartz/rendering/model/Node.hpp"

std::vector<std::shared_ptr<quartz::rendering::Node>>
quartz::rendering::Node::loadChildrenNodePtrs(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Node& gltfNode
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_NODE, "");

    std::vector<std::shared_ptr<quartz::rendering::Node>> childrenNodePtrs;
    childrenNodePtrs.reserve(gltfNode.children.size());

    for (uint32_t i = 0; i < gltfNode.children.size(); ++i) {
        const uint32_t currentNodeIndex = gltfNode.children[i];
        const tinygltf::Node currentGltfNode = gltfModel.nodes[currentNodeIndex];

        childrenNodePtrs.emplace_back(std::make_shared<quartz::rendering::Node>(
            renderingDevice,
            gltfModel,
            currentGltfNode,
            nullptr
        ));
    }

    return childrenNodePtrs;
}

glm::mat4
quartz::rendering::Node::loadTransformationMatrix(
    const tinygltf::Node& gltfNode
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_NODE, "");

    glm::mat4 transformationMatrix = glm::mat4(1.0f);

    if (gltfNode.matrix.size() == 16) {
        transformationMatrix = glm::make_mat4x4(gltfNode.matrix.data());

        LOG_TRACE(MODEL_NODE, "Using local transformation matrix loaded directly from node");
        LOG_TRACE(MODEL_NODE, "{}", glm::to_string(transformationMatrix));

        return transformationMatrix;
    }

    LOG_TRACE(MODEL_NODE, "Using TRS calculation from node");

    glm::vec3 translation = glm::vec3(0.0f);
    if (gltfNode.translation.size() == 3) {
        translation = glm::make_vec3(gltfNode.translation.data());
        LOG_TRACE(MODEL_NODE, "Loaded translation vector {}", glm::to_string(translation));
    }
    transformationMatrix = glm::translate(transformationMatrix, translation);

    glm::mat4 rotation = glm::mat4(1.0f);
    if (gltfNode.rotation.size() == 4) {
        glm::quat quaternion = glm::make_quat(gltfNode.rotation.data());
        LOG_TRACE(MODEL_NODE, "Loaded rotation quaternion {}", glm::to_string(quaternion));
        rotation = glm::mat4(quaternion);
    }
    transformationMatrix = transformationMatrix * rotation;

    glm::vec3 scale = glm::vec3(1.0f);
    if (gltfNode.scale.size() == 3) {
        scale = glm::make_vec3(gltfNode.scale.data());
        LOG_TRACE(MODEL_NODE, "Loaded scale vector {}", glm::to_string(scale));
    }
    transformationMatrix = glm::scale(transformationMatrix, scale);

    LOG_TRACE(MODEL_NODE, "Calculated local transformation matrix");
    LOG_TRACE(MODEL_NODE, "{}", glm::to_string(transformationMatrix));

    return transformationMatrix;
}

std::shared_ptr<quartz::rendering::NewMesh>
quartz::rendering::Node::loadMeshPtr(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Node& gltfNode
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_NODE, "");

    const int32_t meshIndex = gltfNode.mesh;

    if (meshIndex <= -1) {
        LOG_TRACE(MODEL_NODE, "Node does not contain a mesh ({})", meshIndex);
        return nullptr;
    }

    LOG_TRACE(MODEL_NODE, "Using mesh at index {}", meshIndex);
    const tinygltf::Mesh& gltfMesh = gltfModel.meshes[meshIndex];

    return std::make_shared<quartz::rendering::NewMesh>(
        renderingDevice,
        gltfModel,
        gltfMesh
    );
}

quartz::rendering::Node::Node(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Node& gltfNode,
    const quartz::rendering::Node* p_parent
) :
    mp_parent(p_parent),
    m_childrenPtrs(
        quartz::rendering::Node::loadChildrenNodePtrs(
            renderingDevice,
            gltfModel,
            gltfNode
        )
    ),
    m_transformationMatrix(
        quartz::rendering::Node::loadTransformationMatrix(
            gltfNode
        )
    ),
    mp_mesh(
        quartz::rendering::Node::loadMeshPtr(
            renderingDevice,
            gltfModel,
            gltfNode
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");

    for (const std::shared_ptr<quartz::rendering::Node>& p_node : m_childrenPtrs) {
        p_node->setParentPtr(this);
    }
}

quartz::rendering::Node::Node(
    Node&& other
) :
    mp_parent(std::move(other.mp_parent)),
    m_childrenPtrs(std::move(other.m_childrenPtrs)),
    m_transformationMatrix(std::move(other.m_transformationMatrix)),
    mp_mesh(std::move(other.mp_mesh))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Node::~Node() {
    LOG_FUNCTION_CALL_TRACEthis("");
}