#include <string>

#include <tiny_gltf.h>

#define TINY0BJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/model/Model.hpp"

void
quartz::rendering::Model::loadGLTFModel(
    const quartz::rendering::Device& renderingDevice
){
    const std::string filepath = "/Users/keegankochis/Development/Quartz/vendor/tinygltf/models/Cube/Cube.gltf";
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "{}", filepath);

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model gltfModel;

    std::string warningString;
    std::string errorString;

    const bool isBinaryFile =
        util::FileSystem::getFileExtension(filepath) ==
        "glb";

    LOG_TRACE(MODEL, "Using {} gltf file at {}", isBinaryFile ? "binary" : "ascii", filepath);

    const bool fileLoadedSuccessfully = isBinaryFile ?
        gltfContext.LoadBinaryFromFile(
            &gltfModel,
            &errorString,
            &warningString,
            filepath.c_str()
        ) :
        gltfContext.LoadASCIIFromFile(
            &gltfModel,
            &errorString,
            &warningString,
            filepath.c_str()
        );

    if (!fileLoadedSuccessfully) {
        LOG_CRITICAL(MODEL, "Failed to load model at {}", filepath);
        LOG_CRITICAL(MODEL, "  - Warning : {}", warningString);
        LOG_CRITICAL(MODEL, "  - Error   : {}", errorString);
        throw std::runtime_error("");
    }

    if (!warningString.empty()) {
        LOG_WARNING(
            MODEL, "TinyGLTF::Load{}FromFile warning : {}",
            isBinaryFile ? "Binary" : "ASCII", warningString
        );
    }

    if (!errorString.empty()) {
        LOG_ERROR(
            MODEL, "TinyGLTF::Load{}FromFile error : {}",
            isBinaryFile ? "Binary" : "ASCII", errorString
        );
    }

    // ----- textures and their samplers ----- //

    LOG_TRACE(MODEL, "Got {} texture samplers from gltf model", gltfModel.samplers.size());
    LOG_TRACE(MODEL, "Got {} textures from gltf model", gltfModel.textures.size());

    for (uint32_t i = 0; i < gltfModel.textures.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        const tinygltf::Texture& gltfTexture = gltfModel.textures[i];

        LOG_TRACE(MODEL, "Creating texture {} with name \"{}\"", i, gltfTexture.name);

        tinygltf::Sampler gltfSampler;
        const int32_t samplerIndex = gltfTexture.sampler;
        if (samplerIndex == -1) {
            LOG_WARNING(MODEL, "No sampler specified. Using default one");
            gltfSampler.minFilter = -1;
            gltfSampler.magFilter = -1;
            gltfSampler.wrapS = -1;
            gltfSampler.wrapT = -1;
            gltfSampler.wrapT = -1;
        } else {
            gltfSampler = gltfModel.samplers[samplerIndex];
            LOG_TRACE(MODEL, "Using sampler {} with name \"{}\"", samplerIndex, gltfSampler.name);
        }

        const int32_t imageIndex = gltfTexture.source;
        const tinygltf::Image& gltfImage = gltfModel.images[imageIndex];
        LOG_TRACE(MODEL, "Using image {} with name \"{}\"", imageIndex, gltfImage.name);

        UNUSED quartz::rendering::Texture texture(
            renderingDevice,
            gltfImage,
            gltfSampler
        );
    }

    // ---- materials ----- //
}

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

    LOG_DEBUG(MODEL, "Loaded model at {}", filepath);
    LOG_DEBUG(MODEL, "  - {} attribute vertices", tinyobjAttribute.vertices.size());
    LOG_DEBUG(MODEL, "  - {} attribute normals", tinyobjAttribute.normals.size());
    LOG_DEBUG(MODEL, "  - {} attribute texture coordinates", tinyobjAttribute.texcoords.size());
    LOG_DEBUG(MODEL, "  - {} shapes", tinyobjShapes.size());
    LOG_DEBUG(MODEL, "    - {} total indices", totalShapeIndices);
    LOG_DEBUG(MODEL, "  - {} materials", tinyobjMaterials.size());

    return true;
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    UNUSED const std::string& objectFilepath,
    const std::string& textureFilepath
) :
    m_tinyobjAttribute(),
    m_tinyobjShapes(),
    m_tinyobjMaterials(),
    m_loadedSuccessfully(
        quartz::rendering::Model::loadModel(
            objectFilepath,
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
    quartz::rendering::Model::loadGLTFModel(renderingDevice);
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