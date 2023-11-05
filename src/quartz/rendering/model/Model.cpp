#define TINY0BJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "quartz/rendering/model/Model.hpp"

bool
quartz::rendering::Model::loadModel(
    const std::string& filepath,
    tinyobj::attrib_t& tinyobjAttribute,
    std::vector<tinyobj::shape_t>& tinyobjShapes,
    std::vector<tinyobj::material_t>& tinyobjMaterials
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "{}", filepath);
    // Attributes
    //   contains:
    //    - positions
    //    - normals
    //    - texture coordinates
    //
    // Shapes
    //   contains:
    //    - seperate objects and their faces
    //      - Each face consists of an array of vertices
    //        - Each vertex contains indices of position, normal,
    //          and texture coord attributes
    // Face
    //   contains:
    //    - array of vertices
    //
    // Vertex:
    //   contains:
    //    - position index
    //    - normal index
    //    - texture coordinate index

    std::string warningString;
    std::string errorString;

    // Triangulation feature enabled by default, so we can rest assured
    // that every face is going to be a triangle
    if(!tinyobj::LoadObj(
        &tinyobjAttribute,
        &tinyobjShapes,
        &tinyobjMaterials,
        &warningString,
        &errorString,
        filepath.c_str()
    )) {
        LOG_CRITICAL(MODEL, "Failed to load model at {}", filepath);
        LOG_CRITICAL(MODEL, "  - Warning : {}", warningString);
        LOG_CRITICAL(MODEL, "  - Error   : {}", errorString);
        throw std::runtime_error("");
    }

    if (!warningString.empty()) {
        LOG_WARNING(MODEL, "tinyobj::load warning : {}", warningString);
    }

    if (!errorString.empty()) {
        LOG_ERROR(MODEL, "tinyobj::load error : {}", errorString);
    }

    uint32_t totalShapeIndices = 0;
    for (const tinyobj::shape_t& shape : tinyobjShapes) {
        totalShapeIndices += shape.mesh.indices.size();
    }

    LOG_DEBUG(MODEL, "Loaded object at {}", filepath);
    LOG_DEBUG(MODEL, "  - {} attribute vertices", tinyobjAttribute.vertices.size());
    LOG_DEBUG(MODEL, "  - {} attribute normals", tinyobjAttribute.normals.size());
    LOG_DEBUG(MODEL, "  - {} attribute texture coordinates", tinyobjAttribute.texcoords.size());
    LOG_DEBUG(MODEL, "  - {} shapes", tinyobjShapes.size());
    LOG_DEBUG(MODEL, "    - {} total indices", totalShapeIndices);

    return true;
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    UNUSED const std::string& objectFilePath,
    const std::string& textureFilepath
) :
    m_tinyobjAttribute(),
    m_tinyobjShapes(),
    m_tinyobjMaterials(),
    m_loadedSuccessfully(
        quartz::rendering::Model::loadModel(
            objectFilePath,
            m_tinyobjAttribute,
            m_tinyobjShapes,
            m_tinyobjMaterials
        )
    ),
    m_mesh(
        renderingDevice,
        m_tinyobjAttribute,
        m_tinyobjShapes
    ),
    m_texture(renderingDevice, textureFilepath)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_tinyobjAttribute(other.m_tinyobjAttribute),
    m_tinyobjShapes(other.m_tinyobjShapes),
    m_tinyobjMaterials(other.m_tinyobjMaterials),
    m_loadedSuccessfully(other.m_loadedSuccessfully),
    m_mesh(std::move(other.m_mesh)),
    m_texture(std::move(other.m_texture))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}