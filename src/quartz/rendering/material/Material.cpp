#include "quartz/rendering/material/Material.hpp"

quartz::rendering::Material::Material() :
    m_baseColorTextureMasterIndex(0),
    m_normalTextureMasterIndex(0),
    m_emissionTextureMasterIndex(0),
    m_metallicRoughnessTextureMasterIndex(0)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    const uint32_t baseColorTextureMasterIndex,
    const uint32_t normalTextureMasterIndex,
    const uint32_t emissionTextureMasterIndex,
    const uint32_t metallicRoughnessTextureMasterIndex
) :
    m_baseColorTextureMasterIndex(baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(normalTextureMasterIndex),
    m_emissionTextureMasterIndex(emissionTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(metallicRoughnessTextureMasterIndex)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    quartz::rendering::Material&& other
) :
    m_baseColorTextureMasterIndex(other.m_baseColorTextureMasterIndex),
    m_normalTextureMasterIndex(other.m_normalTextureMasterIndex),
    m_emissionTextureMasterIndex(other.m_emissionTextureMasterIndex),
    m_metallicRoughnessTextureMasterIndex(other.m_metallicRoughnessTextureMasterIndex)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::~Material() {
    LOG_FUNCTION_CALL_TRACEthis("");
}