#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf.h>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/buffer/StagedBuffer.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/model/Primitive.hpp"
#include "quartz/rendering/model/TangentCalculator.hpp"
#include "quartz/rendering/model/Vertex.hpp"
#include "quartz/rendering/texture/Texture.hpp"

bool
quartz::rendering::Primitive::handleMissingVertexAttribute(
    std::vector<quartz::rendering::Vertex>& verticesToPopulate,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<uint32_t>& indices,
    const quartz::rendering::Vertex::AttributeType attributeType
) {
    const std::string attributeNameString = quartz::rendering::Vertex::getAttributeNameString(attributeType);
    const std::string attributeGltfString = quartz::rendering::Vertex::getAttributeGLTFString(attributeType);

    if (gltfPrimitive.attributes.find(attributeGltfString) != gltfPrimitive.attributes.end()) {
        return false;
    }
    LOG_TRACE(MODEL_PRIMITIVE, "Primitive does not contain a {} attribute ({})", attributeNameString, attributeGltfString);

    switch (attributeType) {
        case quartz::rendering::Vertex::AttributeType::Position:
        case quartz::rendering::Vertex::AttributeType::Normal:
            LOG_ERROR(MODEL_PRIMITIVE, "Primitive must contain a {} attribute", attributeGltfString);

        case quartz::rendering::Vertex::AttributeType::Tangent:
            LOG_TRACE(MODEL_PRIMITIVE, "Manually calculating vertex tangents. We're operating under the assumption that the other attributes are already populated");
            quartz::rendering::TangentCalculator::populateVerticesWithTangents(gltfModel, gltfPrimitive, indices, verticesToPopulate);
            return true;

        case quartz::rendering::Vertex::AttributeType::Color:
            LOG_TRACE(MODEL_PRIMITIVE, "Using default vertex color of r: {}, g: {}, b: {}", verticesToPopulate[0].color.r, verticesToPopulate[0].color.g, verticesToPopulate[0].color.b);
            return true;

        case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
            LOG_TRACE(MODEL_PRIMITIVE, "Using default base color texture coordinates of {},{}", verticesToPopulate[0].baseColorTextureCoordinate.x, verticesToPopulate[0].baseColorTextureCoordinate.y);
            return true;
        case quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate:
            LOG_TRACE(MODEL_PRIMITIVE, "Using default metallic roughness texture coordinates of {},{}", verticesToPopulate[0].metallicRoughnessTextureCoordinate.x, verticesToPopulate[0].metallicRoughnessTextureCoordinate.y);
            return true;
        case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate:
            LOG_TRACE(MODEL_PRIMITIVE, "Using default normal texture coordinates of {},{}", verticesToPopulate[0].normalTextureCoordinate.x, verticesToPopulate[0].normalTextureCoordinate.y);
            return true;
        case quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate:
            LOG_TRACE(MODEL_PRIMITIVE, "Using default emission texture coordinates of {},{}", verticesToPopulate[0].emissionTextureCoordinate.x, verticesToPopulate[0].emissionTextureCoordinate.y);
            return true;
        case quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate:
            LOG_TRACE(MODEL_PRIMITIVE, "Using default occlusion texture coordinates of {},{}", verticesToPopulate[0].occlusionTextureCoordinate.x, verticesToPopulate[0].occlusionTextureCoordinate.y);
            return true;
    }
}

