#pragma once

#include <string>
#include <utility>
#include <vector>

#include <glm/vec3.hpp>

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/Loggers.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/doodad/Transform.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"
#include "quartz/scene/light/PointLight.hpp"
#include "quartz/scene/light/SpotLight.hpp"
#include "quartz/scene/sky_box/SkyBox.hpp"

namespace quartz {
namespace scene {
    class Scene;
}
}

class quartz::scene::Scene {
public: // member functions
    Scene() = default;
    ~Scene();

    USE_LOGGER(SCENE);

    const quartz::scene::Camera& getCamera() const { return m_camera; }
    const std::vector<quartz::scene::Doodad>& getDoodads() const { return m_doodads; }
    const quartz::scene::SkyBox& getSkyBox() const { return m_skybox; }
    const quartz::scene::AmbientLight& getAmbientLight() const { return m_ambientLight; }
    const quartz::scene::DirectionalLight& getDirectionalLight() const { return m_directionalLight; }
    const std::vector<quartz::scene::PointLight>& getPointLights() const { return m_pointLights; }
    const std::vector<quartz::scene::SpotLight>& getSpotLights() const { return m_spotLights; }
    const glm::vec3& getScreenClearColor() const { return m_screenClearColor; }

    void load(
        const quartz::rendering::Device& renderingDevice,
        const quartz::scene::Camera& camera,
        const quartz::scene::AmbientLight& ambientLight,
        const quartz::scene::DirectionalLight& directionalLight,
        const std::vector<quartz::scene::PointLight>& pointLights,
        const std::vector<quartz::scene::SpotLight>& spotLights,
        const glm::vec3& screenClearColor,
        const std::array<std::string, 6>& skyboxInformation,
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

    quartz::scene::SkyBox m_skybox;
    quartz::scene::AmbientLight m_ambientLight;
    quartz::scene::DirectionalLight m_directionalLight;
    std::vector<quartz::scene::PointLight> m_pointLights;
    std::vector<quartz::scene::SpotLight> m_spotLights;

    glm::vec3 m_screenClearColor;
};