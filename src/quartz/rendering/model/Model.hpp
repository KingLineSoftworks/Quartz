#pragma once

#include <queue>
#include <vector>

#include <tiny_gltf.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/model/Scene.hpp"
#include "quartz/rendering/texture/Texture.hpp"

namespace quartz {
namespace rendering {
    class Model;
}
}

/**
 * @brief 2023/12/1 GLTF NOTES
 *   These notes were taken when reading the gltf 2.0 spec (found at
 * https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#concepts-general).
 *
 * @brief RESPONSIBILITIES
 *   The model is responsible for the following things:
 *       scenes
 *       buffers
 *       accessors
 *
 * @brief COORDINATE SYSTEMS
 *   Gltf uses a RIGHT-HANDED coordinate system. ( up = +y , forward = +z ,
 * right = -x).
 *
 * @brief SCENES
 *   To us, the "scene" represents the highest level of things in the Model.
 * Thusly, the model can have 0 or more scenes. The scene specified as the base
 * scene should be the one displayed.
 *   Each scene may contain nodes.
 *
 * @brief NODES
 *   Every node is either a root node or has only 1 parent.
 *   Each node can contain 0 or more children.
 *   Each node may have transform properties.
 *   Each node may contain a mesh.
 *
 * @brief TRANSFORMATIONS
 *   May be transformation matrix or translation, scale, rotation (TRS) properties.
 *   TRS properties must be converted to matrices and postmultiplied in the order of
 * T * R * S.
 *   The transformation matrix of a node is given by the product of its parent's
 * transformation matrix and it's own transformation matrix.
 *
 * @brief BUFFERS & BUFFER VIEWS
 *   Buffers represent arbitrary data stored as a binary blob in little endian.
 *   Buffers must contain only one type of data. (You cannot use a buffer for both
 * vertex and index data).
 *   Buffer views represent contiguous segment of data in a buffer.
 *   Buffer views for vertex data may have a byteStride property defining the stride
 * in bytes between each vertex.
 *   Buffers and buffer views simply store and define the raw data for retrieval.
 *
 * @brief ACCESSORS (STANDARD & SPARSE)
 *   Accessors define a method for retrieving data as typed arrays from buffer views.
 *   Accessors for vertex attributes require the byteStride attribute to know how
 * to step through the array. All other accessors's data elements are tightly packed.
 *
 * @brief MESHES & PRIMITIVES
 *   Meshes are arrays of primitives.
 *   Primitives are the data required for GPU draw calls.
 *   Primitives specify vertex attributes and indices.
 *   Primitives may contain a material. If they do not then the default material
 * is used.
 *
 */

class quartz::rendering::Model {
public: // member functions
    Model(
        const quartz::rendering::Device& renderingDevice,
        const std::string& objectFilepath
    );
    Model(Model&& other);
    ~Model();

    USE_LOGGER(MODEL);

    const quartz::rendering::Scene& getDefaultScene() const { return m_scenes[m_defaultSceneIndex]; }

private: // static functions
    static tinygltf::Model loadGLTFModel(const std::string& filepath);
    static std::vector<uint32_t> loadTextures(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel
    );
    static uint32_t getMasterTextureIndexFromLocalIndex(
        const std::vector<uint32_t>& masterIndices,
        const int32_t localIndex,
        const quartz::rendering::Texture::Type textureType
    );
    static uint32_t getTextureMasterIndex(
        const tinygltf::Material& gltfMaterial,
        const std::vector<uint32_t>& masterIndices,
        const quartz::rendering::Texture::Type textureType
    );
    static std::vector<quartz::rendering::Material> loadMaterials(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel
    );
    static std::vector<quartz::rendering::Scene> loadScenes(
        const quartz::rendering::Device& renderingDevice,
        const tinygltf::Model& gltfModel,
        const std::vector<quartz::rendering::Material>& materials
    );

private: // member variables
    const tinygltf::Model m_gltfModel;

    std::vector<quartz::rendering::Material> m_materials;

    uint32_t m_defaultSceneIndex;
    std::vector<quartz::rendering::Scene> m_scenes;
};