#include <string>
#include <vector>

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

std::vector<quartz::scene::Doodad>
quartz::scene::Scene::loadDoodads(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<std::string>& filepaths
) {
    LOG_FUNCTION_SCOPE_TRACE(SCENE, "");

    std::vector<quartz::scene::Doodad> doodads;

    for (const std::string& filepath : filepaths) {
        doodads.emplace_back(
            renderingDevice,
            filepath,
            glm::vec3{0.0f, 0.0f, 0.0f}
        );
    }


    LOG_TRACE(SCENE, "Loaded {} doodads", doodads.size());

    return doodads;
}

quartz::scene::Scene::Scene() :
    m_camera(),
    m_doodads()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Scene::~Scene() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Scene::load(
    const quartz::rendering::Device& renderingDevice,
    const quartz::scene::Camera& camera,
    const quartz::scene::AmbientLight& ambientLight,
    const quartz::scene::DirectionalLight& directionalLight,
    const std::vector<std::string>& filepaths
) {
   LOG_FUNCTION_SCOPE_TRACEthis("");

    m_camera = camera;

    m_doodads = quartz::scene::Scene::loadDoodads(
        renderingDevice,
        filepaths
    );

    m_ambientLight = ambientLight;

    m_directionalLight = directionalLight;
}

void
quartz::scene::Scene::update(
    const quartz::rendering::Window& renderingWindow,
    const std::shared_ptr<quartz::managers::InputManager>& p_inputManager,
    const double tickTimeDelta
) {
    m_camera.update(
        static_cast<float>(renderingWindow.getVulkanExtent().width),
        static_cast<float>(renderingWindow.getVulkanExtent().height),
        p_inputManager,
        tickTimeDelta
    );

    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.update(tickTimeDelta);
    }
}