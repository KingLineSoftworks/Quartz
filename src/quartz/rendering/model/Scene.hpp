#pragma once

#include <memory>
#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/model/Node.hpp"

namespace quartz {
namespace rendering {
    class Scene;
}
}

class quartz::rendering::Scene {
public: // member functions
    Scene(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Scene& gltfScene,
        const std::vector<quartz::rendering::Material>& materials
    );
    Scene(Scene&& other);
    ~Scene();

    USE_LOGGER(MODEL_SCENE);

    const std::vector<std::shared_ptr<quartz::rendering::Node>>& getRootNodePtrs() const { return m_rootNodePtrs; }
    std::vector<std::shared_ptr<quartz::rendering::Node>> getAllNodePtrs() const;

private: // static functions
    static std::vector<std::shared_ptr<quartz::rendering::Node>> loadRootNodePtrs(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Scene& gltfScene,
        const std::vector<quartz::rendering::Material>& materials
    );

private: // member variables
    std::vector<std::shared_ptr<quartz::rendering::Node>> m_rootNodePtrs;
};