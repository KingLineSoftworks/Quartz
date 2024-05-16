#include <glm/vec4.hpp>

#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/texture/Texture.hpp"

uint32_t quartz::rendering::Material::defaultMaterialMasterIndex = 0;
std::vector<std::shared_ptr<quartz::rendering::Material>> quartz::rendering::Material::masterMaterialList;

std::string
quartz::rendering::Material::getAlphaModeGLTFString(const quartz::rendering::Material::AlphaMode mode) {
    switch (mode) {
        case quartz::rendering::Material::AlphaMode::Opaque:
            return "OPAQUE";
        case quartz::rendering::Material::AlphaMode::Mask:
            return "MASK";
        case quartz::rendering::Material::AlphaMode::Blend:
            return "BLEND";
    }
}

quartz::rendering::Material::AlphaMode
quartz::rendering::Material::getAlphaModeFromGLTFString(const std::string& modeString) {
    if (modeString == quartz::rendering::Material::getAlphaModeGLTFString(quartz::rendering::Material::AlphaMode::Opaque)) {
        return quartz::rendering::Material::AlphaMode::Opaque;
    }
    if (modeString == quartz::rendering::Material::getAlphaModeGLTFString(quartz::rendering::Material::AlphaMode::Mask)) {
        return quartz::rendering::Material::AlphaMode::Mask;
    }
    if (modeString == quartz::rendering::Material::getAlphaModeGLTFString(quartz::rendering::Material::AlphaMode::Blend)) {
        return quartz::rendering::Material::AlphaMode::Blend;
    }

    // Default to Opaque if we have no idea what we got
    return quartz::rendering::Material::AlphaMode::Opaque;
}

uint32_t
quartz::rendering::Material::createMaterial(
    const quartz::rendering::Device& renderingDevice,
    const std::string& name,
    const uint32_t baseColorTextureMasterIndex,
    const uint32_t metallicRoughnessTextureMasterIndex,
    const uint32_t normalTextureMasterIndex,
    const uint32_t emissionTextureMasterIndex,
    const uint32_t occlusionTextureMasterIndex,
    const glm::vec4& baseColorFactor,
    const glm::vec3& emissiveFactor,
    const float metallicFactor,
    const float roughnessFactor,
    const quartz::rendering::Material::AlphaMode alphaMode,
    const float alphaCutoff,
    const bool doubleSided
) {
    LOG_FUNCTION_SCOPE_TRACE(MATERIAL, "");

    if (quartz::rendering::Material::masterMaterialList.empty()) {
        LOG_TRACE(MATERIAL, "Master material list is empty. Initializing it");
        quartz::rendering::Material::initializeMasterMaterialList(renderingDevice);
    }

    std::shared_ptr<quartz::rendering::Material> p_material = std::make_shared<quartz::rendering::Material>(
        name,
        baseColorTextureMasterIndex,
        metallicRoughnessTextureMasterIndex,
        normalTextureMasterIndex,
        emissionTextureMasterIndex,
        occlusionTextureMasterIndex,
        baseColorFactor,
        emissiveFactor,
        metallicFactor,
        roughnessFactor,
        alphaMode,
        alphaCutoff,
        doubleSided
    );

    quartz::rendering::Material::masterMaterialList.push_back(p_material);
    uint32_t insertedIndex = quartz::rendering::Material::masterMaterialList.size() - 1;
    LOG_TRACE(MATERIAL, "Newly created material [ {} ] was inserted into master material list at index {}", name, insertedIndex);

    return insertedIndex;
}

