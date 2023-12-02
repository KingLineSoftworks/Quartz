#include<memory>
#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/model/Scene.hpp"

std::vector<std::shared_ptr<quartz::rendering::Node>>
quartz::rendering::Scene::loadRootNodePtrs(
    const tinygltf::Model& gltfModel,
    const tinygltf::Scene& gltfScene
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_SCENE, "");

    std::vector<std::shared_ptr<quartz::rendering::Node>> rootNodePtrs;
    rootNodePtrs.reserve(gltfScene.nodes.size());

    for (uint32_t i = 0; i < gltfScene.nodes.size(); ++i) {
        const uint32_t currentNodeIndex = gltfScene.nodes[i];
        const tinygltf::Node gltfNode = gltfModel.nodes[currentNodeIndex];

        rootNodePtrs.emplace_back(std::make_shared<quartz::rendering::Node>(
            gltfModel,
            gltfNode,
            nullptr
        ));
    }

    return rootNodePtrs;
}

quartz::rendering::Scene::Scene(
    const tinygltf::Model& gltfModel,
    const tinygltf::Scene& gltfScene
) :
    m_rootNodePtrs(
        quartz::rendering::Scene::loadRootNodePtrs(
            gltfModel,
            gltfScene
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Scene::Scene(
    UNUSED const quartz::rendering::Scene& other
) :
    m_rootNodePtrs()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Scene::Scene(
    UNUSED quartz::rendering::Scene&& other
) :
    m_rootNodePtrs()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Scene::~Scene() {
    LOG_FUNCTION_CALL_TRACEthis("");
}
