#include <vector>

#include "util/file_system/FileSystem.hpp"

#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

#include "demo_app/SceneParameters.hpp"
#include "demo_app/player/Player.hpp"

quartz::scene::Scene::Parameters
createDemoLevelSceneParameters(
    Player& player
) {
    std::vector<quartz::scene::Doodad::Parameters> doodadParameters = {
        {
            util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb"),
            {
                { 5.0f, 7.5f, 5.0f },
                0.0f,
                { 0.0f, 0.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }
            },
            {{
                reactphysics3d::BodyType::DYNAMIC,
                true,
                math::Vec3(0.0, 1.0, 0.0),
                quartz::physics::BoxShape::Parameters({1.0f, 1.0f, 1.0f})
            }},
            [&player] (quartz::scene::Doodad::AwakenCallbackParameters parameters) { player.awakenCallback(parameters); },
            [&player] (quartz::scene::Doodad::FixedUpdateCallbackParameters parameters) { player.fixedUpdateCallback(parameters); },
            [&player] (quartz::scene::Doodad::UpdateCallbackParameters parameters) { player.updateCallback(parameters); }
        },
        {
            util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/models/unit_models/unit_sphere/glb/unit_sphere.glb"),
            {
                { 5.0f, 10.0f, 0.0f },
                0.0f,
                { 0.0f, 0.0f, 1.0f },
                { 1.0f, 1.0f, 1.0f }
            },
            {{
                reactphysics3d::BodyType::DYNAMIC,
                true,
                math::Vec3(0.0, 1.0, 0.0),
                quartz::physics::SphereShape::Parameters(1.0)
            }},
            {},
            {},
            {}
        },
        {
            util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/models/glTF-Sample-Models/2.0/Cube/glTF/Cube.gltf"),
            {
                {0.0f, -5.0f, 0.0f},
                0.0f,
                {0.0f, 1.0f, 0.0f},
                {25.0f, 1.0f, 25.0f}
            },
            {{
                reactphysics3d::BodyType::STATIC,
                false,
                math::Vec3(1.0, 1.0, 1.0),
                quartz::physics::BoxShape::Parameters({25.0f, 1.0f, 25.0f})
            }},
            {},
            {},
            {}
        },
    };

    quartz::scene::AmbientLight ambientLight({ 0.1f, 0.1f, 0.1f });

    quartz::scene::DirectionalLight directionalLight({ 0.5f, 0.5f, 0.5f }, { 3.0f, -2.0f, 2.0f });

    std::vector<quartz::scene::PointLight> pointLights = {};

    std::vector<quartz::scene::SpotLight> spotLights = {};

    math::Vec3 screenClearColor = { 0.25f, 0.4f, 0.6f };
    
    std::array<std::string, 6> skyBoxInformation = {
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/posx.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/negx.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/posy.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/negy.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/posz.jpg"),
        util::FileSystem::getAbsoluteFilepathInProjectDirectory("assets/sky_boxes/parliament/negz.jpg")
    };

    std::optional<quartz::physics::Field::Parameters> o_fieldParameters({{0.0, -1.0, 0.0}});

    return { 
        "default_test_scene_00",
        ambientLight,
        directionalLight,
        pointLights,
        spotLights,
        screenClearColor,
        skyBoxInformation,
        doodadParameters,
        o_fieldParameters
    };
}

