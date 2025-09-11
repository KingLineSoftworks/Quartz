#include <string>
#include <queue>

#include <glm/vec3.hpp>

#include <tiny_gltf.h>

#include "util/errors/RichException.hpp"
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
            LOG_THROW(MODEL, util::StringException, filepath, "Failed to load model at {} (warning: \"{}\")", filepath, warningString);
        } else if (!errorString.empty()) {
            LOG_THROW(MODEL, util::StringException, filepath, "Failed to load model at {} (error: \"{}\")", filepath, errorString);
        }
        LOG_THROW(MODEL, util::StringException, filepath, "Failed to load model at {} (no warning or error strings)", filepath);
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

    quartz::rendering::Texture::initializeMasterTextureList(renderingDevice);

    std::vector<uint32_t> masterIndices;

    for (uint32_t i = 0; i < gltfModel.textures.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        const tinygltf::Texture& gltfTexture = gltfModel.textures[i];

        LOG_TRACE(MODEL, "Creating texture {} with name \"{}\"", i, gltfTexture.name);

        tinygltf::Sampler gltfSampler;
        const int32_t samplerIndex = gltfTexture.sampler;
        if (samplerIndex == -1) {
            LOG_DEBUG(MODEL, "No gltf sampler specified. Using default one");
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

std::vector<uint32_t>
quartz::rendering::Model::loadMaterialMasterIndices(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel
) {
    /**
     * @brief It is okay if the gltf model does not have any materials, and consequently it is okay if we do
     *    not populate the material master indices vector.
     *    This is because the primitive has a local material index, which will be less than 0 if it should
     *    not be using a material. If this is the case, the primitive will load the default material master
     *    index by default. When it does this, it does not rely on any values in the material master indices
     *    vector.
     *    We still want to ensure the master material list is initialized, but we do not need to populate
     *    this master material indices list with the default material.
     */

    LOG_FUNCTION_SCOPE_TRACE(MODEL, "");

    quartz::rendering::Material::initializeMasterMaterialList(renderingDevice);

    std::vector<uint32_t> masterTextureIndices = quartz::rendering::Model::loadTextures(renderingDevice, gltfModel);
    LOG_TRACE(MODEL, "Loaded {} texture indices", masterTextureIndices.size());

    LOG_TRACE(MODEL, "Creating list of materials");
    std::vector<uint32_t> masterMaterialIndices;

    LOG_TRACE(MODEL, "Reserving space for {} elements in materials list", gltfModel.materials.size());
    masterMaterialIndices.reserve(gltfModel.materials.size());

    LOG_TRACE(MODEL, "Processing {} materials", gltfModel.materials.size());
    for (uint32_t i = 0; i < gltfModel.materials.size(); ++i) {
        LOG_SCOPE_CHANGE_TRACE(MODEL);
        LOG_TRACE(MODEL, "Processing material {}", i);

        const tinygltf::Material& gltfMaterial = gltfModel.materials[i];
        const std::string materialName = gltfMaterial.name;
        LOG_TRACE(MODEL, "Material has name {}", materialName);

        const uint32_t baseColorMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterTextureIndices,
            quartz::rendering::Texture::Type::BaseColor
        );
        const uint32_t metallicRoughnessMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterTextureIndices,
            quartz::rendering::Texture::Type::MetallicRoughness
        );
        const uint32_t normalMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterTextureIndices,
            quartz::rendering::Texture::Type::Normal
        );
        const uint32_t emissionMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterTextureIndices,
            quartz::rendering::Texture::Type::Emission
        );
        const uint32_t occlusionMasterIndex = quartz::rendering::Model::getTextureMasterIndex(
            gltfMaterial,
            masterTextureIndices,
            quartz::rendering::Texture::Type::Occlusion
        );

        const std::vector<double>& baseColorFactorVector = gltfMaterial.pbrMetallicRoughness.baseColorFactor; // assuming length == 4
        const math::Vec4 baseColorFactor(
            baseColorFactorVector[0],
            baseColorFactorVector[1],
            baseColorFactorVector[2],
            baseColorFactorVector[3]
        );

        const std::vector<double>& emissiveFactorVector = gltfMaterial.emissiveFactor; // assuming length == 3
        const math::Vec3 emissiveFactor = math::Vec3(
            emissiveFactorVector[0],
            emissiveFactorVector[1],
            emissiveFactorVector[2]
        );

        const double metallicFactor = gltfMaterial.pbrMetallicRoughness.metallicFactor;

        const double roughnessFactor = gltfMaterial.pbrMetallicRoughness.roughnessFactor;

        const quartz::rendering::Material::AlphaMode alphaMode = quartz::rendering::Material::getAlphaModeFromGLTFString(gltfMaterial.alphaMode);

        const float alphaCutoff = gltfMaterial.alphaCutoff;

        const bool doubleSided = gltfMaterial.doubleSided;

        LOG_TRACE(MODEL, "Using texture master indices:");
        LOG_TRACE(MODEL, "  Base Color         : {}", baseColorMasterIndex);
        LOG_TRACE(MODEL, "  Metallic Roughness : {}", metallicRoughnessMasterIndex);
        LOG_TRACE(MODEL, "  Normal             : {}", normalMasterIndex);
        LOG_TRACE(MODEL, "  Emission           : {}", emissionMasterIndex);
        LOG_TRACE(MODEL, "  Occlusion          : {}", occlusionMasterIndex);

        uint32_t currentMaterialMasterIndex = quartz::rendering::Material::createMaterial(
            renderingDevice,
            materialName,
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

        LOG_TRACE(MODEL, "Pushing material with master index {} to back of the list", currentMaterialMasterIndex);
        masterMaterialIndices.push_back(currentMaterialMasterIndex);
        LOG_TRACE(MODEL, "Materials list is now of size {}", masterMaterialIndices.size());
        for (uint32_t i = 0; i < masterMaterialIndices.size(); ++i) {
            LOG_TRACE(MODEL, "  local index {} = master index {}", i, masterMaterialIndices[i]);
        }
    }

    LOG_TRACE(MODEL, "Model's master material indices:");
    for (uint32_t i = 0; i < masterMaterialIndices.size(); ++i) {
        LOG_TRACE(MODEL, "  local index {} = master index {}", i, masterMaterialIndices[i]);
    }

    return masterMaterialIndices;
}

std::vector<quartz::rendering::Scene>
quartz::rendering::Model::loadScenes(
    const quartz::rendering::Device& renderingDevice,
    const tinygltf::Model& gltfModel,
    const std::vector<uint32_t>& materialMasterIndices
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
            materialMasterIndices
        );
    }

    return scenes;
}

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath
) :
    m_gltfModel(
        quartz::rendering::Model::loadGLTFModel(objectFilepath)
    ),
    m_materialMasterIndices(
        quartz::rendering::Model::loadMaterialMasterIndices(
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
            m_materialMasterIndices
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_gltfModel(std::move(other.m_gltfModel)),
    m_materialMasterIndices(std::move(other.m_materialMasterIndices)),
    m_defaultSceneIndex(std::move(other.m_defaultSceneIndex)),
    m_scenes(std::move(other.m_scenes))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}
