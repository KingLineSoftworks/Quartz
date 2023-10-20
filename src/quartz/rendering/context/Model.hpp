#pragma once

#include <vector>

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
        const std::string& textureFilepath
    );
    Model(Model&& other);
    ~Model();

    USE_LOGGER(MODEL);

    const quartz::rendering::Mesh& getMesh() const { return m_mesh; }
    const quartz::rendering::Texture& getTexture() const { return m_texture; }

private: // static functions

private: // member variables
    quartz::rendering::Mesh m_mesh;
    quartz::rendering::Texture m_texture;
};