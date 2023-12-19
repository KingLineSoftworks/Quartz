#include <glm/vec3.hpp>

#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/texture/Texture.hpp"

quartz::rendering::Material::Material() :
    m_baseColorTextureMasterIndex(quartz::rendering::Texture::getBaseColorDefaultIndex()),
    m_normalTextureMasterIndex(quartz::rendering::Texture::getNormalDefaultIndex()),
    m_emissiveTextureMasterIndex(quartz::rendering::Texture::getEmissiveDefaultIndex()),
    m_metallicRoughnessTextureMasterIndex(quartz::rendering::Texture::getMetallicRoughnessDefaultIndex()),
    m_baseColorFactor(1.0f, 1.0f, 1.0f),
    m_emissiveFactor(1.0f, 1.0f, 1.0f),
    m_metallicFactor(1.0f),
    m_roughnessFactor(1.0f)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    const uint32_t baseColorTextureMasterIndex,
    const uint32_t normalTextureMasterIndex,
    const uint32_t emissiveTextureMasterIndex,
    const uint32_t metallicRoughnessTextureMasterIndex,
    const glm::vec3& baseColorFactor,
    const glm::vec3& emissiveFactor,
    const float metallicFactor,
    const float roughnessFactor
) :
    m_baseColorTextureMasterIndex(baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(normalTextureMasterIndex),
    m_emissiveTextureMasterIndex(emissiveTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(metallicRoughnessTextureMasterIndex),
    m_baseColorFactor(baseColorFactor),
    m_emissiveFactor(emissiveFactor),
    m_metallicFactor(metallicFactor),
    m_roughnessFactor(roughnessFactor)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    const quartz::rendering::Material& other
) :
    m_baseColorTextureMasterIndex(other.m_baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(other.m_normalTextureMasterIndex),
    m_emissiveTextureMasterIndex(other.m_emissiveTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(other.m_metallicRoughnessTextureMasterIndex),
    m_baseColorFactor(other.m_baseColorFactor),
    m_emissiveFactor(other.m_emissiveFactor),
    m_metallicFactor(other.m_metallicFactor),
    m_roughnessFactor(other.m_roughnessFactor)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    quartz::rendering::Material&& other
) :
    m_baseColorTextureMasterIndex(other.m_baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(other.m_normalTextureMasterIndex),
    m_emissiveTextureMasterIndex(other.m_emissiveTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(other.m_metallicRoughnessTextureMasterIndex),
    m_baseColorFactor(other.m_baseColorFactor),
    m_emissiveFactor(other.m_emissiveFactor),
    m_metallicFactor(other.m_metallicFactor),
    m_roughnessFactor(other.m_roughnessFactor)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::~Material() {
    LOG_FUNCTION_CALL_TRACEthis("");
}