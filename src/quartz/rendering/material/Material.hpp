#pragma once

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
public: // member functions
    Material();
    Material(
        const uint32_t baseColorTextureMasterIndex,
        const uint32_t normalTextureMasterIndex,
        const uint32_t emissiveTextureMasterIndex,
        const uint32_t metallicRoughnessTextureMasterIndex
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

private: // member variables
    uint32_t m_baseColorTextureMasterIndex;
    uint32_t m_normalTextureMasterIndex;
    uint32_t m_emissiveTextureMasterIndex;
    uint32_t m_metallicRoughnessTextureMasterIndex;
};