void
quartz::rendering::Material::initializeMasterMaterialList(const quartz::rendering::Device& renderingDevice) {
    LOG_FUNCTION_SCOPE_TRACE(MATERIAL, "");

    if (!quartz::rendering::Material::masterMaterialList.empty()) {
        LOG_TRACE(MATERIAL, "Master material list is already initialized. Not doing anything");
        return;
    }

    LOG_TRACE(MATERIAL, "Initializing master texture list");
    quartz::rendering::Texture::initializeMasterTextureList(renderingDevice);

    LOG_TRACE(MATERIAL, "Reserving space for {} materials", QUARTZ_MAX_NUMBER_MATERIALS);
    quartz::rendering::Material::masterMaterialList.reserve(QUARTZ_MAX_NUMBER_MATERIALS);

    LOG_TRACE(MATERIAL, "Creating default material");
    std::shared_ptr<quartz::rendering::Material> p_defaultMaterial = std::make_shared<quartz::rendering::Material>(
        "The_Default_Material",
        quartz::rendering::Texture::getBaseColorDefaultMasterIndex(),
        quartz::rendering::Texture::getMetallicRoughnessDefaultMasterIndex(),
        quartz::rendering::Texture::getNormalDefaultMasterIndex(),
        quartz::rendering::Texture::getEmissionDefaultMasterIndex(),
        quartz::rendering::Texture::getOcclusionDefaultMasterIndex(),

        glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        1.0f,
        1.0f,

        quartz::rendering::Material::AlphaMode::Opaque,
        0.5f,
        false
    );

    quartz::rendering::Material::masterMaterialList.push_back(p_defaultMaterial);
    quartz::rendering::Material::defaultMaterialMasterIndex = quartz::rendering::Material::masterMaterialList.size() - 1;
    LOG_INFO(MATERIAL, "Initialized master material list. Size is now {} with default material at index {}", quartz::rendering::Material::masterMaterialList.size(), quartz::rendering::Material::defaultMaterialMasterIndex);
}

void
quartz::rendering::Material::cleanUpAllMaterials() {
    LOG_FUNCTION_CALL_TRACE(MATERIAL, "");

    quartz::rendering::Material::masterMaterialList.clear();
}

quartz::rendering::Material::Material() :
    m_baseColorTextureMasterIndex(quartz::rendering::Texture::getBaseColorDefaultMasterIndex()),
    m_metallicRoughnessTextureMasterIndex(quartz::rendering::Texture::getMetallicRoughnessDefaultMasterIndex()),
    m_normalTextureMasterIndex(quartz::rendering::Texture::getNormalDefaultMasterIndex()),
    m_emissionTextureMasterIndex(quartz::rendering::Texture::getEmissionDefaultMasterIndex()),
    m_occlusionTextureMasterIndex(quartz::rendering::Texture::getOcclusionDefaultMasterIndex()),
    m_baseColorFactor(1.0f, 1.0f, 1.0f, 1.0f),
    m_emissiveFactor(0.0f, 0.0f, 0.0f),
    m_metallicFactor(1.0f),
    m_roughnessFactor(1.0f),
    m_alphaMode(quartz::rendering::Material::AlphaMode::Opaque),
    m_alphaCutoff(0.5f),
    m_doubleSided(false),
    m_name("A_Default_Material")
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Using all default texture indices");
    LOG_TRACEthis("Name: {}", m_name);
    LOG_TRACEthis("Base Color         master index: {}", m_baseColorTextureMasterIndex);
    LOG_TRACEthis("Metallic Roughness master index: {}", m_metallicRoughnessTextureMasterIndex);
    LOG_TRACEthis("Normal             master index: {}", m_normalTextureMasterIndex);
    LOG_TRACEthis("Emission           master index: {}", m_emissionTextureMasterIndex);
    LOG_TRACEthis("Occlusion          master index: {}", m_occlusionTextureMasterIndex);
    LOG_TRACEthis("Base color factor: {}, {}, {}, {}", m_baseColorFactor.r, m_baseColorFactor.g, m_baseColorFactor.b, m_baseColorFactor.a);
    LOG_TRACEthis("Emissive   factor: {}, {}, {}", m_emissiveFactor.r, m_emissiveFactor.g, m_emissiveFactor.b);
    LOG_TRACEthis("Metallic   factor: {}", m_metallicFactor);
    LOG_TRACEthis("Roughness  factor: {}", m_roughnessFactor);
    LOG_TRACEthis("Alpha mode  : {}", quartz::rendering::Material::getAlphaModeGLTFString(m_alphaMode));
    LOG_TRACEthis("Alpha cutoff: {}", m_alphaCutoff);
    LOG_TRACEthis("Double sided: {:s}", m_doubleSided);
}

