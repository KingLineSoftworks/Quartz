#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/model/TangentCalculator.hpp"
#include "quartz/rendering/model/Vertex.hpp"
#include "quartz/rendering/model/Primitive.hpp"

void
quartz::rendering::Primitive::populateVerticesWithAttribute(
    std::vector<quartz::rendering::Vertex>& verticesToPopulate,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<uint32_t>& indices,
    const quartz::rendering::Vertex::AttributeType attributeType
) {
    const std::string attributeString =
        quartz::rendering::Vertex::getAttributeGLTFString(attributeType);
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "{} with {} indices", attributeString, indices.size());

    if (gltfPrimitive.attributes.find(attributeString) == gltfPrimitive.attributes.end()) {
        LOG_TRACE(MODEL_PRIMITIVE, "Primitive does not contain a {} attribute", attributeString);

        switch (attributeType) {
            case quartz::rendering::Vertex::AttributeType::Position:
            case quartz::rendering::Vertex::AttributeType::Normal:
                LOG_CRITICAL(MODEL_PRIMITIVE, "Primitive must contain a {} attribute", attributeString);
                throw std::runtime_error("");
            case quartz::rendering::Vertex::AttributeType::Tangent:
                LOG_INFO(MODEL_PRIMITIVE, "Manually calculating vertex tangents. Assuming other attributes are already populated");
                quartz::rendering::TangentCalculator::populateVerticesWithTangents(
                    gltfModel,
                    gltfPrimitive,
                    indices,
                    verticesToPopulate
                );
                return;
            case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
                LOG_TRACE(MODEL_PRIMITIVE, "Using default base color texture");
                break;
            default:
                break;
        }

        return;
    }

    LOG_TRACE(MODEL_PRIMITIVE, "Loading {} attribute", attributeString);

    const uint32_t accessorIndex = gltfPrimitive.attributes.find(attributeString)->second;
    LOG_TRACE(MODEL_PRIMITIVE, "Using accessor at index {}", accessorIndex);
    const tinygltf::Accessor& accessor = gltfModel.accessors[accessorIndex];

    const uint32_t bufferViewIndex = accessor.bufferView;
    LOG_TRACE(MODEL_PRIMITIVE, "Using buffer view at index {}", bufferViewIndex);
    const tinygltf::BufferView& bufferView = gltfModel.bufferViews[bufferViewIndex];

    const uint32_t bufferIndex = bufferView.buffer;
    LOG_TRACE(MODEL_PRIMITIVE, "Using buffer at index {}", bufferIndex);
    const tinygltf::Buffer& buffer = gltfModel.buffers[bufferIndex];

    const uint32_t accessorByteOffset = accessor.byteOffset;
    const uint32_t bufferViewByteOffset = bufferView.byteOffset;
    const std::vector<uint8_t>& bufferData = buffer.data;
    const uint8_t* bufferDataStartAddress = bufferData.data();
    const uint8_t* desiredDataStartAddress = bufferDataStartAddress + accessorByteOffset + bufferViewByteOffset;
    LOG_TRACE(MODEL_PRIMITIVE, "Accessor uses byte offset of {}", accessorByteOffset);
    LOG_TRACE(MODEL_PRIMITIVE, "Buffer view uses byte offset of {}", bufferViewByteOffset);
    LOG_TRACE(MODEL_PRIMITIVE, "Using total byte offset of {}", accessorByteOffset + bufferViewByteOffset);
    LOG_TRACE(MODEL_PRIMITIVE, "Buffer data starting at {}", reinterpret_cast<const void*>(bufferDataStartAddress));
    LOG_TRACE(MODEL_PRIMITIVE, "Using data starting at {}", reinterpret_cast<const void*>(desiredDataStartAddress));
    LOG_TRACE(
        MODEL_PRIMITIVE, "{} + {} = {} SHOULD EQUAL {}",
        reinterpret_cast<const void*>(bufferDataStartAddress),
        accessorByteOffset + bufferViewByteOffset,
        reinterpret_cast<const void*>(bufferDataStartAddress + accessorByteOffset + bufferViewByteOffset),
        reinterpret_cast<const void*>(desiredDataStartAddress)
    );

    const float* p_data = reinterpret_cast<const float*>(desiredDataStartAddress);

    uint32_t tinygltfVecType = TINYGLTF_TYPE_VEC3;
    if (attributeType == quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate) {
        LOG_TRACE(MODEL_PRIMITIVE, "{} attribute uses vector2", attributeString);
        tinygltfVecType = TINYGLTF_TYPE_VEC2;
    }

    bool hasTangent = true;
    if (attributeType == quartz::rendering::Vertex::AttributeType::Normal) {
        const std::string tangentString = quartz::rendering::Vertex::getAttributeGLTFString(
            quartz::rendering::Vertex::AttributeType::Tangent
        );

        if (gltfPrimitive.attributes.find(tangentString) == gltfPrimitive.attributes.end()) {
            LOG_INFO(MODEL_PRIMITIVE, "No tangent attribute present. Approximating them using normal");
            hasTangent = false;
        }
    }

    const int32_t byteStride = accessor.ByteStride(bufferView) ?
        accessor.ByteStride(bufferView) / sizeof (float) :
        tinygltf::GetNumComponentsInType(tinygltfVecType);
    LOG_TRACE(MODEL_PRIMITIVE, "Using a byte stride of {}", byteStride);

    for (uint32_t i = 0; i < verticesToPopulate.size(); ++i) {
        switch (attributeType) {
            case quartz::rendering::Vertex::AttributeType::Position: {
                verticesToPopulate[i].position = glm::make_vec3(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Normal: {
                verticesToPopulate[i].normal = glm::make_vec3(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Tangent: {
                verticesToPopulate[i].tangent = glm::make_vec3(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::Color: {
                verticesToPopulate[i].color = glm::make_vec3(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate: {
                verticesToPopulate[i].baseColorTextureCoordinate = glm::make_vec2(&p_data[i * byteStride]);
                break;
            }
        }
    }
}

std::vector<uint32_t>
quartz::rendering::Primitive::getIndices(
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "");

    const uint32_t indexAccessorIndex = gltfPrimitive.indices; // will always be >= 0
    const tinygltf::Accessor& indexAccessor = gltfModel.accessors[indexAccessorIndex];
    const uint32_t indexCount = indexAccessor.count;

    const uint32_t indexBufferViewIndex = indexAccessor.bufferView;
    const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews[indexBufferViewIndex];

    const uint32_t indexBufferIndex = indexBufferView.buffer;
    const tinygltf::Buffer& indexBuffer = gltfModel.buffers[indexBufferIndex];

    const uint32_t indexAccessorByteOffset = indexAccessor.byteOffset;
    const uint32_t indexBufferViewByteOffset = indexBufferView.byteOffset;
    const std::vector<uint8_t>& indexBufferData = indexBuffer.data;
    const uint8_t* indexBufferDataStartAddress = indexBufferData.data();
    const uint8_t* desiredIndexDataStartAddress = indexBufferDataStartAddress + indexAccessorByteOffset + indexBufferViewByteOffset;

    std::vector<uint32_t> indices(indexCount);

    switch (indexAccessor.componentType) {
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
            LOG_TRACE(MODEL_PRIMITIVE, "  - using {} indices of type uint32_t", indexCount);
            const uint32_t* p_indices = reinterpret_cast<const uint32_t*>(desiredIndexDataStartAddress);
            for (uint32_t j = 0; j < indexCount; ++j) {
                indices[j] = p_indices[j];
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            LOG_TRACE(MODEL_PRIMITIVE, "  - using {} indices of type uint16_t", indexCount);
            const uint16_t* p_indices = reinterpret_cast<const uint16_t*>(desiredIndexDataStartAddress);
            for (uint16_t j = 0; j < indexCount; ++j) {
                indices[j] = p_indices[j];
            }
            break;
        }
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            LOG_TRACE(MODEL_PRIMITIVE, "  - using {} indices of type uint8_t", indexCount);
            const uint8_t* p_indices = reinterpret_cast<const uint8_t*>(desiredIndexDataStartAddress);
            for (uint8_t j = 0; j < indexCount; ++j) {
                indices[j] = p_indices[j];
            }
            break;
        }
    }

    LOG_TRACE(MODEL_PRIMITIVE, "Successfully loaded {} indices for primitive", indexCount);
    return indices;
}

quartz::rendering::StagedBuffer
quartz::rendering::Primitive::createStagedVertexBuffer(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<uint32_t>& indices
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "");

    const uint32_t accessorIndex = gltfPrimitive.attributes.find("POSITION")->second;
    const uint32_t vertexCount = gltfModel.accessors[accessorIndex].count;

    std::vector<quartz::rendering::Vertex> vertices(vertexCount);

    std::vector<quartz::rendering::Vertex::AttributeType> attributeTypes = {
        quartz::rendering::Vertex::AttributeType::Position,
        quartz::rendering::Vertex::AttributeType::Normal,
        quartz::rendering::Vertex::AttributeType::Color,
        quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate,
        quartz::rendering::Vertex::AttributeType::Tangent, // do tangents last so we can use everything else in calculations
    };

    for (const quartz::rendering::Vertex::AttributeType attributeType : attributeTypes) {
        quartz::rendering::Primitive::populateVerticesWithAttribute(
            vertices,
            gltfModel,
            gltfPrimitive,
            indices,
            attributeType
        );
    }

    LOG_TRACE(MODEL_PRIMITIVE, "Successfully populated {} vertices", vertexCount);

    quartz::rendering::StagedBuffer stagedVertexBuffer(
        renderingDevice,
        sizeof(quartz::rendering::Vertex) * vertices.size(),
        vk::BufferUsageFlagBits::eVertexBuffer,
        vertices.data()
    );

    return stagedVertexBuffer;
}

const quartz::rendering::Material&
quartz::rendering::Primitive::getMaterial(
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<quartz::rendering::Material>& materials
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "");

    int32_t materialIndex = gltfPrimitive.material;
    LOG_TRACE(MODEL_PRIMITIVE, "Primitive uses material {}", materialIndex);

    if (materialIndex < 0) {
        LOG_TRACE(MODEL_PRIMITIVE, "Actually using material 0 (default)");
        return materials[0];
    }

    materialIndex += 1;
    LOG_TRACE(MODEL_PRIMITIVE, "Actually using material {}", materialIndex);
    LOG_TRACE(MODEL_PRIMITIVE, "  (to account for default material at index 0)");

    return materials[materialIndex];
}

quartz::rendering::Primitive::Primitive(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<quartz::rendering::Material>& materials
) :
    m_indexCount(gltfModel.accessors[gltfPrimitive.indices].count),
    m_indices(
        quartz::rendering::Primitive::getIndices(
            gltfModel,
            gltfPrimitive
        )
    ),
    m_stagedVertexBuffer(
        quartz::rendering::Primitive::createStagedVertexBuffer(
            renderingDevice,
            gltfModel,
            gltfPrimitive,
            m_indices
        )
    ),
    m_stagedIndexBuffer(
        quartz::rendering::StagedBuffer(
            renderingDevice,
            sizeof(uint32_t) * m_indices.size(),
            vk::BufferUsageFlagBits::eIndexBuffer,
            m_indices.data()
        )
    ),
    m_material(
        quartz::rendering::Primitive::getMaterial(
            gltfPrimitive,
            materials
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::Primitive(
    quartz::rendering::Primitive&& other
) :
    m_indexCount(other.m_indexCount),
    m_stagedVertexBuffer(std::move(other.m_stagedVertexBuffer)),
    m_stagedIndexBuffer(std::move(other.m_stagedIndexBuffer)),
    m_material(other.m_material)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::~Primitive() {
    LOG_FUNCTION_CALL_TRACEthis("");
}