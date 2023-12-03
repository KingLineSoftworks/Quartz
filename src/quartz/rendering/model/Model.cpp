#include <string>
#include <queue>

#include <glm/vec3.hpp>

#include <tiny_gltf.h>

#include "util/file_system/FileSystem.hpp"

#include "quartz/rendering/model/Model.hpp"

tinygltf::Model
quartz::rendering::Model::loadGLTFModel(
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "{}", filepath);

    tinygltf::TinyGLTF gltfContext;
    tinygltf::Model gltfModel;

    std::string warningString;
    std::string errorString;

    const bool isBinaryFile =
        util::FileSystem::getFileExtension(filepath) ==
        "glb";

    LOG_TRACE(MODEL, "Using {} gltf file at {}", isBinaryFile ? "binary" : "ascii", filepath);

    const bool fileLoadedSuccessfully =
        isBinaryFile ?
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

    return gltfModel;
}

std::vector<quartz::rendering::Scene>
quartz::rendering::Model::loadScenes(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<quartz::rendering::Scene> scenes;
    scenes.reserve(gltfModel.scenes.size());

    for (uint32_t i = 0; i < gltfModel.scenes.size(); ++i) {
        LOG_TRACE(MODEL, "Loading scene {}", i);

        const tinygltf::Scene& gltfScene = gltfModel.scenes[i];

        scenes.emplace_back(
            renderingDevice,
            gltfModel,
            gltfScene
        );
    }

    return scenes;
}

std::vector<uint32_t>
quartz::rendering::Model::loadTextures(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    LOG_TRACE(MODEL, "Got {} texture samplers from gltf model", gltfModel.samplers.size());
    LOG_TRACE(MODEL, "Got {} textures from gltf model", gltfModel.textures.size());

    quartz::rendering::Texture::initializeMasterList(renderingDevice);

    std::vector<uint32_t> masterIndices;

    for (uint32_t i = 0; i < gltfModel.textures.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        const tinygltf::Texture& gltfTexture = gltfModel.textures[i];

        LOG_TRACE(MODEL, "Creating texture {} with name \"{}\"", i, gltfTexture.name);

        tinygltf::Sampler gltfSampler;
        const int32_t samplerIndex = gltfTexture.sampler;
        if (samplerIndex == -1) {
            LOG_WARNING(MODEL, "No gltf sampler specified. Using default one");
            gltfSampler.minFilter = -1;
            gltfSampler.magFilter = -1;
            gltfSampler.wrapS = -1;
            gltfSampler.wrapT = -1;
            gltfSampler.wrapT = -1;
        } else {
            gltfSampler = gltfModel.samplers[samplerIndex];
            LOG_TRACE(MODEL, "Using gltf sampler {} with name \"{}\"", samplerIndex, gltfSampler.name);
        }

        const int32_t imageIndex = gltfTexture.source;
        const tinygltf::Image& gltfImage = gltfModel.images[imageIndex];
        LOG_TRACE(MODEL, "Using gltf image {} with name \"{}\"", imageIndex, gltfImage.name);

        /**
         * @todo 2023/11/17 We might be able to determine the type of texture we are creating based
         *   on the information in gltfImage and in gltfSampler. We might not be able to store all of
         *   these textures in 1 master list due to their differences in structure.
         */

        masterIndices.emplace_back(quartz::rendering::Texture::createTexture(
            renderingDevice,
            gltfImage,
            gltfSampler
        ));
    }

    return masterIndices;
}

quartz::rendering::Material
quartz::rendering::Model::loadMaterial(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<uint32_t> masterIndices =
        quartz::rendering::Model::loadTextures(
            renderingDevice,
            gltfModel
        );

    int32_t baseColorLocalIndex = -1;
    int32_t normalLocalIndex = -1;
    int32_t emissionLocalIndex = -1;
    int32_t metallicRoughnessLocalIndex = -1;

    LOG_TRACE(TEXTURE, "Processing {} materials", gltfModel.materials.size());
    for (uint32_t i = 0; i < gltfModel.materials.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(TEXTURE);
        LOG_TRACE(TEXTURE, "Processing material {}", i);

        const tinygltf::Material& gltfMaterial = gltfModel.materials[i];

        LOG_TRACE(TEXTURE, "Looking for base color texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator baseColorIterator =
            gltfMaterial.values.find("baseColorTexture");
        if (baseColorIterator != gltfMaterial.values.end()) {
            baseColorLocalIndex = baseColorIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found base color texture local index {}", baseColorLocalIndex);
        }

        LOG_TRACE(TEXTURE, "Looking for normal texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator normalIterator =
            gltfMaterial.values.find("normalTexture");
        if (normalIterator != gltfMaterial.values.end()) {
            normalLocalIndex = normalIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found normal texture local index {}", normalLocalIndex);
        }

        LOG_TRACE(TEXTURE, "Looking for emission texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator emissionIterator =
            gltfMaterial.values.find("emissiveTexture");
        if (emissionIterator != gltfMaterial.values.end()) {
            emissionLocalIndex = emissionIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found emission texture local index {}", emissionLocalIndex);
        }

        LOG_TRACE(TEXTURE, "Looking for metallic roughness texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator metallicRoughnessIterator =
            gltfMaterial.values.find("metallicRoughnessTexture");
        if (metallicRoughnessIterator != gltfMaterial.values.end()) {
            metallicRoughnessLocalIndex = metallicRoughnessIterator->second.TextureIndex();
            LOG_TRACE(TEXTURE, "Found metallic roughness texture local index {}", metallicRoughnessLocalIndex);
        }
    }

    const uint32_t baseColorMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getBaseColorDefaultIndex() :
        masterIndices[
            baseColorLocalIndex >= 0 ?
                baseColorLocalIndex :
                0
        ];
    const uint32_t normalMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getNormalDefaultIndex() :
        masterIndices[
            normalLocalIndex >= 0 ?
                normalLocalIndex :
                0
        ];
    const uint32_t emissionMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getEmissionDefaultIndex() :
        masterIndices[
            emissionLocalIndex >= 0 ?
                emissionLocalIndex :
                0
        ];
    const uint32_t metallicRoughnessMasterIndex = masterIndices.empty() ?
        quartz::rendering::Texture::getMetallicRoughnessDefaultIndex() :
        masterIndices[
            metallicRoughnessLocalIndex >= 0 ?
                metallicRoughnessLocalIndex :
                0
        ];
    LOG_TRACE(TEXTURE, "base color         texture master index = {}", baseColorMasterIndex);
    LOG_TRACE(TEXTURE, "normal             texture master index = {}", normalMasterIndex);
    LOG_TRACE(TEXTURE, "emission           texture master index = {}", emissionMasterIndex);
    LOG_TRACE(TEXTURE, "metallic roughness texture master index = {}", metallicRoughnessMasterIndex);

    return {
        baseColorMasterIndex,
        normalMasterIndex,
        emissionMasterIndex,
        metallicRoughnessMasterIndex
    };
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath
) :
    m_gltfModel(quartz::rendering::Model::loadGLTFModel(objectFilepath)),
    m_defaultSceneIndex(
        m_gltfModel.defaultScene <= -1 ?
            0 :
            m_gltfModel.defaultScene
    ),
    m_scenes(
        quartz::rendering::Model::loadScenes(
            renderingDevice,
            m_gltfModel
        )
    ),
    m_material(
        quartz::rendering::Model::loadMaterial(
            renderingDevice,
            m_gltfModel
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_gltfModel(other.m_gltfModel),
    m_scenes(std::move(other.m_scenes)),
    m_material(std::move(other.m_material))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}