bool
quartz::rendering::Primitive::handleDefaultTextureAttribute(
    UNUSED std::vector<quartz::rendering::Vertex>& verticesToPopulate,
    const std::shared_ptr<quartz::rendering::Material>& p_material,
    const quartz::rendering::Vertex::AttributeType attributeType
) {
    switch (attributeType) {
        case quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate:
            if (p_material->getBaseColorTextureMasterIndex() == quartz::rendering::Texture::getBaseColorDefaultMasterIndex()) {
                LOG_TRACE(MODEL_PRIMITIVE, "Using default base color texture, so leaving coordinates to be {},{}", verticesToPopulate[0].baseColorTextureCoordinate.x, verticesToPopulate[0].baseColorTextureCoordinate.y);
                return true;
            }
            break;

        case quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate:
            if (p_material->getMetallicRoughnessTextureMasterIndex() == quartz::rendering::Texture::getMetallicRoughnessDefaultMasterIndex()) {
                LOG_TRACE(MODEL_PRIMITIVE, "Using default metallic roughness texture, so leaving coordinates to be {},{}", verticesToPopulate[0].metallicRoughnessTextureCoordinate.x, verticesToPopulate[0].metallicRoughnessTextureCoordinate.y);
                return true;
            }
            break;

        case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate:
            if (p_material->getNormalTextureMasterIndex() == quartz::rendering::Texture::getNormalDefaultMasterIndex()) {
                LOG_TRACE(MODEL_PRIMITIVE, "Using default normal texture, so leaving coordinates to be {},{}", verticesToPopulate[0].normalTextureCoordinate.x, verticesToPopulate[0].normalTextureCoordinate.y);
                return true;
            }
            break;

        case quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate:
            if (p_material->getEmissionTextureMasterIndex() == quartz::rendering::Texture::getEmissionDefaultMasterIndex()) {
                LOG_TRACE(MODEL_PRIMITIVE, "Using default emission texture, so leaving coordinates to be {},{}", verticesToPopulate[0].emissionTextureCoordinate.x, verticesToPopulate[0].emissionTextureCoordinate.y);
                return true;
            }
            break;

        case quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate:
            if (p_material->getOcclusionTextureMasterIndex() == quartz::rendering::Texture::getOcclusionDefaultMasterIndex()) {
                LOG_TRACE(MODEL_PRIMITIVE, "Using default occlusion texture, so leaving coordinates to be {},{}", verticesToPopulate[0].occlusionTextureCoordinate.x, verticesToPopulate[0].occlusionTextureCoordinate.y);
                return true;
            }
            break;

        default:
            break;
    }

    return false;
}

uint32_t
quartz::rendering::Primitive::determineGltfAccessorByteStride(
    const quartz::rendering::Vertex::AttributeType attributeType,
    const tinygltf::Accessor& accessor,
    const tinygltf::BufferView& bufferView
) {
    const std::string attributeNameString = quartz::rendering::Vertex::getAttributeNameString(attributeType);
    const std::string attributeGltfString = quartz::rendering::Vertex::getAttributeGLTFString(attributeType);

    uint32_t tinygltfVecType = TINYGLTF_TYPE_VEC3;
    if (
        attributeType == quartz::rendering::Vertex::AttributeType::BaseColorTextureCoordinate ||
        attributeType == quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate ||
        attributeType == quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate ||
        attributeType == quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate ||
        attributeType == quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate
    ) {
        LOG_TRACE(MODEL_PRIMITIVE, "{} attribute ({}) attribute uses vector2", attributeNameString, attributeGltfString);
        tinygltfVecType = TINYGLTF_TYPE_VEC2;
    }

    const uint32_t byteStride = accessor.ByteStride(bufferView) ?
        accessor.ByteStride(bufferView) / sizeof (float) :
        tinygltf::GetNumComponentsInType(tinygltfVecType);
    LOG_TRACE(MODEL_PRIMITIVE, "Using a byte stride of {}", byteStride);

    return byteStride;
}

uint32_t
quartz::rendering::Primitive::loadMaterialMasterIndex(
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<uint32_t>& materialMasterIndices
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "");

    int32_t materialLocalIndex = gltfPrimitive.material;
    LOG_TRACE(MODEL_PRIMITIVE, "Primitive uses material at local index {}", materialLocalIndex);

    if (materialLocalIndex < 0) {
        const uint32_t defaultMaterialIndex = quartz::rendering::Material::getDefaultMaterialMasterIndex();
        LOG_TRACE(MODEL_PRIMITIVE, "No material provided, so using default material at index {}, with {} materials created", defaultMaterialIndex, quartz::rendering::Material::getNumCreatedMaterials());
        return defaultMaterialIndex;
    }

    const uint32_t materialMasterIndex = materialMasterIndices[materialLocalIndex];
    LOG_TRACE(MODEL_PRIMITIVE, "Material at local index {} gives master material {}", materialLocalIndex, materialMasterIndex);

    return materialMasterIndex;
}

