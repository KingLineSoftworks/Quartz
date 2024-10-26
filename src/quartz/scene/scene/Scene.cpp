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
    const std::vector<quartz::scene::PointLight>& pointLights,
    const std::vector<quartz::scene::SpotLight>& spotLights,
    const glm::vec3& screenClearColor,
    const std::array<std::string, 6>& skyBoxInformation,
    const std::vector<std::pair<std::string, quartz::scene::Transform>>& doodadInformations
) {
   LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis("Initializing master texture list");
    quartz::rendering::Texture::initializeMasterTextureList(renderingDevice);

    LOG_TRACEthis("Initializing master material list");
    quartz::rendering::Material::initializeMasterMaterialList(renderingDevice);

    m_camera = camera;
    LOG_TRACEthis("Loaded camera at position {}", glm::to_string(m_camera.getWorldPosition()));

    m_skyBox = quartz::scene::SkyBox(
        renderingDevice,
        skyBoxInformation[0],
        skyBoxInformation[1],
        skyBoxInformation[2],
        skyBoxInformation[3],
        skyBoxInformation[4],
        skyBoxInformation[5]
    );
    LOG_TRACEthis("Loaded skybox");

    m_doodads = quartz::scene::Scene::loadDoodads(
        renderingDevice,
        doodadInformations
    );
    LOG_TRACEthis("Loaded {} doodads", m_doodads.size());

    m_ambientLight = ambientLight;
    LOG_TRACEthis("Loaded ambient light with color {}", glm::to_string(m_ambientLight.color));

    m_directionalLight = directionalLight;
    LOG_TRACEthis("Loaded directional light with color {} and direction {}", glm::to_string(m_directionalLight.color), glm::to_string(m_directionalLight.direction));

    m_pointLights = pointLights;
    LOG_TRACEthis("Loaded {} point lights", m_pointLights.size());

    m_spotLights = spotLights;
    LOG_TRACEthis("Loaded {} spot lights", m_spotLights.size());

    m_screenClearColor = screenClearColor;
    LOG_TRACEthis("Loaded screen clear color {}", glm::to_string(m_screenClearColor));
}

void
quartz::scene::Scene::update(
    const quartz::rendering::Window& renderingWindow,
    const quartz::managers::InputManager& inputManager,
    const double tickTimeDelta
) {
    m_camera.update(
        static_cast<float>(renderingWindow.getVulkanExtent().width),
        static_cast<float>(renderingWindow.getVulkanExtent().height),
        inputManager,
        tickTimeDelta
    );

    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.update(tickTimeDelta);
    }
}