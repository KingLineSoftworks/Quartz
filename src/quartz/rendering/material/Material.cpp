#include "quartz/rendering/material/Material.hpp"

quartz::rendering::Material::Material() :
    m_baseColorTextureMasterIndex(-1),
    m_normalTextureMasterIndex(-1),
    m_emissionTextureMasterIndex(-1),
    m_metallicRoughnessTextureMasterIndex(-1)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Material::Material(
    const int32_t baseColorTextureMasterIndex,
    const int32_t normalTextureMasterIndex,
    const int32_t emissionTextureMasterIndex,
    const int32_t metallicRoughnessTextureMasterIndex
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