#pragma once

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
        const std::vector<uint32_t>& materialMasterIndices
    );
    Primitive(Primitive&& other);
    ~Primitive();

    USE_LOGGER(MODEL_PRIMITIVE);

    uint32_t getIndexCount() const { return m_indices.size(); }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }
    uint32_t getMaterialMasterIndex() const { return m_materialMasterIndex; }

private: // static functions
    // These are helper functions
    static bool handleMissingVertexAttribute(
        std::vector<quartz::rendering::Vertex>& verticesToPopulate,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const std::vector<uint32_t>& indices,
        const quartz::rendering::Vertex::AttributeType attributeType
    );
    static bool handleDefaultTextureAttribute(
        std::vector<quartz::rendering::Vertex>& verticesToPopulate,
        const std::shared_ptr<quartz::rendering::Material>& p_material,
        const quartz::rendering::Vertex::AttributeType attributeType
    );
    static uint32_t determineGltfAccessorByteStride(
        const quartz::rendering::Vertex::AttributeType attributeType,
        const tinygltf::Accessor& accessor,
        const tinygltf::BufferView& bufferView
    );

    // These functions are the actual meat and potatoes
    static uint32_t loadMaterialMasterIndex(
        const tinygltf::Primitive& gltfPrimitive,
        const std::vector<uint32_t>& materialMasterIndices
    );
    static std::vector<uint32_t> loadIndicesFromGltfPrimitive(
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
    uint32_t m_materialMasterIndex;
    std::vector<uint32_t> m_indices;
    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
};