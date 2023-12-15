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
        const std::vector<quartz::rendering::Material>& materials
    );
    Primitive(Primitive&& other);
    ~Primitive();

    USE_LOGGER(MODEL_PRIMITIVE);

    uint32_t getIndexCount() const { return m_indexCount; }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }
    const quartz::rendering::Material& getMaterial() const { return m_material; }

private: // static functions
    static void populateVerticesWithAttribute(
        std::vector<quartz::rendering::Vertex>& verticesToPopulate,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const quartz::rendering::Vertex::AttributeType attributeType
    );
    static quartz::rendering::StagedBuffer createStagedVertexBuffer(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive
    );
    static quartz::rendering::StagedBuffer createStagedIndexBuffer(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive
    );
    static const quartz::rendering::Material& getMaterial(
        const tinygltf::Primitive& gltfPrimitive,
        const std::vector<quartz::rendering::Material>& materials
    );

private: // member variables
    uint32_t m_indexCount;
    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
    const quartz::rendering::Material& m_material;
};