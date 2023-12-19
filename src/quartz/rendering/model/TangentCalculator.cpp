#include <glm/vec3.hpp>

#include <mikktspace.h>

#include "util/macros.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/model/TangentCalculator.hpp"

quartz::rendering::TangentCalculator::Information::Information(
    const tinygltf::Model* p_gltfModel_,
    const tinygltf::Primitive* p_gltfPrimitive_,
    const uint32_t* p_indices_,
    const uint32_t indexCount_,
    quartz::rendering::Vertex* p_verticesToPopulate_,
    const uint32_t vertexCount_
) :
    p_gltfModel(p_gltfModel_),
    p_gltfPrimitive(p_gltfPrimitive_),
    p_indices(p_indices_),
    indexCount(indexCount_),
    p_verticesToPopulate(p_verticesToPopulate_),
    vertexCount(vertexCount_)
{}

void
quartz::rendering::TangentCalculator::populateVerticesWithTangents(
    const tinygltf::Model& gltfModel,
    const tinygltf::Primitive& gltfPrimitive,
    const std::vector<uint32_t>& indices,
    std::vector<quartz::rendering::Vertex>& verticesToPopulate
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL_PRIMITIVE, "");

    SMikkTSpaceInterface mikktspaceInterface;
    mikktspaceInterface.m_getNumFaces = quartz::rendering::TangentCalculator::getNumFaces;
    mikktspaceInterface.m_getNumVerticesOfFace = quartz::rendering::TangentCalculator::getNumVerticesOfFace;
    mikktspaceInterface.m_getPosition = quartz::rendering::TangentCalculator::getPosition;
    mikktspaceInterface.m_getNormal = quartz::rendering::TangentCalculator::getNormal;
    mikktspaceInterface.m_getTexCoord = quartz::rendering::TangentCalculator::getTextureCoordinate;
    mikktspaceInterface.m_setTSpace = nullptr;
    mikktspaceInterface.m_setTSpaceBasic = quartz::rendering::TangentCalculator::setTangentSpaceBasic;

    quartz::rendering::TangentCalculator::Information information(
        &gltfModel,
        &gltfPrimitive,
        indices.data(),
        indices.size(),
        verticesToPopulate.data(),
        verticesToPopulate.size()
    );

    SMikkTSpaceContext mikktspaceContext;
    mikktspaceContext.m_pInterface = &mikktspaceInterface;
    mikktspaceContext.m_pUserData = &information;

    LOG_TRACE(MODEL_PRIMITIVE, "Got {} faces (from {} indices)", information.indexCount / 3, information.indexCount);
    LOG_TRACE(MODEL_PRIMITIVE, "Got {} vertices", information.vertexCount);

    genTangSpaceDefault(&mikktspaceContext);
}

uint32_t
quartz::rendering::TangentCalculator::getVertexIndex(
    const SMikkTSpaceContext* p_mikktspaceContext,
    int32_t faceIndex,
    int32_t faceLocalVertexIndex
) {
    const quartz::rendering::TangentCalculator::Information* p_information =
        static_cast<quartz::rendering::TangentCalculator::Information*>(p_mikktspaceContext->m_pUserData);

    const uint32_t verticesPerFace = quartz::rendering::TangentCalculator::getNumVerticesOfFace(p_mikktspaceContext, faceIndex);

    const uint32_t indicesIndex = (faceIndex * verticesPerFace) + faceLocalVertexIndex;

    if (indicesIndex >= p_information->indexCount) {
        LOG_ERROR(MODEL_PRIMITIVE, "  Attempting to get master index at index {} when we have only {} indices", indicesIndex, p_information->indexCount);
        return 0;
    }

    const uint32_t masterIndex = p_information->p_indices[indicesIndex];

    return masterIndex;
}

glm::vec3
quartz::rendering::TangentCalculator::getVertexAttribute(
    const SMikkTSpaceContext* p_mikktspaceContext,
    const int32_t faceIndex,
    const int32_t faceLocalVertexIndex,
    const quartz::rendering::Vertex::AttributeType type
) {
    const quartz::rendering::TangentCalculator::Information* p_information =
        static_cast<quartz::rendering::TangentCalculator::Information*>(p_mikktspaceContext->m_pUserData);

    const uint32_t masterVertexIndex = quartz::rendering::TangentCalculator::getVertexIndex(
        p_mikktspaceContext,
        faceIndex,
        faceLocalVertexIndex
    );

    if (masterVertexIndex >= p_information->vertexCount) {
        LOG_ERROR(MODEL_PRIMITIVE, "  Attempting to get vertex at position {} when we have only {} vertices", masterVertexIndex, p_information->vertexCount);
        return {0.0f, 0.0f, 0.0f};
    }

    const quartz::rendering::Vertex& vertex = p_information->p_verticesToPopulate[masterVertexIndex];

    switch (type) {
        case quartz::rendering::Vertex::AttributeType::Position:
            return vertex.position;
        case quartz::rendering::Vertex::AttributeType::Normal:
            return vertex.normal;
        case quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate:
            return {vertex.normalTextureCoordinate.x, vertex.normalTextureCoordinate.y, 0.0f};
        default:
            LOG_ERROR(MODEL_PRIMITIVE, "  Not getting vertex attribute {}", quartz::rendering::Vertex::getAttributeGLTFString(type));
            return {0.0f, 0.0f, 0.0f};
    }
}

