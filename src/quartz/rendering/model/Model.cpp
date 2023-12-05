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

uint32_t
quartz::rendering::Model::getMasterTextureIndexFromLocalIndex(
    const std::vector<uint32_t>& masterIndices,
    const int32_t localIndex,
    const quartz::rendering::Texture::Type textureType
) {
    if (masterIndices.empty()) {
        switch (textureType) {
            case quartz::rendering::Texture::Type::BaseColor:
                return quartz::rendering::Texture::getBaseColorDefaultIndex();
            case quartz::rendering::Texture::Type::Normal:
                return quartz::rendering::Texture::getNormalDefaultIndex();
            case quartz::rendering::Texture::Type::Emission:
                return quartz::rendering::Texture::getEmissionDefaultIndex();
            case quartz::rendering::Texture::Type::MetallicRoughness:
                return quartz::rendering::Texture::getMetallicRoughnessDefaultIndex();
        }
    }

    if (localIndex < 0) {
        return masterIndices[0];
    }

    return masterIndices[localIndex];
}

std::vector<quartz::rendering::Material>
quartz::rendering::Model::loadMaterials(
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

    std::vector<quartz::rendering::Material> materials = {
        {} // a default material
    };

    LOG_TRACE(MODEL, "Processing {} materials", gltfModel.materials.size());
    for (uint32_t i = 0; i < gltfModel.materials.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        LOG_TRACE(MODEL, "Processing material {}", i);

        const tinygltf::Material& gltfMaterial = gltfModel.materials[i];

        LOG_TRACE(MODEL, "Looking for base color texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator baseColorIterator =
            gltfMaterial.values.find("baseColorTexture");
        if (baseColorIterator != gltfMaterial.values.end()) {
            baseColorLocalIndex = baseColorIterator->second.TextureIndex();
            LOG_TRACE(MODEL, "Found base color texture local index {}", baseColorLocalIndex);
        }
        const uint32_t baseColorMasterIndex =
            quartz::rendering::Model::getMasterTextureIndexFromLocalIndex(
                masterIndices,
                baseColorLocalIndex,
                quartz::rendering::Texture::Type::BaseColor
            );
        LOG_TRACE(MODEL, "base color         texture master index = {}", baseColorMasterIndex);

        LOG_TRACE(MODEL, "Looking for normal texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator normalIterator =
            gltfMaterial.values.find("normalTexture");
        if (normalIterator != gltfMaterial.values.end()) {
            normalLocalIndex = normalIterator->second.TextureIndex();
            LOG_TRACE(MODEL, "Found normal texture local index {}", normalLocalIndex);
        }
        const uint32_t normalMasterIndex =
            quartz::rendering::Model::getMasterTextureIndexFromLocalIndex(
                masterIndices,
                normalLocalIndex,
                quartz::rendering::Texture::Type::Normal
            );
        LOG_TRACE(MODEL, "normal             texture master index = {}", normalMasterIndex);

        LOG_TRACE(MODEL, "Looking for emission texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator emissionIterator =
            gltfMaterial.values.find("emissiveTexture");
        if (emissionIterator != gltfMaterial.values.end()) {
            emissionLocalIndex = emissionIterator->second.TextureIndex();
            LOG_TRACE(MODEL, "Found emission texture local index {}", emissionLocalIndex);
        }
        const uint32_t emissionMasterIndex =
            quartz::rendering::Model::getMasterTextureIndexFromLocalIndex(
                masterIndices,
                emissionLocalIndex,
                quartz::rendering::Texture::Type::Emission
            );
        LOG_TRACE(MODEL, "emission           texture master index = {}", emissionMasterIndex);

        LOG_TRACE(MODEL, "Looking for metallic roughness texture");
        std::map<std::string, tinygltf::Parameter>::const_iterator metallicRoughnessIterator =
            gltfMaterial.values.find("metallicRoughnessTexture");
        if (metallicRoughnessIterator != gltfMaterial.values.end()) {
            metallicRoughnessLocalIndex = metallicRoughnessIterator->second.TextureIndex();
            LOG_TRACE(MODEL, "Found metallic roughness texture local index {}", metallicRoughnessLocalIndex);
        }
        const uint32_t metallicRoughnessMasterIndex =
            quartz::rendering::Model::getMasterTextureIndexFromLocalIndex(
                masterIndices,
                metallicRoughnessLocalIndex,
                quartz::rendering::Texture::Type::MetallicRoughness
            );
        LOG_TRACE(MODEL, "metallic roughness texture master index = {}", metallicRoughnessMasterIndex);

        materials.emplace_back(
            baseColorMasterIndex,
            normalMasterIndex,
            emissionMasterIndex,
            metallicRoughnessMasterIndex
        );
    }

    return materials;
}

std::vector<quartz::rendering::Scene>
quartz::rendering::Model::loadScenes(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const std::vector<quartz::rendering::Material>& materials
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
            gltfScene,
            materials
        );
    }

    return scenes;
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath
) :
    m_gltfModel(quartz::rendering::Model::loadGLTFModel(objectFilepath)),
    m_materials(
        quartz::rendering::Model::loadMaterials(
            renderingDevice,
            m_gltfModel
        )
    ),
    m_defaultSceneIndex(
        m_gltfModel.defaultScene <= -1 ?
            0 :
            m_gltfModel.defaultScene
    ),
    m_scenes(
        quartz::rendering::Model::loadScenes(
            renderingDevice,
            m_gltfModel,
            m_materials
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_gltfModel(other.m_gltfModel),
    m_materials(std::move(other.m_materials)),
    m_defaultSceneIndex(other.m_defaultSceneIndex),
    m_scenes(std::move(other.m_scenes))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}