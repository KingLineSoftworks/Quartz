#include <string>
#include <vector>

#include <glm/gtx/string_cast.hpp>

#include "util/logger/Logger.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

quartz::scene::Camera quartz::scene::Scene::defaultCamera;

std::vector<quartz::scene::Doodad>
quartz::scene::Scene::constructDoodads(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    std::optional<quartz::physics::Field>& o_field,
    const std::vector<quartz::scene::Doodad::Parameters>& doodadParameters
) {
    LOG_FUNCTION_SCOPE_TRACE(SCENE, "");

    std::vector<quartz::scene::Doodad> doodads;

    for (const quartz::scene::Doodad::Parameters& parameters : doodadParameters) {
        const std::optional<std::string>& o_filepath = parameters.o_objectFilepath;
        const math::Transform& transform = parameters.transform;
        const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyInformation = parameters.o_rigidBodyParameters;
        const quartz::scene::Doodad::AwakenCallback awakenCallback = parameters.awakenCallback;
        const quartz::scene::Doodad::FixedUpdateCallback fixedUpdateCallback = parameters.fixedUpdateCallback;
        const quartz::scene::Doodad::UpdateCallback updateCallback = parameters.updateCallback;

        LOG_TRACE(SCENE, "Loading doodad with information:");
        LOG_TRACE(SCENE, "  model: {}", o_filepath ? *o_filepath : "none");
        LOG_TRACE(SCENE, "  transform:");
        LOG_TRACE(SCENE, "    position = {}", transform.position.toString());
        LOG_TRACE(SCENE, "    rotation = {}", transform.rotation.toString());
        LOG_TRACE(SCENE, "    scale    = {}", transform.scale.toString());
        LOG_TRACE(SCENE, "  rigid body properties:");
        if (o_rigidBodyInformation) {
            LOG_TRACE(SCENE, "    body type       = {}", quartz::physics::RigidBody::getBodyTypeString(o_rigidBodyInformation->bodyType));
            LOG_TRACE(SCENE, "    gravity enabled = {}", o_rigidBodyInformation->enableGravity);
        } else {
            LOG_TRACE(SCENE, "    no rigid body");
        }

        doodads.emplace_back(
            renderingDevice,
            physicsManager,
            o_field,
            o_filepath,
            transform,
            o_rigidBodyInformation,
            awakenCallback,
            fixedUpdateCallback,
            updateCallback
        );
    }

    LOG_TRACE(SCENE, "Loaded {} doodads", doodads.size());

    return doodads;
}

quartz::scene::Scene::Scene() :
    mo_field(),
    mr_camera(quartz::scene::Scene::defaultCamera),
    m_doodads(),
    m_skyBox(),
    m_ambientLight(),
    m_directionalLight(),
    m_pointLights(),
    m_spotLights(),
    m_screenClearColor()
{}

quartz::scene::Scene::Scene(
    quartz::scene::Scene&& other
) :
    mo_field(std::move(other.mo_field)),
    mr_camera(other.mr_camera), // don't need to move a reference
    m_doodads(std::move(other.m_doodads)),
    m_skyBox(std::move(other.m_skyBox)),
    m_ambientLight(std::move(other.m_ambientLight)),
    m_directionalLight(std::move(other.m_directionalLight)),
    m_pointLights(std::move(other.m_pointLights)),
    m_spotLights(std::move(other.m_spotLights)),
    m_screenClearColor(std::move(other.m_screenClearColor))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Scene::~Scene() {
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Cleaning up all textures");
    quartz::rendering::Texture::cleanUpAllTextures();
}

void
quartz::scene::Scene::setCamera(
    quartz::scene::Camera& camera
) {
    mr_camera = camera;
}