std::vector<uint32_t>
quartz::rendering::Primitive::loadIndicesFromGltfPrimitive(
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "");

    const uint32_t indexAccessorID = gltfPrimitive.indices; // will always be >= 0
    const tinygltf::Accessor& indexAccessor = gltfModel.accessors[indexAccessorID];
    const uint32_t indexCount = indexAccessor.count;

    const uint32_t indexBufferViewID = indexAccessor.bufferView;
    const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews[indexBufferViewID];

    const uint32_t indexBufferID = indexBufferView.buffer;
    const tinygltf::Buffer& indexBuffer = gltfModel.buffers[indexBufferID];
    const uint32_t indexAccessorByteOffset = indexAccessor.byteOffset;
    const uint32_t indexBufferViewByteOffset = indexBufferView.byteOffset;
    const std::vector<uint8_t>& indexBufferData = indexBuffer.data;
    const uint8_t* indexBufferDataStartAddress = indexBufferData.data();
    const uint8_t* desiredIndexDataStartAddress = indexBufferDataStartAddress + indexAccessorByteOffset + indexBufferViewByteOffset;

    std::vector<uint32_t> indices(indexCount);

    switch(indexAccessor.componentType) {
        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
            LOG_TRACE(MODEL_PRIMITIVE, "Using {} indices of type uint32_t", indexCount);
            const uint32_t* p_gltfIndices = reinterpret_cast<const uint32_t*>(desiredIndexDataStartAddress);

            for (uint32_t i = 0; i < indexCount; ++i) {
                indices[i] = p_gltfIndices[i];
            }

            break;
        }

        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
            LOG_TRACE(MODEL_PRIMITIVE, "Using {} indices of type uint16_t", indexCount);
            const uint16_t* p_gltfIndices = reinterpret_cast<const uint16_t*>(desiredIndexDataStartAddress);

            for (uint32_t i = 0; i < indexCount; ++i) {
                indices[i] = p_gltfIndices[i];
            }

            break;
        }

        case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
            LOG_TRACE(MODEL_PRIMITIVE, "Using {} indices of type uint8_t", indexCount);
            const uint8_t* p_gltfIndices = reinterpret_cast<const uint8_t*>(desiredIndexDataStartAddress);

            for (uint32_t i = 0; i < indexCount; ++i) {
                indices[i] = p_gltfIndices[i];
            }

            break;
        }
    }

    LOG_TRACE(MODEL_PRIMITIVE, "Successfully loaded {} indices for primitive", indices.size());
    return indices;
}

