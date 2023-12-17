#pragma once

#include <cstdint>
#include <vector>

#include <mikktspace.h>

#include "quartz/rendering/model/Vertex.hpp"

namespace quartz {
namespace rendering {
    class TangentCalculator;
}
}

class quartz::rendering::TangentCalculator {
public: // static functions

    static void populateVerticesWithTangents(
        std::vector<quartz::rendering::Vertex>& verticesToPopulate
    );

    /**
     * @brief Callbacks for MikkTSpace to use
     */

    static int32_t getVertexIndex(
        const SMikkTSpaceContext* p_mikktspaceContext,
        int32_t faceIndex,
        int32_t vertexIndex
    );
    static int32_t getNumFaces(
        const SMikkTSpaceContext* p_mikktspaceContext
    );
    static int32_t getNumVerticesOfFace(
        const SMikkTSpaceContext* p_mikktspaceContext,
        int32_t faceIndex
    );
    static void getPosition(
        const SMikkTSpaceContext* p_mikktspaceContext,
        float outpos[],
        int32_t faceIndex,
        int32_t vertexIndex
    );
    static void getNormal(
        const SMikkTSpaceContext* p_mikktspaceContext,
        float outnormal[],
        int32_t faceIndex,
        int32_t vertexIndex
    );
    static void getTextureCoordinate(
        const SMikkTSpaceContext* p_mikktspaceContext,
        float outuv[],
        int32_t faceIndex,
        int32_t vertexIndex
    );
    static void setTSpaceBasic(
        const SMikkTSpaceContext *context,
        const float tangentu[],
        float fSign,
        int32_t faceIndex,
        int32_t vertexIndex
    );

public: // member functions
    TangentCalculator() = delete;
};