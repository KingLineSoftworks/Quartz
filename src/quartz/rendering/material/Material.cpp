#include <glm/vec4.hpp>

#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/texture/Texture.hpp"

quartz::rendering::Material::Material() :
    m_baseColorTextureMasterIndex(quartz::rendering::Texture::getBaseColorDefaultMasterIndex()),
    m_metallicRoughnessTextureMasterIndex(quartz::rendering::Texture::getMetallicRoughnessDefaultMasterIndex()),
    m_normalTextureMasterIndex(quartz::rendering::Texture::getNormalDefaultMasterIndex()),
    m_emissionTextureMasterIndex(quartz::rendering::Texture::getEmissionDefaultMasterIndex()),
    m_occlusionTextureMasterIndex(quartz::rendering::Texture::getOcclusionDefaultMasterIndex()),
    m_baseColorFactor(1.0f, 1.0f, 1.0f, 1.0f),
    m_emissiveFactor(0.0f, 0.0f, 0.0f),
    m_metallicFactor(1.0f),
    m_roughnessFactor(1.0f)
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
    const float roughnessFactor
) :
    m_baseColorTextureMasterIndex(baseColorTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(metallicRoughnessTextureMasterIndex),
    m_normalTextureMasterIndex(normalTextureMasterIndex),
    m_emissionTextureMasterIndex(emissionTextureMasterIndex),
    m_occlusionTextureMasterIndex(occlusionTextureMasterIndex),
    m_baseColorFactor(baseColorFactor),
    m_emissiveFactor(emissiveFactor),
    m_metallicFactor(metallicFactor),
    m_roughnessFactor(roughnessFactor)
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
    m_roughnessFactor(other.m_roughnessFactor)
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
    m_roughnessFactor(other.m_roughnessFactor)
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
}

quartz::rendering::Material::~Material() {
    LOG_FUNCTION_CALL_TRACEthis("");
}