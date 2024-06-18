#include <memory>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <tiny_gltf.h>

#include "quartz/rendering/model/Mesh.hpp"
#include "quartz/rendering/model/Node.hpp"

std::vector<std::shared_ptr<quartz::rendering::Node>>
quartz::rendering::Node::loadChildrenNodePtrs(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Node& gltfNode,
    const std::vector<uint32_t>& materialMasterIndices
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_NODE, "");

    std::vector<std::shared_ptr<quartz::rendering::Node>> childrenNodePtrs;
    childrenNodePtrs.reserve(gltfNode.children.size());

    LOG_TRACE(MODEL_SCENE, "Loading {} child nodes", gltfNode.children.size());

    for (uint32_t i = 0; i < gltfNode.children.size(); ++i) {
        const uint32_t currentNodeIndex = gltfNode.children[i];
        const tinygltf::Node currentGltfNode = gltfModel.nodes[currentNodeIndex];
        LOG_TRACE(MODEL_SCENE, "Node's {}th child node is at index {}", i, currentNodeIndex);

        childrenNodePtrs.emplace_back(std::make_shared<quartz::rendering::Node>(
            renderingDevice,
            gltfModel,
            currentGltfNode,
            nullptr,
            materialMasterIndices
        ));
    }

    return childrenNodePtrs;
}

glm::mat4
quartz::rendering::Node::loadLocalTransformationMatrix(
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

    math::Vec3 translation(0.0f);
    if (gltfNode.translation.size() == 3) {
        translation = glm::make_vec3(gltfNode.translation.data());
        LOG_TRACE(MODEL_NODE, "Loaded translation vector {}", translation.toString());
    }
    transformationMatrix = glm::translate(transformationMatrix, translation.glmVec);

    glm::mat4 rotation = glm::mat4(1.0f);
    if (gltfNode.rotation.size() == 4) {
        glm::quat quaternion = glm::make_quat(gltfNode.rotation.data());
        LOG_TRACE(MODEL_NODE, "Loaded rotation quaternion {}", glm::to_string(quaternion));
        rotation = glm::mat4(quaternion);
    }
    transformationMatrix = transformationMatrix * rotation;

    math::Vec3 scale(1.0f);
    if (gltfNode.scale.size() == 3) {
        scale = glm::make_vec3(gltfNode.scale.data());
        LOG_TRACE(MODEL_NODE, "Loaded scale vector {}", scale.toString());
    }
    transformationMatrix = glm::scale(transformationMatrix, scale.glmVec);

    LOG_TRACE(MODEL_NODE, "Calculated local transformation matrix");
    LOG_TRACE(MODEL_NODE, "{}", glm::to_string(transformationMatrix));

    return transformationMatrix;
}

std::shared_ptr<quartz::rendering::Mesh>
quartz::rendering::Node::loadMeshPtr(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Node& gltfNode,
    const std::vector<uint32_t>& materialMasterIndices
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_NODE, "");

    const int32_t meshIndex = gltfNode.mesh;

    if (meshIndex <= -1) {
        LOG_TRACE(MODEL_NODE, "Node does not contain a mesh ({})", meshIndex);
        return nullptr;
    }

    LOG_TRACE(MODEL_NODE, "Using mesh at index {}", meshIndex);
    const tinygltf::Mesh& gltfMesh = gltfModel.meshes[meshIndex];

    return std::make_shared<quartz::rendering::Mesh>(
        renderingDevice,
        gltfModel,
        gltfMesh,
        materialMasterIndices
    );
}

quartz::rendering::Node::Node(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Node& gltfNode,
    const quartz::rendering::Node* p_parent,
    const std::vector<uint32_t>& materialMasterIndices
) :
    mp_parent(p_parent),
    m_childrenPtrs(
        quartz::rendering::Node::loadChildrenNodePtrs(
            renderingDevice,
            gltfModel,
            gltfNode,
            materialMasterIndices
        )
    ),
    m_localTransformationMatrix(
        quartz::rendering::Node::loadLocalTransformationMatrix(
            gltfNode
        )
    ),
    mp_mesh(
        quartz::rendering::Node::loadMeshPtr(
            renderingDevice,
            gltfModel,
            gltfNode,
            materialMasterIndices
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
    m_localTransformationMatrix(std::move(other.m_localTransformationMatrix)),
    mp_mesh(std::move(other.mp_mesh))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Node::~Node() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

/**
 * @todo 2023/12/07 We should store the overarching transformation matrix as a member
 *   variable as well as the local transformation matrix. We don't want to be computing
 *   this matrix every time we want to use it.
 *   Instead we should calculate the initial transformation matrix upon construction,
 *   then whenever we update our local transformation matrix we should update the
 *   transformation matrix as well.
 */
glm::mat4
quartz::rendering::Node::getTransformationMatrix() const {
    glm::mat4 transformationMatrix = m_localTransformationMatrix;

    if (mp_parent) {
        transformationMatrix = mp_parent->getTransformationMatrix() * m_localTransformationMatrix;
    } else {

        /**
         * @brief 2023/12/15 For some reason our gltf models are loading in upside down, so if this
         *   is a root node then we want to flip it upside down about the z axis. This will cause all
         *   children nodes to also be flipped (causing them to be rightside up).
         *
         * @todo 2023/12/15 Figure out why this is happening. This fixes the problem but forces the
         *   transformation matrices to be non identity matrices.
         */

        glm::mat4 rotationMatrix = glm::mat4(1.0);
        rotationMatrix = glm::rotate(
            rotationMatrix,
            glm::radians(180.0f),
            {0.0f, 0.0f, 1.0f}
        );

        transformationMatrix = rotationMatrix * transformationMatrix;
    }

    return transformationMatrix;
}