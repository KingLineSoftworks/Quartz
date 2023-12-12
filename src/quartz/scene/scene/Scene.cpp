#include <string>
#include <vector>

#include <glm/gtx/string_cast.hpp>

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

std::vector<quartz::scene::Doodad>
quartz::scene::Scene::loadDoodads(
    const quartz::rendering::Device& renderingDevice,
    const std::vector<std::pair<std::string, quartz::scene::Transform>>& doodadInformations
) {
    LOG_FUNCTION_SCOPE_TRACE(SCENE, "");

    std::vector<quartz::scene::Doodad> doodads;

    for (const std::pair<std::string, quartz::scene::Transform>& doodadInformation : doodadInformations) {
        const std::string& filepath = doodadInformation.first;
        const quartz::scene::Transform& transform = doodadInformation.second;

        LOG_TRACE(SCENE, "Loading doodad with model from {} and transform:", filepath);
        LOG_TRACE(SCENE, "  position         = {}", glm::to_string(transform.position));
        LOG_TRACE(SCENE, "  rotation degrees = {}", transform.rotationAmountDegrees);
        LOG_TRACE(SCENE, "  rotation axis    = {}", glm::to_string(transform.rotationAxis));
        LOG_TRACE(SCENE, "  scale            = {}", glm::to_string(transform.scale));

        doodads.emplace_back(
            renderingDevice,
            filepath,
            transform
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
    LOG_TRACEthis("Cleaning up all textures");
    quartz::rendering::Texture::cleanUpAllTextures();
}

void
quartz::scene::Scene::load(
    const quartz::rendering::Device& renderingDevice,
    const quartz::scene::Camera& camera,
    const quartz::scene::AmbientLight& ambientLight,
    const quartz::scene::DirectionalLight& directionalLight,
    const std::vector<std::pair<std::string, quartz::scene::Transform>>& doodadInformations
) {
   LOG_FUNCTION_SCOPE_TRACEthis("");

    m_camera = camera;

    m_doodads = quartz::scene::Scene::loadDoodads(
        renderingDevice,
        doodadInformations
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