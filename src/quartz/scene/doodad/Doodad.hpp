#pragma once

#include <string>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/scene/Loggers.hpp"
#include "quartz/scene/doodad/Transform.hpp"

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
        const quartz::scene::Transform& transform
    );
    Doodad(Doodad&& other);
    ~Doodad();

    USE_LOGGER(DOODAD);

    const quartz::rendering::Model& getModel() const { return m_model; }
    const glm::mat4& getTransformationMatrix() const { return m_transformationMatrix; }

    void update(const double tickTimeDelta);

private: // static functions

private: // member variables
    quartz::rendering::Model m_model;

    quartz::scene::Transform m_transform;

    glm::mat4 m_transformationMatrix;
};