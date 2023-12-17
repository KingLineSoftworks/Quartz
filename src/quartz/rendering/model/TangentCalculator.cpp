#include <mikktspace.h>

#include "util/macros.hpp"

#include "quartz/rendering/model/TangentCalculator.hpp"

void
quartz::rendering::TangentCalculator::populateVerticesWithTangents(
    UNUSED std::vector<quartz::rendering::Vertex>& verticesToPopulate
) {

}

int32_t
quartz::rendering::TangentCalculator::getVertexIndex(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext,
    UNUSED int32_t faceIndex,
    UNUSED int32_t vertexIndex
) {
    return 0;
}

int32_t
quartz::rendering::TangentCalculator::getNumFaces(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext
) {
    return 0;
}

int32_t
quartz::rendering::TangentCalculator::getNumVerticesOfFace(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext,
    UNUSED int32_t faceIndex
) {
    return 0;
}

void
quartz::rendering::TangentCalculator::getPosition(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext,
    UNUSED float outpos[],
    UNUSED int32_t faceIndex,
    UNUSED int32_t vertexIndex
) {

}

void
quartz::rendering::TangentCalculator::getNormal(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext,
    UNUSED float outnormal[],
    UNUSED int32_t faceIndex,
    UNUSED int32_t vertexIndex
) {

}

void
quartz::rendering::TangentCalculator::getTextureCoordinate(
    UNUSED const SMikkTSpaceContext* p_mikktspaceContext,
    UNUSED float outuv[],
    UNUSED int32_t faceIndex,
    UNUSED int32_t vertexIndex
) {

}

void
quartz::rendering::TangentCalculator::setTSpaceBasic(
    UNUSED const SMikkTSpaceContext *context,
    UNUSED const float tangentu[],
    UNUSED float fSign,
    UNUSED int32_t faceIndex,
    UNUSED int32_t vertexIndex
) {

}
