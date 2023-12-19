#pragma once

#include <memory>
#include <vector>

#include <glm/vec3.hpp>

#include "quartz/rendering/Loggers.hpp"

namespace quartz {
namespace rendering {
    class Material;
}
}

/**
 * @brief Based on GLTF 2.0 materials specified <a href="https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material">here</a>
 */
class quartz::rendering::Material {
public: // static functions
    static uint32_t createMaterial(
        const uint32_t baseColorTextureMasterIndex,
        const uint32_t normalTextureMasterIndex,
        const uint32_t emissiveTextureMasterIndex,
        const uint32_t metallicRoughnessTextureMasterIndex,
        const glm::vec3& baseColorFactor,
        const glm::vec3& emissiveFactor,
        const float metallicFactor,
        const float roughnessFactor
    );
    static void initializeMasterList();
    static void cleanupAllMaterials();

    static uint32_t getDefaultMaterialIndex() { return quartz::rendering::Material::defaultMaterialIndex; }

public: // member functions
    Material();
    Material(
        const uint32_t baseColorTextureMasterIndex,
        const uint32_t normalTextureMasterIndex,
        const uint32_t emissiveTextureMasterIndex,
        const uint32_t metallicRoughnessTextureMasterIndex,
        const glm::vec3& baseColorFactor,
        const glm::vec3& emissiveFactor,
        const float metallicFactor,
        const float roughnessFactor
    );
    Material(const Material& other);
    Material(Material&& other);
    ~Material();

    USE_LOGGER(MATERIAL);

    uint32_t getBaseColorTextureMasterIndex() const { return m_baseColorTextureMasterIndex; }
    uint32_t getNormalTextureMasterIndex() const { return m_normalTextureMasterIndex; }
    uint32_t getEmissiveTextureMasterIndex() const { return m_emissiveTextureMasterIndex; }
    uint32_t getMetallicRoughnessTextureMasterIndex() const { return m_metallicRoughnessTextureMasterIndex; }

private: // static functions

private: // static variables
    static uint32_t defaultMaterialIndex;

    static std::vector<std::shared_ptr<Material>> masterList;

private: // member variables
    alignas(4) uint32_t m_baseColorTextureMasterIndex;
    alignas(4) uint32_t m_normalTextureMasterIndex;
    alignas(4) uint32_t m_emissiveTextureMasterIndex;
    alignas(4) uint32_t m_metallicRoughnessTextureMasterIndex;

    alignas(16) glm::vec3 m_baseColorFactor;
    alignas(16) glm::vec3 m_emissiveFactor;
    alignas(4) float m_metallicFactor;
    alignas(4) float m_roughnessFactor;
};