#pragma once

#include <string>
#include <vector>

#include "math/transform/Vec3.hpp"
#include "math/transform/Vec4.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"

namespace quartz {
namespace rendering {
    class Material;
}
}

/**
 * @brief Based on GLTF 2.0 materials specified <a href="https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#metallic-roughness-material">here</a>
 *
 * @brief Default values specified here https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#reference-material
 */
class quartz::rendering::Material {
public: // classes
    /**
     * @brief So we don't need to pass all of the information stored in the material (don't pass the name string)
     *
     * @todo 2024/06/04 Can we copy the material into the GPU's memory but only copy 72 bytes so we slice off the name variable?
     */
    struct UniformBufferObject {
    public: // member functions
        UniformBufferObject() = default;
        UniformBufferObject(
            const uint32_t baseColorTextureMasterIndex_,
            const uint32_t metallicRoughnessTextureMasterIndex_,
            const uint32_t normalTextureMasterIndex_,
            const uint32_t emissionTextureMasterIndex_,
            const uint32_t occlusionTextureMasterIndex_,
            const math::Vec4& baseColorFactor_,
            const math::Vec3& emissiveFactor_,
            const float metallicFactor_,
            const float roughnessFactor_,
            const uint32_t alphaMode_,
            const float alphaCutoff_,
            const bool doubleSided_
        );
        UniformBufferObject(const Material& material);

    public: // member variables
        alignas(4) uint32_t baseColorTextureMasterIndex;
        alignas(4) uint32_t metallicRoughnessTextureMasterIndex;
        alignas(4) uint32_t normalTextureMasterIndex;
        alignas(4) uint32_t emissionTextureMasterIndex;
        alignas(4) uint32_t occlusionTextureMasterIndex;

        alignas(16) math::Vec4 baseColorFactor;
        alignas(16) math::Vec3 emissiveFactor;
        alignas(4) float metallicFactor;
        alignas(4) float roughnessFactor;

        alignas(4) uint32_t alphaMode;
        alignas(4) float alphaCutoff;
        alignas(4) bool doubleSided;
    };

public: // enums
    enum class AlphaMode : uint32_t {
        Opaque  = 0,
        Mask    = 1,
        Blend   = 2
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
        const math::Vec4& baseColorFactor,
        const math::Vec3& emissiveFactor,
        const float metallicFactor,
        const float roughnessFactor,
        const quartz::rendering::Material::AlphaMode alphaMode,
        const float alphaCutoff,
        const bool doubleSided
    );
    static void initializeMasterMaterialList(const quartz::rendering::Device& renderingDevice);
    static void cleanUpAllMaterials();

    static uint32_t getDefaultMaterialMasterIndex() { return quartz::rendering::Material::defaultMaterialMasterIndex; }

    static uint32_t getNumCreatedMaterials() { return quartz::rendering::Material::masterMaterialList.size(); }
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
        const math::Vec4& baseColorFactor,
        const math::Vec3& emissiveFactor,
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

    const math::Vec4& getBaseColorFactor() const { return m_baseColorFactor; }
    const math::Vec3& getEmissiveFactor() const { return m_emissiveFactor; }
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
    alignas(16) math::Vec4 m_baseColorFactor;
    alignas(16) math::Vec3 m_emissiveFactor;
    alignas(4) float m_metallicFactor;
    alignas(4) float m_roughnessFactor;

    // 12 bytes of other
    alignas(4) quartz::rendering::Material::AlphaMode m_alphaMode;
    alignas(4) float m_alphaCutoff;
    alignas(4) bool m_doubleSided;

    // name
    std::string m_name;
};