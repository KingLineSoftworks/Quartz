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

    const bool isBinaryFile = util::FileSystem::getFileExtension(filepath) == "glb";

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
        if (!warningString.empty()) {
            LOG_THROW(MODEL, util::AssetLoadFailedError, "Failed to load model at {} (warning: \"{}\")", filepath, warningString);
        } else if (!errorString.empty()) {
            LOG_THROW(MODEL, util::AssetLoadFailedError, "Failed to load model at {} (error: \"{}\")", filepath, errorString);
        }
        LOG_THROW(MODEL, util::AssetLoadFailedError, "Failed to load model at {} (no warning or error strings)", filepath);
    }

    if (!warningString.empty()) {
        LOG_WARNING(MODEL, "TinyGLTF::Load{}FromFile warning : {}", isBinaryFile ? "Binary" : "ASCII", warningString);
    }

    if (!errorString.empty()) {
        LOG_ERROR(MODEL, "TinyGLTF::Load{}FromFile error : {}", isBinaryFile ? "Binary" : "ASCII", errorString);
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
    LOG_TRACE(MODEL, "  Getting texture with local index {} using master indices list of size {}", localIndex, masterIndices.size());

    if (masterIndices.empty() || localIndex < 0) {
        switch (textureType) {
            case quartz::rendering::Texture::Type::BaseColor:
                LOG_TRACE(MODEL, "  Using default base color texture: {}", quartz::rendering::Texture::getBaseColorDefaultMasterIndex());
                return quartz::rendering::Texture::getBaseColorDefaultMasterIndex();
            case quartz::rendering::Texture::Type::MetallicRoughness:
                LOG_TRACE(MODEL, "  Using default metallic roughness texture: {}", quartz::rendering::Texture::getMetallicRoughnessDefaultMasterIndex());
                return quartz::rendering::Texture::getMetallicRoughnessDefaultMasterIndex();
            case quartz::rendering::Texture::Type::Normal:
                LOG_TRACE(MODEL, "  Using default normal texture: {}", quartz::rendering::Texture::getNormalDefaultMasterIndex());
                return quartz::rendering::Texture::getNormalDefaultMasterIndex();
            case quartz::rendering::Texture::Type::Emission:
                LOG_TRACE(MODEL, "  Using default emission texture: {}", quartz::rendering::Texture::getEmissionDefaultMasterIndex());
                return quartz::rendering::Texture::getEmissionDefaultMasterIndex();
            case quartz::rendering::Texture::Type::Occlusion:
                LOG_TRACE(MODEL, "  Using default occlusion texture: {}", quartz::rendering::Texture::getOcclusionDefaultMasterIndex());
                return quartz::rendering::Texture::getOcclusionDefaultMasterIndex();
        }
    }

    return masterIndices[localIndex];
}

uint32_t
quartz::rendering::Model::getTextureMasterIndex(
    const tinygltf::Material& gltfMaterial,
    const std::vector<uint32_t>& masterIndices,
    const quartz::rendering::Texture::Type textureType
) {
    const std::string textureTypeString = quartz::rendering::Texture::getTextureTypeGLTFString(textureType);

    LOG_TRACE(MODEL, "Looking for {}", textureTypeString);

    int32_t localIndex = -1; // Will still be -1 if no texture of this type was provided to material

    switch(textureType) {
        case quartz::rendering::Texture::Type::BaseColor:
            localIndex = gltfMaterial.pbrMetallicRoughness.baseColorTexture.index;
            break;
        case quartz::rendering::Texture::Type::MetallicRoughness:
            localIndex = gltfMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index;
            break;
        case quartz::rendering::Texture::Type::Normal:
            localIndex = gltfMaterial.normalTexture.index;
            break;
        case quartz::rendering::Texture::Type::Emission:
            localIndex = gltfMaterial.emissiveTexture.index;
            break;
        case quartz::rendering::Texture::Type::Occlusion:
            localIndex = gltfMaterial.occlusionTexture.index;
            break;
    }

    LOG_TRACE(MODEL, "  Got {} local index of {} from gltf material", textureTypeString, localIndex);

    const uint32_t masterIndex = quartz::rendering::Model::getMasterTextureIndexFromLocalIndex(
        masterIndices,
        localIndex,
        textureType
    );

    LOG_TRACE(MODEL, "  {} master index = {}", textureTypeString, masterIndex);

    return masterIndex;
}

std::vector<quartz::rendering::Material>
quartz::rendering::Model::loadMaterials(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    std::vector<uint32_t> masterIndices = quartz::rendering::Model::loadTextures(
        renderingDevice,
        gltfModel
    );

    LOG_TRACE(MODEL, "Creating list of materials, initialized with a default material");
    std::vector<quartz::rendering::Material> materials = {
        {} // a default material
    };

    LOG_TRACE(MODEL, "Processing {} materials", gltfModel.materials.size());
    for (uint32_t i = 0; i < gltfModel.materials.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        LOG_TRACE(MODEL, "Processing material {}", i);

        const tinygltf::Material& gltfMaterial = gltfModel.materials[i];

        const uint32_t baseColorMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterIndices,
            quartz::rendering::Texture::Type::BaseColor
        );
        const uint32_t metallicRoughnessMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterIndices,
            quartz::rendering::Texture::Type::MetallicRoughness
        );
        const uint32_t normalMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterIndices,
            quartz::rendering::Texture::Type::Normal
        );
        const uint32_t emissionMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterIndices,
            quartz::rendering::Texture::Type::Emission
        );
        const uint32_t occlusionMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterIndices,
            quartz::rendering::Texture::Type::Occlusion
        );

        const std::vector<double>& baseColorFactorVector = gltfMaterial.pbrMetallicRoughness.baseColorFactor; // assuming length == 4
        const glm::vec4 baseColorFactor = glm::vec4(
            baseColorFactorVector[0],
            baseColorFactorVector[1],
            baseColorFactorVector[2],
            baseColorFactorVector[3]
        );

        const std::vector<double>& emissiveFactorVector = gltfMaterial.emissiveFactor; // assuming length == 3
        const glm::vec3 emissiveFactor = glm::vec3(
            emissiveFactorVector[0],
            emissiveFactorVector[1],
            emissiveFactorVector[2]
        );

        const double metallicFactor = gltfMaterial.pbrMetallicRoughness.metallicFactor;

        const double roughnessFactor = gltfMaterial.pbrMetallicRoughness.roughnessFactor;

        const quartz::rendering::Material::AlphaMode alphaMode = quartz::rendering::Material::getAlphaModeFromGLTFString(gltfMaterial.alphaMode);

        const double alphaCutoff = gltfMaterial.alphaCutoff;

        const bool doubleSided = gltfMaterial.doubleSided;

        materials.emplace_back(
            baseColorMasterIndex,
            metallicRoughnessMasterIndex,
            normalMasterIndex,
            emissionMasterIndex,
            occlusionMasterIndex,
            baseColorFactor,
            emissiveFactor,
            metallicFactor,
            roughnessFactor,
            alphaMode,
            alphaCutoff,
            doubleSided
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