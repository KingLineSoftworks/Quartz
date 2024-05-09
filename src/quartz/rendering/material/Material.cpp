#include <glm/vec4.hpp>

#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/texture/Texture.hpp"

std::string quartz::rendering::Material::getAlphaModeGLTFString(const quartz::rendering::Material::AlphaMode mode) {
    switch (mode) {
        case quartz::rendering::Material::AlphaMode::Opaque:
            return "OPAQUE";
        case quartz::rendering::Material::AlphaMode::Mask:
            return "MASK";
        case quartz::rendering::Material::AlphaMode::Blend:
            return "BLEND";
    }
}

quartz::rendering::Material::AlphaMode quartz::rendering::Material::getAlphaModeFromGLTFString(const std::string& modeString) {
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
    m_doubleSided(false)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
    LOG_TRACEthis("Using all default texture indices");
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
    const double alphaCutoff,
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
    m_doubleSided(doubleSided)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
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
    m_doubleSided(other.m_doubleSided)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
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
    m_doubleSided(other.m_doubleSided)
{
    LOG_FUNCTION_SCOPE_TRACEthis("");
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