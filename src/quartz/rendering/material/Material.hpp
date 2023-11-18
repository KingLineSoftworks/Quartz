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
        const int32_t baseColorTextureMasterIndex,
        const int32_t normalTextureMasterIndex,
        const int32_t emissionTextureMasterIndex,
        const int32_t metallicRoughnessTextureMasterIndex
    );
    Material(Material&& other);
    ~Material();

    USE_LOGGER(MATERIAL);

    int32_t getBaseColorTextureMasterIndex() const { return m_baseColorTextureMasterIndex; }
    int32_t getNormalTextureMasterIndex() const { return m_normalTextureMasterIndex; }
    int32_t getEmissionTextureMasterIndex() const { return m_emissionTextureMasterIndex; }
    int32_t getMetallicRoughnessTextureMasterIndex() const { return m_metallicRoughnessTextureMasterIndex; }

private: // static functions

private: // member variables
    int32_t m_baseColorTextureMasterIndex;
    int32_t m_normalTextureMasterIndex;
    int32_t m_emissionTextureMasterIndex;
    int32_t m_metallicRoughnessTextureMasterIndex;
};