quartz::rendering::Material::Material(
    const std::string& name,
    const uint32_t baseColorTextureMasterIndex,
    const uint32_t metallicRoughnessTextureMasterIndex,
    const uint32_t normalTextureMasterIndex,
    const uint32_t emissionTextureMasterIndex,
    const uint32_t occlusionTextureMasterIndex,
    const glm::vec4& baseColorFactor,
    const glm::vec3& emissiveFactor,
    const float metallicFactor,
    const float roughnessFactor,
    const quartz::rendering::Material::AlphaMode alphaMode,
    const float alphaCutoff,
    const bool doubleSided
) :
    m_baseColorTextureMasterIndex(baseColorTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(metallicRoughnessTextureMasterIndex),
    m_normalTextureMasterIndex(normalTextureMasterIndex),
    m_emissionTextureMasterIndex(emissionTextureMasterIndex),
    m_occlusionTextureMasterIndex(occlusionTextureMasterIndex),
    m_baseColorFactor(baseColorFactor),
    m_emissiveFactor(emissiveFactor),
    m_metallicFactor(metallicFactor),
    m_roughnessFactor(roughnessFactor),
    m_alphaMode(alphaMode),
    m_alphaCutoff(alphaCutoff),
    m_doubleSided(doubleSided),
    m_name(name)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Name: {}", m_name);
    LOG_TRACEthis("Base Color         master index: {}", m_baseColorTextureMasterIndex);
    LOG_TRACEthis("Metallic Roughness master index: {}", m_metallicRoughnessTextureMasterIndex);
    LOG_TRACEthis("Normal             master index: {}", m_normalTextureMasterIndex);
    LOG_TRACEthis("Emission           master index: {}", m_emissionTextureMasterIndex);
    LOG_TRACEthis("Occlusion          master index: {}", m_occlusionTextureMasterIndex);
    LOG_TRACEthis("Base color factor: {}, {}, {}, {}", m_baseColorFactor.r, m_baseColorFactor.g, m_baseColorFactor.b, m_baseColorFactor.a);
    LOG_TRACEthis("Emissive   factor: {}, {}, {}", m_emissiveFactor.r, m_emissiveFactor.g, m_emissiveFactor.b);
    LOG_TRACEthis("Metallic   factor: {}", m_metallicFactor);
    LOG_TRACEthis("Roughness  factor: {}", m_roughnessFactor);
    LOG_TRACEthis("Alpha mode  : {}", quartz::rendering::Material::getAlphaModeGLTFString(m_alphaMode));
    LOG_TRACEthis("Alpha cutoff: {}", m_alphaCutoff);
    LOG_TRACEthis("Double sided: {:s}", m_doubleSided);
}

quartz::rendering::Material::Material(
    const quartz::rendering::Material& other
) :
    m_baseColorTextureMasterIndex(other.m_baseColorTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(other.m_metallicRoughnessTextureMasterIndex),
    m_normalTextureMasterIndex(other.m_normalTextureMasterIndex),
    m_emissionTextureMasterIndex(other.m_emissionTextureMasterIndex),
    m_occlusionTextureMasterIndex(other.m_occlusionTextureMasterIndex),
    m_baseColorFactor(other.m_baseColorFactor),
    m_emissiveFactor(other.m_emissiveFactor),
    m_metallicFactor(other.m_metallicFactor),
    m_roughnessFactor(other.m_roughnessFactor),
    m_alphaMode(other.m_alphaMode),
    m_alphaCutoff(other.m_alphaCutoff),
    m_doubleSided(other.m_doubleSided),
    m_name(other.m_name)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Name: {}", m_name);
    LOG_TRACEthis("Base Color         master index: {}", m_baseColorTextureMasterIndex);
    LOG_TRACEthis("Metallic Roughness master index: {}", m_metallicRoughnessTextureMasterIndex);
    LOG_TRACEthis("Normal             master index: {}", m_normalTextureMasterIndex);
    LOG_TRACEthis("Emission           master index: {}", m_emissionTextureMasterIndex);
    LOG_TRACEthis("Occlusion          master index: {}", m_occlusionTextureMasterIndex);
    LOG_TRACEthis("Base color factor: {}, {}, {}, {}", m_baseColorFactor.r, m_baseColorFactor.g, m_baseColorFactor.b, m_baseColorFactor.a);
    LOG_TRACEthis("Emissive   factor: {}, {}, {}", m_emissiveFactor.r, m_emissiveFactor.g, m_emissiveFactor.b);
    LOG_TRACEthis("Metallic   factor: {}", m_metallicFactor);
    LOG_TRACEthis("Roughness  factor: {}", m_roughnessFactor);
    LOG_TRACEthis("Alpha mode  : {}", quartz::rendering::Material::getAlphaModeGLTFString(m_alphaMode));
    LOG_TRACEthis("Alpha cutoff: {}", m_alphaCutoff);
    LOG_TRACEthis("Double sided: {:s}", m_doubleSided);
}

