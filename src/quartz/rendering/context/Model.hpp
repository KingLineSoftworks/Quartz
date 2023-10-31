#pragma once

#include <vector>

#include <tiny_obj_loader.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Mesh.hpp"
#include "quartz/rendering/context/Texture.hpp"

namespace quartz {
namespace rendering {
    class Model;
}
}

class quartz::rendering::Model {
public: // member functions
    Model(
        const quartz::rendering::Device& renderingDevice,
        const std::string& objectFilePath,
        const std::string& textureFilepath
    );
    Model(Model&& other);
    ~Model();

    USE_LOGGER(MODEL);

    const quartz::rendering::Mesh& getMesh() const { return m_mesh; }
    const quartz::rendering::Texture& getTexture() const { return m_texture; }

private: // static functions
    static bool loadModel(
        const std::string& filepath,
        tinyobj::attrib_t& tinyobjAttribute,
        std::vector<tinyobj::shape_t>& tinyobjShapes,
        std::vector<tinyobj::material_t>& tinyobjMaterials
    );

private: // member variables
    tinyobj::attrib_t m_tinyobjAttribute;
    std::vector<tinyobj::shape_t> m_tinyobjShapes;
    std::vector<tinyobj::material_t> m_tinyobjMaterials;
    bool m_loadedSuccessfully;

    quartz::rendering::Mesh m_mesh;
    quartz::rendering::Texture m_texture;
};