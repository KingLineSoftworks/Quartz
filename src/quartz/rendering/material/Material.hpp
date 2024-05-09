#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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
public: // enums
    enum class AlphaMode {
        Opaque = 0,
        Mask = 1,
        Blend = 2
    };

public: // static functions
    static std::string getAlphaModeGLTFString(const quartz::rendering::Material::AlphaMode mode);
    static quartz::rendering::Material::AlphaMode getAlphaModeFromGLTFString(const std::string& modeString);

public: // member functions
    Material();
    Material(
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
    );
    Material(const Material& other);
    Material(Material&& other);
    ~Material();

    USE_LOGGER(MATERIAL);

    uint32_t getBaseColorTextureMasterIndex() const { return m_baseColorTextureMasterIndex; }
    uint32_t getMetallicRoughnessTextureMasterIndex() const { return m_metallicRoughnessTextureMasterIndex; }
    uint32_t getNormalTextureMasterIndex() const { return m_normalTextureMasterIndex; }
    uint32_t getEmissionTextureMasterIndex() const { return m_emissionTextureMasterIndex; }
    uint32_t getOcclusionTextureMasterIndex() const { return m_occlusionTextureMasterIndex; }

private: // static functions

private: // member variables
    uint32_t m_baseColorTextureMasterIndex;
    uint32_t m_metallicRoughnessTextureMasterIndex;
    uint32_t m_normalTextureMasterIndex;
    uint32_t m_emissionTextureMasterIndex;
    uint32_t m_occlusionTextureMasterIndex;

    glm::vec4 m_baseColorFactor;
    glm::vec3 m_emissiveFactor;
    float m_metallicFactor;
    float m_roughnessFactor;

    quartz::rendering::Material::AlphaMode m_alphaMode;
    double m_alphaCutoff;
    bool m_doubleSided;
};