void
quartz::scene::Scene::load(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    const quartz::scene::AmbientLight& ambientLight,
    const quartz::scene::DirectionalLight& directionalLight,
    const std::vector<quartz::scene::PointLight>& pointLights,
    const std::vector<quartz::scene::SpotLight>& spotLights,
    const math::Vec3& screenClearColor,
    const std::array<std::string, 6>& skyBoxInformation,
    const std::vector<quartz::scene::Doodad::Parameters>& doodadParameters,
    const std::optional<quartz::physics::Field::Parameters>& o_fieldParameters
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    /**
     * @todo 2024/11/09 Create a quartz::physics::EventListener class that allows for the
     *    ability to inject functions, so the user doesn't have to implement their own class
     *    extending the reactphysics3d::EventListener class.
     *    We can allow the current scene to set the event listener's functions, so each scene
     *    can handle the events in their own way.
     */
    // mp_physicsWorld->setEventListener(&el);
    
    if (o_fieldParameters) {
        LOG_TRACEthis("Initializing physics field");
        mo_field.emplace(physicsManager.createField(*o_fieldParameters));
    }

    LOG_TRACEthis("Initializing master texture list");
    quartz::rendering::Texture::initializeMasterTextureList(renderingDevice);

    LOG_TRACEthis("Initializing master material list");
    quartz::rendering::Material::initializeMasterMaterialList(renderingDevice);

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

    m_doodads = quartz::scene::Scene::constructDoodads(
        renderingDevice,
        physicsManager,
        mo_field,
        doodadParameters
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

    LOG_DEBUGthis("Camera {} with position {}", mr_camera.get().getId(), mr_camera.get().getWorldPosition().toString());
    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.awaken(this);
    }
    LOG_TRACEthis("Awoke all doodads");
    LOG_DEBUGthis("Camera {} with position {}", mr_camera.get().getId(), mr_camera.get().getWorldPosition().toString());
}

void
quartz::scene::Scene::load(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    const quartz::scene::Scene::Parameters& sceneParameters
) {
    load(
        renderingDevice,
        physicsManager,
        sceneParameters.ambientLight,
        sceneParameters.directionalLight,
        sceneParameters.pointLights,
        sceneParameters.spotLights,
        sceneParameters.screenClearColor,
        sceneParameters.skyBoxInformation,
        sceneParameters.doodadParameters,
        sceneParameters.o_fieldParameters
    );
}

void
quartz::scene::Scene::unload(
    quartz::managers::PhysicsManager& physicsManager
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    if (!mo_field) {
        LOG_TRACEthis("Not unloading physics items");
        return;
    }
    LOG_TRACEthis("Unloading physics items");

    LOG_TRACEthis("Unloading {} doodads", m_doodads.size());
    for (uint32_t i = 0; i < m_doodads.size(); i++) {
        LOG_TRACEthis("Unloading doodad {}", i);

        quartz::scene::Doodad& doodad = m_doodads[i];

        if (!doodad.getRigidBodyOptionalReference()) {
            LOG_TRACEthis("  doodad {} has no rigidbody", i);
            continue;
        }

        LOG_TRACEthis("  destroying rigidbody");
        physicsManager.destroyRigidBody(*mo_field, *doodad.getRigidBodyOptionalReference());
    }

    physicsManager.destroyField(*mo_field);
}

void
quartz::scene::Scene::fixedUpdate(
    const quartz::managers::InputManager& inputManager,
    UNUSED const quartz::managers::PhysicsManager& physicsManager,
    const double totalElapsedTime,
    const double tickTimeDelta
) {
    // The doodad's fixedUpdate will make changes to the rigidBody and its transform, so
    // there is no need to manually snap the rigidBody to the doodad
    const double ticksPerSecond = 1.0 / tickTimeDelta;
    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.fixedUpdate(inputManager, totalElapsedTime, tickTimeDelta, ticksPerSecond);
    }

    // This is only going to update the rigid bodies, not the doodads
    if (mo_field) {
        mo_field->fixedUpdate(tickTimeDelta);
    }

    // We want to move the doodad to the rigid body's new transform after it got updated by
    // the physics field
    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.snapToRigidBody();
    }
}

void
quartz::scene::Scene::update(
    UNUSED const quartz::rendering::Window& renderingWindow,
    const quartz::managers::InputManager& inputManager,
    const double totalElapsedTime,
    const double frameTimeDelta,
    const double frameInterpolationFactor
) {
    for (quartz::scene::Doodad& doodad : m_doodads) {
        doodad.update(
            inputManager,
            totalElapsedTime,
            frameTimeDelta,
            frameInterpolationFactor
        );
    }

    mr_camera.get().update(
        static_cast<float>(renderingWindow.getVulkanExtent().width),
        static_cast<float>(renderingWindow.getVulkanExtent().height),
        frameTimeDelta,
        frameInterpolationFactor
    );

    /**
     * @todo 2024/12/01 Snap the rigid body to the doodad's position. We are not going
     *    to be doing any physics updates until the next fixedUpdate call, so we don't need
     *    to worry about it having any physics implications.
     *    If things get weird as a result of this, that is the fault of the client. Don't be
     *    tampering with physics stuff outside of fixedUpdate. If you don't care about physics
     *    or if you don't have a rigid body, then feel free to do whatever you want here
     */
}