void
quartz::rendering::Primitive::populateVerticesWithAttribute(
    std::vector<quartz::rendering::Vertex>& verticesToPopulate,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::shared_ptr<quartz::rendering::Material>& p_material,
    const std::vector<uint32_t>& indices,
    const quartz::rendering::Vertex::AttributeType attributeType
) {
    const std::string attributeNameString = quartz::rendering::Vertex::getAttributeNameString(attributeType);
    const std::string attributeGltfString = quartz::rendering::Vertex::getAttributeGLTFString(attributeType);
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "{} ({})", attributeNameString, attributeGltfString);

    if (quartz::rendering::Primitive::handleMissingVertexAttribute(verticesToPopulate, gltfModel, gltfPrimitive, indices, attributeType)) {
        return;
    }

    if (quartz::rendering::Primitive::handleDefaultTextureAttribute(verticesToPopulate, p_material, attributeType)) {
        return;
    }

    LOG_TRACE(MODEL_PRIMITIVE, "Loading {} attribute ({})", attributeNameString, attributeGltfString);

    const uint32_t accessorIndex = gltfPrimitive.attributes.find(attributeGltfString)->second;
    const tinygltf::Accessor& accessor = gltfModel.accessors[accessorIndex];

    const uint32_t bufferViewIndex = accessor.bufferView;
    const tinygltf::BufferView& bufferView = gltfModel.bufferViews[bufferViewIndex];

    const uint32_t bufferIndex = bufferView.buffer;
    const tinygltf::Buffer& buffer = gltfModel.buffers[bufferIndex];

    const uint32_t accessorByteOffset = accessor.byteOffset;
    const uint32_t bufferViewByteOffset = bufferView.byteOffset;
    const std::vector<uint8_t>& bufferData = buffer.data;
    const uint8_t* bufferDataStartAddress = bufferData.data();
    const uint8_t* desiredDataStartAddress = bufferDataStartAddress + accessorByteOffset + bufferViewByteOffset;

    const float* p_data = reinterpret_cast<const float*>(desiredDataStartAddress);

    const uint32_t byteStride = quartz::rendering::Primitive::determineGltfAccessorByteStride(attributeType, accessor, bufferView);

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
            case quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate: {
                verticesToPopulate[i].metallicRoughnessTextureCoordinate = glm::make_vec2(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate: {
                verticesToPopulate[i].normalTextureCoordinate = glm::make_vec2(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate: {
                verticesToPopulate[i].emissionTextureCoordinate = glm::make_vec2(&p_data[i * byteStride]);
                break;
            }
            case quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate: {
                verticesToPopulate[i].occlusionTextureCoordinate = glm::make_vec2(&p_data[i * byteStride]);
                break;
            }
        }
    }
}

quartz::rendering::StagedBuffer
quartz::rendering::Primitive::createStagedVertexBuffer(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::shared_ptr<quartz::rendering::Material>& p_material,
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
        quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate,
        quartz::rendering::Vertex::AttributeType::MetallicRoughnessTextureCoordinate,
        quartz::rendering::Vertex::AttributeType::EmissionTextureCoordinate,
        quartz::rendering::Vertex::AttributeType::OcclusionTextureCoordinate,
        quartz::rendering::Vertex::AttributeType::Tangent, // needs to go last. uses other attributes in calculations if not provided by model
    };

    for (const quartz::rendering::Vertex::AttributeType attributeType : attributeTypes) {
        quartz::rendering::Primitive::populateVerticesWithAttribute(
            vertices,
            gltfModel,
            gltfPrimitive,
            p_material,
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

    LOG_INFO(MODEL_PRIMITIVE, "Successfully created staged vertex buffer for {} vertices", vertexCount);

    return stagedVertexBuffer;
}

quartz::rendering::Primitive::Primitive(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<uint32_t>& materialMasterIndices
) :
    m_materialMasterIndex(
        quartz::rendering::Primitive::loadMaterialMasterIndex(
            gltfPrimitive,
            materialMasterIndices
        )
    ),
    m_indices(
        quartz::rendering::Primitive::loadIndicesFromGltfPrimitive(
            gltfModel,
            gltfPrimitive
        )
    ),
    m_stagedVertexBuffer(
        quartz::rendering::Primitive::createStagedVertexBuffer(
            renderingDevice,
            gltfModel,
            gltfPrimitive,
            quartz::rendering::Material::getMaterialPtr(m_materialMasterIndex),
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
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::Primitive(
    quartz::rendering::Primitive&& other
) :
    m_materialMasterIndex(other.m_materialMasterIndex),
    m_indices(std::move(other.m_indices)),
    m_stagedVertexBuffer(std::move(other.m_stagedVertexBuffer)),
    m_stagedIndexBuffer(std::move(other.m_stagedIndexBuffer))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::~Primitive() {
    LOG_FUNCTION_CALL_TRACEthis("");
}
