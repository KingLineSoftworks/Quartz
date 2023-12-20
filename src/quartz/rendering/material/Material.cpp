#include <memory>
#include <vector>

#include <glm/vec3.hpp>

#include "quartz/rendering/material/Material.hpp"
#include "quartz/rendering/texture/Texture.hpp"

uint32_t quartz::rendering::Material::defaultMaterialIndex = 0;
std::vector<std::shared_ptr<quartz::rendering::Material>> quartz::rendering::Material::masterList;

uint32_t
quartz::rendering::Material::createMaterial(
    const quartz::rendering::Device& renderingDevice,
    const uint32_t baseColorTextureMasterIndex,
    const uint32_t normalTextureMasterIndex,
    const uint32_t emissiveTextureMasterIndex,
    const uint32_t metallicRoughnessTextureMasterIndex,
    const glm::vec3& baseColorFactor,
    const glm::vec3& emissiveFactor,
    const float metallicFactor,
    const float roughnessFactor
) {
    LOG_FUNCTION_SCOPE_TRACE(MATERIAL, "");

    if (quartz::rendering::Material::masterList.empty()) {
        LOG_TRACE(MATERIAL, "Master list is empty, initializing");
        quartz::rendering::Material::initializeMasterList(renderingDevice);
    }

    std::shared_ptr<quartz::rendering::Material> p_material =
        std::make_shared<quartz::rendering::Material>(
            baseColorTextureMasterIndex,
            normalTextureMasterIndex,
            emissiveTextureMasterIndex,
            metallicRoughnessTextureMasterIndex,
            baseColorFactor,
            emissiveFactor,
            metallicFactor,
            roughnessFactor
        );

    quartz::rendering::Material::masterList.push_back(p_material);

    uint32_t insertedIndex = quartz::rendering::Material::masterList.size() - 1;
    LOG_TRACE(MATERIAL, "Material was inserted into master list at index {}", insertedIndex);

    return insertedIndex;
}

void
quartz::rendering::Material::initializeMasterList(
    const quartz::rendering::Device& renderingDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(MATERIAL, "");

    if (!quartz::rendering::Material::masterList.empty()) {
        LOG_TRACE(MATERIAL, "Master list is already initialized. Not doing anything");
        return;
    }

    LOG_TRACE(MATERIAL, "Initializing textures");
    quartz::rendering::Texture::initializeMasterList(renderingDevice);

    /**
     * @todo 2023/12/19 Compile time determine number of materials we want to have and
     *   reserve that many spaces for them right here
     */

    LOG_TRACE(MATERIAL, "Creating default material. Assuming textures have already been initialized");
    std::shared_ptr<quartz::rendering::Material> p_defaultMaterial =
        std::make_shared<quartz::rendering::Material>(
            quartz::rendering::Texture::getBaseColorDefaultIndex(),
            quartz::rendering::Texture::getNormalDefaultIndex(),
            quartz::rendering::Texture::getEmissiveDefaultIndex(),
            quartz::rendering::Texture::getMetallicRoughnessDefaultIndex(),
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(1.0f, 1.0f, 1.0f),
            1.0f,
            1.0f
        );
    quartz::rendering::Material::masterList.push_back(p_defaultMaterial);
    quartz::rendering::Material::defaultMaterialIndex = quartz::rendering::Material::masterList.size() - 1;
    LOG_INFO(MATERIAL, "Default material at index {}", quartz::rendering::Material::defaultMaterialIndex);
}

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