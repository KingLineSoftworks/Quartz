#pragma once

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/mesh/Vertex.hpp"

namespace quartz {
namespace rendering {
    class NewPrimitive;
}
}

class quartz::rendering::NewPrimitive {
public: // member functions
    NewPrimitive(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive
    );
    NewPrimitive(NewPrimitive&& other);
    ~NewPrimitive();

    USE_LOGGER(MODEL_PRIMITIVE);

    uint32_t getIndexCount() const { return m_indexCount; }
    const quartz::rendering::StagedBuffer& getStagedVertexBuffer() const { return m_stagedVertexBuffer; }
    const quartz::rendering::StagedBuffer& getStagedIndexBuffer() const { return m_stagedIndexBuffer; }

private: // static functions
    void populateVerticesWithAttribute(
        std::vector<quartz::rendering::Vertex>& verticesToPopulate,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const quartz::rendering::Vertex::AttributeType attributeType
    );

    quartz::rendering::StagedBuffer createStagedVertexBuffer(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive
    );

    quartz::rendering::StagedBuffer createStagedIndexBuffer(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive
    );

private: // member variables
    uint32_t m_indexCount;
    quartz::rendering::StagedBuffer m_stagedVertexBuffer;
    quartz::rendering::StagedBuffer m_stagedIndexBuffer;
    /** @todo store the material index */
};