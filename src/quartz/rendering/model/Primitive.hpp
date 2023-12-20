#pragma once

#include <memory>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/model/Vertex.hpp"

namespace quartz {
namespace rendering {
    class Primitive;
}
}

class quartz::rendering::Primitive {
public: // member functions
    Primitive(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const std::vector<uint32_t>& masterMaterialIndices
    );
    Primitive(Primitive&& other);
    ~Primitive();

    USE_LOGGER(MODEL_PRIMITIVE);

    uint32_t getMaterialIndex() const { return m_materialIndex; }
    uint32_t getIndexCount() const { return m_indexCount; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }

private: // static functions
    static uint32_t getMaterial(
        const tinygltf::Primitive& gltfPrimitive,
        const std::vector<uint32_t>& masterMaterialIndices
    );
    static std::vector<uint32_t> getIndices(
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive
    );
    static void populateVerticesWithAttribute(
        std::vector<quartz::rendering::Vertex>& verticesToPopulate,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const std::shared_ptr<quartz::rendering::Material>& p_material,
        const std::vector<uint32_t>& indices,
        const quartz::rendering::Vertex::AttributeType attributeType
    );
    static quartz::rendering::StagedBuffer createStagedVertexBuffer(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const std::shared_ptr<quartz::rendering::Material>& p_material,
        const std::vector<uint32_t>& indices
    );

private: // member variables
    const uint32_t m_materialIndex;
    uint32_t m_indexCount;
    std::vector<uint32_t> m_indices;
    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
};