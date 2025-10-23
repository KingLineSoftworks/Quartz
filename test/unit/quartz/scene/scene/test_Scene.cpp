#include <optional>

#include "util/unit_test/UnitTest.hpp"
#include "util/file_system/FileSystem.hpp"

#include "math/transform/Transform.hpp"

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/scene/scene/Scene.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/light/AmbientLight.hpp"
#include "quartz/scene/light/DirectionalLight.hpp"

namespace quartz {
namespace unit_test {

class PhysicsManagerUnitTestClient {
public:
    static quartz::managers::PhysicsManager& getInstance() {
        return quartz::managers::PhysicsManager::Client::getInstance();
    }

    static quartz::physics::Field createField() {
        return quartz::managers::PhysicsManager::Client::getInstance().createField({math::Vec3(0, -9.81, 0)});
    }

    static void destroyField(quartz::physics::Field& field) {
        quartz::managers::PhysicsManager::Client::getInstance().destroyField(field);
    }

private:
    PhysicsManagerUnitTestClient() = delete;
};

class InputManagerUnitTestClient {
public:
    static quartz::managers::InputManager& getInstance(const std::shared_ptr<GLFWwindow>& p_glfwWindow) {
        return quartz::managers::InputManager::Client::getInstance(p_glfwWindow);
    }
private:
    InputManagerUnitTestClient() = delete;
};

} // namespace unit_test
} // namespace quartz

UT_FUNCTION(test_construction) {
    const quartz::scene::Scene scene;
    
    UT_CHECK_TRUE(scene.getDoodads().empty());
    UT_CHECK_TRUE(scene.getPointLights().empty());
    UT_CHECK_TRUE(scene.getSpotLights().empty());
    UT_CHECK_EQUAL(scene.getScreenClearColor(), math::Vec3(0));
}

UT_FUNCTION(test_high_level) {
    quartz::rendering::Instance renderingInstance("SCENE_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const std::string name = "Scene Test";
    const quartz::scene::AmbientLight ambientLight;
    const quartz::scene::DirectionalLight directionalLight;
    const std::vector<quartz::scene::PointLight> pointLights;
    const std::vector<quartz::scene::SpotLight> spotLights;
    const math::Vec3 screenClearColor(0xFF, 0x00, 0xFF);
    const std::array<std::string, 6> skyBoxInformation {
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posx-00FFFF-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negx-FF0000-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posy-FF00FF-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negy-00FF00-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posz-FFFF00-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negz-0000FF-2x2.jpg")
    };
    const std::vector<quartz::scene::Doodad::Parameters> doodadParameters = {
        quartz::scene::Doodad::Parameters{
            std::nullopt,
            math::Transform{},
            std::nullopt,
            {},
            {},
            {}
        }
    };
    const quartz::physics::Field::Parameters fieldParameters(math::Vec3(0, -9.81, 0));

    const quartz::scene::Scene::Parameters sceneParameters(
        name,
        ambientLight,
        directionalLight,
        pointLights,
        spotLights,
        screenClearColor,
        skyBoxInformation,
        doodadParameters,
        fieldParameters
    );

    quartz::scene::Scene scene;

    scene.load(renderingDevice, physicsManager, sceneParameters);

    UT_CHECK_EQUAL(scene.getAmbientLight(), ambientLight);
    UT_CHECK_EQUAL(scene.getDirectionalLight(), directionalLight);
    UT_CHECK_EQUAL_CONTAINERS(scene.getPointLights(), pointLights);
    UT_CHECK_EQUAL_CONTAINERS(scene.getSpotLights(), spotLights);
    UT_CHECK_EQUAL(scene.getScreenClearColor(), screenClearColor);
    UT_CHECK_EQUAL(scene.getDoodads().size(), doodadParameters.size());

    const uint32_t numIterations = 5;
    for (uint32_t iIteration = 0; iIteration < numIterations; ++iIteration) {
        /**
         * @todo 2025/09/04 Give the doodad parameters some rigid bodies, and update callbacks
         *    so we can update the scene (and fixed update) and validate that things are working.
         *    For now we are trusting the tests written for the doodads and rigid bodies
         */
    }

    scene.unload(physicsManager);

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_high_level);
    UT_RUN_TESTS();
}
