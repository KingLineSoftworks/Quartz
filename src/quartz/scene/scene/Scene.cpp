#include <string>
#include <vector>

#include <glm/gtx/string_cast.hpp>

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

std::vector<quartz::scene::Doodad>
quartz::scene::Scene::loadDoodads(
    const quartz::rendering::Device& renderingDevice,
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const std::vector<std::tuple<std::string, quartz::scene::Transform, std::optional<quartz::scene::PhysicsProperties>>>& doodadInformations
) {
    LOG_FUNCTION_SCOPE_TRACE(SCENE, "");

    std::vector<quartz::scene::Doodad> doodads;

    for (const std::tuple<std::string, quartz::scene::Transform, std::optional<quartz::scene::PhysicsProperties>>& doodadInformation : doodadInformations) {
        const std::string& filepath = std::get<0>(doodadInformation);
        const quartz::scene::Transform& transform = std::get<1>(doodadInformation);
        const std::optional<quartz::scene::PhysicsProperties>& o_physicsProperties = std::get<2>(doodadInformation);

        LOG_TRACE(SCENE, "Loading doodad with model from {} and transform:", filepath);
        LOG_TRACE(SCENE, "  position = {}", transform.position.toString());
        LOG_TRACE(SCENE, "  rotation = {}", transform.rotation.toString());
        LOG_TRACE(SCENE, "  scale    = {}", transform.scale.toString());

        doodads.emplace_back(
            renderingDevice,
            filepath,
            o_physicsProperties,
            transform,
            p_physicsWorld
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
    quartz::managers::PhysicsManager& physicsManager,
    const quartz::scene::Camera& camera,
    const quartz::scene::AmbientLight& ambientLight,
    const quartz::scene::DirectionalLight& directionalLight,
    const std::vector<quartz::scene::PointLight>& pointLights,
    const std::vector<quartz::scene::SpotLight>& spotLights,
    const math::Vec3& screenClearColor,
    const std::array<std::string, 6>& skyBoxInformation,
    const std::vector<std::tuple<std::string, quartz::scene::Transform, std::optional<quartz::scene::PhysicsProperties>>>& doodadInformations
) {
   LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis("Creating physics world");
    /**
     * @todo 2024/06/20 Create a class extending reactphysics3d::EventListener,
     *    instantiate an instance of that class, and set that instance as the
     *    physics world's event listener
     */
    reactphysics3d::PhysicsWorld::WorldSettings physicsWorldSettings;
    physicsWorldSettings.defaultVelocitySolverNbIterations = 10;
    physicsWorldSettings.defaultPositionSolverNbIterations = 5;
    physicsWorldSettings.isSleepingEnabled = true;
    physicsWorldSettings.defaultTimeBeforeSleep = 1.0;      // seconds
    physicsWorldSettings.defaultSleepLinearVelocity = 0.5;  // meters per second
    physicsWorldSettings.defaultSleepAngularVelocity = 0.5; // meters per second
    physicsWorldSettings.gravity = math::Vec3(0, -9.81, 0);
    mp_physicsWorld = physicsManager.createPhysicsWorldPtr(physicsWorldSettings);

    LOG_TRACEthis("Initializing master texture list");
    quartz::rendering::Texture::initializeMasterTextureList(renderingDevice);

    LOG_TRACEthis("Initializing master material list");
    quartz::rendering::Material::initializeMasterMaterialList(renderingDevice);

    m_camera = camera;
    LOG_TRACEthis("Loaded camera at position {}", m_camera.getWorldPosition().toString());

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
        mp_physicsWorld,
        doodadInformations
    );
    LOG_TRACEthis("Loaded {} doodads", m_doodads.size());

    m_ambientLight = ambientLight;
    LOG_TRACEthis("Loaded ambient light with color {}", m_ambientLight.color.toString());

    m_directionalLight = directionalLight;
    LOG_TRACEthis("Loaded directional light with color {} and direction {}", m_directionalLight.color.toString(), m_directionalLight.direction.toString());

    m_pointLights = pointLights;
    LOG_TRACEthis("Loaded {} point lights", m_pointLights.size());

    m_spotLights = spotLights;
    LOG_TRACEthis("Loaded {} spot lights", m_spotLights.size());

    m_screenClearColor = screenClearColor;
    LOG_TRACEthis("Loaded screen clear color {}", m_screenClearColor.toString());
}

void
quartz::scene::Scene::fixedUpdate(
    const quartz::managers::InputManager& inputManager,
    UNUSED const quartz::managers::PhysicsManager& physicsManager,
    UNUSED const double totalElapsedTime,
    const double tickTimeDelta
) {
    m_camera.fixedUpdate(inputManager);

    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.fixedUpdate();
    }

    mp_physicsWorld->update(tickTimeDelta);
}

void
quartz::scene::Scene::update(
    const quartz::rendering::Window& renderingWindow,
    const double frameTimeDelta,
    const double frameInterpolationFactor
) {
    m_camera.update(
        static_cast<float>(renderingWindow.getVulkanExtent().width),
        static_cast<float>(renderingWindow.getVulkanExtent().height),
        frameTimeDelta,
        frameInterpolationFactor
    );

    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.update(frameTimeDelta, frameInterpolationFactor);
    }
}