#pragma once

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class Material;
}
}

/**
 * @brief Based on GLTF 2.0 materials specified <a href="https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material">here</a>
 */
class quartz::rendering::Material {
public: // enums
    enum class AlphaMode : uint32_t {
        Opaque = 0,
        Mask = 1,
        Blend = 2
    };

// -----+++++===== Static Interface =====+++++----- //

public: // static functions
    static std::string getAlphaModeGLTFString(const quartz::rendering::Material::AlphaMode mode);
    static quartz::rendering::Material::AlphaMode getAlphaModeFromGLTFString(const std::string& modeString);

    static uint32_t createMaterial(
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
    );
    static void initializeMasterMaterialList(const quartz::rendering::Device& renderingDevice);
    static void cleanUpAllMaterials();

    static uint32_t getDefaultMaterialMasterIndex() { return quartz::rendering::Material::defaultMaterialMasterIndex; }

    static std::shared_ptr<quartz::rendering::Material> getMaterialPtr(const uint32_t index) { return quartz::rendering::Material::masterMaterialList[index]; }
    static const std::vector<std::shared_ptr<quartz::rendering::Material>>& getMasterMaterialList() { return quartz::rendering::Material::masterMaterialList; }

private: // static functions

private: // static variables
    static uint32_t defaultMaterialMasterIndex;
    static std::vector<std::shared_ptr<Material>> masterMaterialList;

// -----+++++===== Instance Interface =====+++++----- //

public: // member functions
    Material();
    Material(
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
    );
    Material(const Material& other);
    Material(Material&& other);
    ~Material();

    Material& operator=(const Material& other);

    USE_LOGGER(MATERIAL);

    uint32_t getBaseColorTextureMasterIndex() const { return m_baseColorTextureMasterIndex; }
    uint32_t getMetallicRoughnessTextureMasterIndex() const { return m_metallicRoughnessTextureMasterIndex; }
    uint32_t getNormalTextureMasterIndex() const { return m_normalTextureMasterIndex; }
    uint32_t getEmissionTextureMasterIndex() const { return m_emissionTextureMasterIndex; }
    uint32_t getOcclusionTextureMasterIndex() const { return m_occlusionTextureMasterIndex; }

    const glm::vec4& getBaseColorFactor() const { return m_baseColorFactor; }
    const glm::vec3& getEmissiveFactor() const { return m_emissiveFactor; }
    float getMetallicFactor() const { return m_metallicFactor; }
    float getRoughnessFactor() const { return m_roughnessFactor; }

    quartz::rendering::Material::AlphaMode getAlphaMode() const { return m_alphaMode; }
    float getAlphaCutoff() const { return m_alphaCutoff; }
    float getDoubleSided() const { return m_doubleSided; }

    const std::string& getName() const { return m_name; }

private: // member variables
    // 20 bytes of texture indices
    alignas(4) uint32_t m_baseColorTextureMasterIndex;
    alignas(4) uint32_t m_metallicRoughnessTextureMasterIndex;
    alignas(4) uint32_t m_normalTextureMasterIndex;
    alignas(4) uint32_t m_emissionTextureMasterIndex;
    alignas(4) uint32_t m_occlusionTextureMasterIndex;

    // 40 bytes of factors
    alignas(16) glm::vec4 m_baseColorFactor;
    alignas(16) glm::vec3 m_emissiveFactor;
    alignas(4) float m_metallicFactor;
    alignas(4) float m_roughnessFactor;

    // 12 bytes of other
    alignas(4) quartz::rendering::Material::AlphaMode m_alphaMode;
    alignas(4) float m_alphaCutoff;
    alignas(4) bool m_doubleSided;

    // name
    std::string m_name;
};