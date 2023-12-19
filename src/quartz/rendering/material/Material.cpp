#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/texture/Texture.hpp"

quartz::rendering::Material::Material() :
    m_baseColorTextureMasterIndex(quartz::rendering::Texture::getBaseColorDefaultIndex()),
    m_normalTextureMasterIndex(quartz::rendering::Texture::getNormalDefaultIndex()),
    m_emissiveTextureMasterIndex(quartz::rendering::Texture::getEmissiveDefaultIndex()),
    m_metallicRoughnessTextureMasterIndex(quartz::rendering::Texture::getMetallicRoughnessDefaultIndex())
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    const uint32_t baseColorTextureMasterIndex,
    const uint32_t normalTextureMasterIndex,
    const uint32_t emissiveTextureMasterIndex,
    const uint32_t metallicRoughnessTextureMasterIndex
) :
    m_baseColorTextureMasterIndex(baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(normalTextureMasterIndex),
    m_emissiveTextureMasterIndex(emissiveTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(metallicRoughnessTextureMasterIndex)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    const quartz::rendering::Material& other
) :
    m_baseColorTextureMasterIndex(other.m_baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(other.m_normalTextureMasterIndex),
    m_emissiveTextureMasterIndex(other.m_emissiveTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(other.m_metallicRoughnessTextureMasterIndex)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    quartz::rendering::Material&& other
) :
    m_baseColorTextureMasterIndex(other.m_baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(other.m_normalTextureMasterIndex),
    m_emissiveTextureMasterIndex(other.m_emissiveTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(other.m_metallicRoughnessTextureMasterIndex)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::~Material() {
    LOG_FUNCTION_CALL_TRACEthis("");
}