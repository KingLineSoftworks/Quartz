#pragma once

#include <cstdint>
#include <vector>

#include <mikktspace.h>

#include <tiny_gltf.h>

#include "math/transform/Vec3.hpp"

#include "quartz/rendering/model/Vertex.hpp"

namespace quartz {
namespace rendering {
    class TangentCalculator;
}
}

class quartz::rendering::TangentCalculator {
public: // classes and enums
    struct Information {
        Information(
            const tinygltf::Model* p_gltfModel_,
            const tinygltf::Primitive* p_gltfPrimitive_,
            const uint32_t* p_indices_,
            const uint32_t indexCount_,
            quartz::rendering::Vertex* p_verticesToPopulate_,
            const uint32_t vertexCount_
        );

        const tinygltf::Model* p_gltfModel;
        const tinygltf::Primitive* p_gltfPrimitive;
        const uint32_t* p_indices;
        const uint32_t indexCount;
        quartz::rendering::Vertex* p_verticesToPopulate;
        const uint32_t vertexCount;
    };

public: // static functions

    static void populateVerticesWithTangents(
        const tinygltf::Model& gltfModel,
        const tinygltf::Primitive& gltfPrimitive,
        const std::vector<uint32_t>& indices,
        std::vector<quartz::rendering::Vertex>& verticesToPopulate
    );

    /**
     * @brief Helper functions
     */

    static uint32_t getVertexIndex(
        const SMikkTSpaceContext* p_mikktspaceContext,
        int32_t faceIndex,
        int32_t faceLocalVertexIndex
    );
    static math::Vec3 getVertexAttribute(
        const SMikkTSpaceContext* p_mikktspaceContext,
        const int32_t faceIndex,
        const int32_t faceLocalVertexIndex,
        const quartz::rendering::Vertex::AttributeType type
    );

    /**
     * @brief Callbacks for MikkTSpace to use
     */

    static int32_t getNumFaces(
        const SMikkTSpaceContext* p_mikktspaceContext
    );
    static int32_t getNumVerticesOfFace(
        const SMikkTSpaceContext* p_mikktspaceContext,
        int32_t faceIndex
    );
    static void getPosition(
        const SMikkTSpaceContext* p_mikktspaceContext,
        float positionToPopulate3[],
        int32_t faceIndex,
        int32_t faceLocalVertexIndex
    );
    static void getNormal(
        const SMikkTSpaceContext* p_mikktspaceContext,
        float normalToPopulate3[],
        int32_t faceIndex,
        int32_t faceLocalVertexIndex
    );
    static void getTextureCoordinate(
        const SMikkTSpaceContext* p_mikktspaceContext,
        float textureCoordinateToPopulate2[],
        int32_t faceIndex,
        int32_t faceLocalVertexIndex
    );
    static void setTangentSpaceBasic(
        const SMikkTSpaceContext* p_mikktspaceContext,
        const float populatedTangent3[],
        float fSign,
        int32_t faceIndex,
        int32_t faceLocalVertexIndex
    );

public: // member functions
    TangentCalculator() = delete;
};