quartz::rendering::Material::Material(
    quartz::rendering::Material&& other
) :
    m_baseColorTextureMasterIndex(other.m_baseColorTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(other.m_metallicRoughnessTextureMasterIndex),
    m_normalTextureMasterIndex(other.m_normalTextureMasterIndex),
    m_emissionTextureMasterIndex(other.m_emissionTextureMasterIndex),
    m_occlusionTextureMasterIndex(other.m_occlusionTextureMasterIndex),
    m_baseColorFactor(other.m_baseColorFactor),
    m_emissiveFactor(other.m_emissiveFactor),
    m_metallicFactor(other.m_metallicFactor),
    m_roughnessFactor(other.m_roughnessFactor),
    m_alphaMode(other.m_alphaMode),
    m_alphaCutoff(other.m_alphaCutoff),
    m_doubleSided(other.m_doubleSided),
    m_name(other.m_name)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Name: {}", m_name);
    LOG_TRACEthis("Base Color         master index: {}", m_baseColorTextureMasterIndex);
    LOG_TRACEthis("Metallic Roughness master index: {}", m_metallicRoughnessTextureMasterIndex);
    LOG_TRACEthis("Normal             master index: {}", m_normalTextureMasterIndex);
    LOG_TRACEthis("Emission           master index: {}", m_emissionTextureMasterIndex);
    LOG_TRACEthis("Occlusion          master index: {}", m_occlusionTextureMasterIndex);
    LOG_TRACEthis("Base color factor: {}, {}, {}, {}", m_baseColorFactor.r, m_baseColorFactor.g, m_baseColorFactor.b, m_baseColorFactor.a);
    LOG_TRACEthis("Emissive   factor: {}, {}, {}", m_emissiveFactor.r, m_emissiveFactor.g, m_emissiveFactor.b);
    LOG_TRACEthis("Metallic   factor: {}", m_metallicFactor);
    LOG_TRACEthis("Roughness  factor: {}", m_roughnessFactor);
    LOG_TRACEthis("Alpha mode  : {}", quartz::rendering::Material::getAlphaModeGLTFString(m_alphaMode));
    LOG_TRACEthis("Alpha cutoff: {}", m_alphaCutoff);
    LOG_TRACEthis("Double sided: {:s}", m_doubleSided);
}

quartz::rendering::Material::~Material() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material&
quartz::rendering::Material::operator=(
    const quartz::rendering::Material& other
) {
    if (&other == this) {
        return *this;
    }

    LOG_FUNCTION_CALL_TRACEthis(
        "base color {}, metallic roughness {}, normal {}, emission {}, occlusion {}",
        other.m_baseColorTextureMasterIndex,
        other.m_metallicRoughnessTextureMasterIndex,
        other.m_normalTextureMasterIndex,
        other.m_emissionTextureMasterIndex,
        other.m_occlusionTextureMasterIndex
    );

    m_baseColorTextureMasterIndex = other.m_baseColorTextureMasterIndex;
    m_metallicRoughnessTextureMasterIndex = other.m_metallicRoughnessTextureMasterIndex;
    m_normalTextureMasterIndex = other.m_normalTextureMasterIndex;
    m_emissionTextureMasterIndex = other.m_emissionTextureMasterIndex;
    m_occlusionTextureMasterIndex = other.m_occlusionTextureMasterIndex;
    m_baseColorFactor = other.m_baseColorFactor;
    m_emissiveFactor = other.m_emissiveFactor;
    m_metallicFactor = other.m_metallicFactor;
    m_roughnessFactor = other.m_roughnessFactor;
    m_alphaMode = other.m_alphaMode;
    m_alphaCutoff = other.m_alphaCutoff;
    m_doubleSided = other.m_doubleSided;
    m_name = other.m_name;

    return *this;
}