int32_t
quartz::rendering::TangentCalculator::getNumFaces(
    const SMikkTSpaceContext* p_mikktspaceContext
) {
    const quartz::rendering::TangentCalculator::Information* p_information =
        static_cast<quartz::rendering::TangentCalculator::Information*>(p_mikktspaceContext->m_pUserData);

    const uint32_t indexCount = p_information->p_gltfModel->accessors[p_information->p_gltfPrimitive->indices].count;

    const uint32_t faceCount = indexCount / 3;

    return faceCount;
}

int32_t
quartz::rendering::TangentCalculator::getNumVerticesOfFace(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext,
    UNUSED int32_t faceIndex
) {
    return 3;
}

void
quartz::rendering::TangentCalculator::getPosition(
    const SMikkTSpaceContext* p_mikktspaceContext,
    float positionToPopulate3[],
    int32_t faceIndex,
    int32_t faceLocalVertexIndex
) {
    const glm::vec3 vertexAttribute = quartz::rendering::TangentCalculator::getVertexAttribute(
        p_mikktspaceContext,
        faceIndex,
        faceLocalVertexIndex,
        quartz::rendering::Vertex::AttributeType::Position
    );

    positionToPopulate3[0] = vertexAttribute.x;
    positionToPopulate3[1] = vertexAttribute.y;
    positionToPopulate3[2] = vertexAttribute.z;
}

void
quartz::rendering::TangentCalculator::getNormal(
    const SMikkTSpaceContext* p_mikktspaceContext,
    float normalToPopulate3[],
    int32_t faceIndex,
    int32_t faceLocalVertexIndex
) {
    const glm::vec3 vertexAttribute = quartz::rendering::TangentCalculator::getVertexAttribute(
        p_mikktspaceContext,
        faceIndex,
        faceLocalVertexIndex,
        quartz::rendering::Vertex::AttributeType::Normal
    );

    normalToPopulate3[0] = vertexAttribute.x;
    normalToPopulate3[1] = vertexAttribute.y;
    normalToPopulate3[2] = vertexAttribute.z;
}

void
quartz::rendering::TangentCalculator::getTextureCoordinate(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext,
    UNUSED float textureCoordinateToPopulate2[],
    UNUSED int32_t faceIndex,
    UNUSED int32_t faceLocalVertexIndex
) {
    const glm::vec3 vertexAttribute = quartz::rendering::TangentCalculator::getVertexAttribute(
        p_mikktspaceContext,
        faceIndex,
        faceLocalVertexIndex,
        quartz::rendering::Vertex::AttributeType::NormalTextureCoordinate
    );

    textureCoordinateToPopulate2[0] = vertexAttribute.x;
    textureCoordinateToPopulate2[1] = vertexAttribute.y;
}

void
quartz::rendering::TangentCalculator::setTangentSpaceBasic(
    const SMikkTSpaceContext *p_mikktspaceContext,
    const float populatedTangent3[],
    UNUSED float fSign,
    int32_t faceIndex,
    int32_t faceLocalVertexIndex
) {
    quartz::rendering::TangentCalculator::Information* p_information =
        static_cast<quartz::rendering::TangentCalculator::Information*>(p_mikktspaceContext->m_pUserData);

    const uint32_t masterVertexIndex = quartz::rendering::TangentCalculator::getVertexIndex(
        p_mikktspaceContext,
        faceIndex,
        faceLocalVertexIndex
    );

    if (masterVertexIndex >= p_information->vertexCount) {
        LOG_ERROR(MODEL_PRIMITIVE, "  Attempting to get vertex at position {} when we have only {} vertices", masterVertexIndex, p_information->vertexCount);
        return;
    }

    quartz::rendering::Vertex& vertex = p_information->p_verticesToPopulate[masterVertexIndex];

    /**
     * @brief We are ignoring the fSign value for now. It seems that it might be needed for calculating
     *   bitangent vectors using the equation `bitangent = fSign * cross(normal, tangent)` but we are
     *   handling this in the shader and we might not actually need this value to do so? Maybe we are
     *   okay without this because we are normalizing the tangent with respect to the normal in the
     *   vertex shader?
     *
     * @todo 2023/12/17 Look into if we need to (or should) be passing fsign with the tangent to the
     *   vertex shader for calculation of the bitangent. Maybe calculating `bitangent = fSign * cross(normal, tangent)`
     *   is faster than doing it the way we are doing currently? But this will require an extra float (4 bytes)
     *   to be passed to the vertex shader.
     */

    vertex.tangent.x = populatedTangent3[0];
    vertex.tangent.y = populatedTangent3[1];
    vertex.tangent.z = populatedTangent3[2];
//    vertex.tangent.w = fSign;
}
