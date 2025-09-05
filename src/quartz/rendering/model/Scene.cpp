#include<memory>
#include <queue>
#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/model/Scene.hpp"

std::vector<std::shared_ptr<quartz::rendering::Node>>
quartz::rendering::Scene::loadRootNodePtrs(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Scene& gltfScene,
    const std::vector<uint32_t>& materialMasterIndices
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_SCENE, "");

    std::vector<std::shared_ptr<quartz::rendering::Node>> rootNodePtrs;
    rootNodePtrs.reserve(gltfScene.nodes.size());

    LOG_TRACE(MODEL_SCENE, "Loading {} root nodes", gltfScene.nodes.size());

    for (uint32_t i = 0; i < gltfScene.nodes.size(); ++i) {
        const uint32_t currentNodeIndex = gltfScene.nodes[i];
        const tinygltf::Node gltfNode = gltfModel.nodes[currentNodeIndex];
        LOG_TRACE(MODEL_SCENE, "Scene's {}th root node is at index {}", i, currentNodeIndex);

        rootNodePtrs.emplace_back(std::make_shared<quartz::rendering::Node>(
            renderingDevice,
            gltfModel,
            gltfNode,
            nullptr,
            materialMasterIndices
        ));
    }

    return rootNodePtrs;
}

quartz::rendering::Scene::Scene(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Scene& gltfScene,
    const std::vector<uint32_t>& materialMasterIndices
) :
    m_rootNodePtrs(
        quartz::rendering::Scene::loadRootNodePtrs(
            renderingDevice,
            gltfModel,
            gltfScene,
            materialMasterIndices
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Scene::Scene(
    quartz::rendering::Scene&& other
) :
    m_rootNodePtrs(other.m_rootNodePtrs)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Scene::~Scene() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

std::vector<std::shared_ptr<quartz::rendering::Node>>
quartz::rendering::Scene::getAllNodePtrs() const {
    std::vector<std::shared_ptr<quartz::rendering::Node>> masterList;

    std::queue<std::shared_ptr<quartz::rendering::Node>> workingQueue(
        std::deque(
            m_rootNodePtrs.begin(),
            m_rootNodePtrs.end()
        )
    );

    while (!workingQueue.empty()) {
        std::shared_ptr<quartz::rendering::Node> p_currentNode = workingQueue.front();
        workingQueue.pop();

        masterList.push_back(p_currentNode);

        for (const std::shared_ptr<quartz::rendering::Node>& p_childNode : p_currentNode->getChildrenNodePtrs()) {
            workingQueue.push(p_childNode);
        }
    }

    return masterList;
}
