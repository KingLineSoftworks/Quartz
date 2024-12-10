#pragma once

#include <string>
#include <vector>

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Vec3.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/window/Window.hpp"
#include "quartz/scene/Loggers.hpp"
#include "quartz/scene/camera/Camera.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
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
public: // classes
    struct Parameters {
        Parameters(
            const std::string& name_,
            const quartz::scene::Camera& camera_,
            const quartz::scene::AmbientLight& ambientLight_,
            const quartz::scene::DirectionalLight& directionalLight_,
            const std::vector<quartz::scene::PointLight>& pointLights_,
            const std::vector<quartz::scene::SpotLight>& spotLights_,
            const math::Vec3& screenClearColor_,
            const std::array<std::string, 6>& skyBoxInformation_,
            const std::vector<quartz::scene::Doodad::Parameters>& doodadParameters_,
            const std::optional<quartz::physics::Field::Parameters>& o_fieldParameters_
        ) :
            name(name_),
            camera(camera_),
            ambientLight(ambientLight_),
            directionalLight(directionalLight_),
            pointLights(pointLights_),
            spotLights(spotLights_),
            screenClearColor(screenClearColor_),
            skyBoxInformation(skyBoxInformation_),
            doodadParameters(doodadParameters_),
            o_fieldParameters(o_fieldParameters_)
        {}

        std::string name;
        quartz::scene::Camera camera;
        quartz::scene::AmbientLight ambientLight;
        quartz::scene::DirectionalLight directionalLight;
        std::vector<quartz::scene::PointLight> pointLights;
        std::vector<quartz::scene::SpotLight> spotLights;
        math::Vec3 screenClearColor;
        std::array<std::string, 6> skyBoxInformation;
        std::vector<quartz::scene::Doodad::Parameters> doodadParameters;
        std::optional<quartz::physics::Field::Parameters> o_fieldParameters;
    };

public: // member functions
    Scene() = default;
    Scene(Scene&& other);
    ~Scene();

    USE_LOGGER(SCENE);

    const quartz::scene::Camera& getCamera() const { return m_camera; }
    const std::vector<quartz::scene::Doodad>& getDoodads() const { return m_doodads; }
    const quartz::scene::SkyBox& getSkyBox() const { return m_skyBox; }
    const quartz::scene::AmbientLight& getAmbientLight() const { return m_ambientLight; }
    const quartz::scene::DirectionalLight& getDirectionalLight() const { return m_directionalLight; }
    const std::vector<quartz::scene::PointLight>& getPointLights() const { return m_pointLights; }
    const std::vector<quartz::scene::SpotLight>& getSpotLights() const { return m_spotLights; }
    const math::Vec3& getScreenClearColor() const { return m_screenClearColor; }

    /**
     * @todo 2024/11/26 We should have a way to create a scene without rendering information for testing
     *    purposes. See test_doodad_rigidbody_collider_transforms.cpp to see that we need to duplicate
     *    scene functionality for testing purposes.
     */
    void load(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        const quartz::scene::Camera& camera,
        const quartz::scene::AmbientLight& ambientLight,
        const quartz::scene::DirectionalLight& directionalLight,
        const std::vector<quartz::scene::PointLight>& pointLights,
        const std::vector<quartz::scene::SpotLight>& spotLights,
        const math::Vec3& screenClearColor,
        const std::array<std::string, 6>& skyBoxInformation,
        const std::vector<quartz::scene::Doodad::Parameters>& doodadParameters,
        const std::optional<quartz::physics::Field::Parameters>& o_fieldParameters
    );
    void load(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        const quartz::scene::Scene::Parameters& sceneParameters
    );

    void fixedUpdate(
        const quartz::managers::InputManager& inputManager,
        const quartz::managers::PhysicsManager& physicsManager,
        const double totalElapsedTime,
        const double tickTimeDelta
    );
    void update(
        const quartz::rendering::Window& renderingWindow,
        const quartz::managers::InputManager& inputManager,
        const double totalElapsedTime,
        const double frameTimeDelta,
        const double frameInterpolationFactor
    );

private: // static functions
    static std::vector<quartz::scene::Doodad> constructDoodads(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        std::optional<quartz::physics::Field>& o_field,
        const std::vector<quartz::scene::Doodad::Parameters>& doodadParameters
    );

private: // member variables
    std::optional<quartz::physics::Field> mo_field; // optional because we can have scenes without physics (main menu, etc.)

    quartz::scene::Camera m_camera;

    std::vector<quartz::scene::Doodad> m_doodads;

    quartz::scene::SkyBox m_skyBox;
    quartz::scene::AmbientLight m_ambientLight;
    quartz::scene::DirectionalLight m_directionalLight;
    std::vector<quartz::scene::PointLight> m_pointLights;
    std::vector<quartz::scene::SpotLight> m_spotLights;

    math::Vec3 m_screenClearColor;
};
