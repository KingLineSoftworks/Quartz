#pragma once

#include <string>
#include <utility>
#include <vector>

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/Loggers.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/doodad/Transform.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"

namespace quartz {
namespace scene {
    class Scene;
}
}

class quartz::scene::Scene {
public: // member functions
    Scene();
    ~Scene();

    USE_LOGGER(SCENE);

    const quartz::scene::Camera& getCamera() const { return m_camera; }
    const std::vector<quartz::scene::Doodad>& getDoodads() const { return m_doodads; }
    const quartz::scene::AmbientLight& getAmbientLight() const { return m_ambientLight; }
    const quartz::scene::DirectionalLight& getDirectionalLight() const { return m_directionalLight; }

    void load(
        const quartz::rendering::Device& renderingDevice,
        const quartz::scene::Camera& camera,
        const quartz::scene::AmbientLight& ambientLight,
        const quartz::scene::DirectionalLight& directionalLight,
        const std::vector<std::pair<std::string, quartz::scene::Transform>>& doodadInformations
    );

    void update(
        const quartz::rendering::Window& renderingWindow,
        const std::shared_ptr<quartz::managers::InputManager>& p_inputManager,
        const double tickTimeDelta
    );

private: // static functions
    static std::vector<quartz::scene::Doodad> loadDoodads(
        const quartz::rendering::Device& renderingDevice,
        const std::vector<std::pair<std::string, quartz::scene::Transform>>& doodadInformations
    );

private: // member variables
    quartz::scene::Camera m_camera;
    std::vector<quartz::scene::Doodad> m_doodads;
    quartz::scene::AmbientLight m_ambientLight;
    quartz::scene::DirectionalLight m_directionalLight;
};