#pragma once

#include <string>

#include <glm/mat4x4.hpp>

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/scene/Loggers.hpp"

namespace quartz {
namespace scene {
    class Doodad;
}
}

class quartz::scene::Doodad {
public: // member functions
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        const std::string& objectFilepath,
        const std::string& textureFilepath
    );
    Doodad(Doodad&& other);
    ~Doodad();

    USE_LOGGER(DOODAD);

    const quartz::rendering::Model& getModel() const { return m_model; }

    void update(const float executionDurationTimeCount);

private: // static functions

private: // member variables
    quartz::rendering::Model m_model;

    glm::mat4 m_modelMatrix;
};