#include <cstdint>
#include <memory>
#include <optional>

#include "quartz/physics/collider/SphereShape.hpp"
#include "util/Loggers.hpp"
#include "util/file_system/FileSystem.hpp"
#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "math/transform/Mat4.hpp"
#include "math/transform/Vec3.hpp"
#include "math/transform/Quaternion.hpp"
#include "math/transform/Transform.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/collider/Collider.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/model/Scene.hpp"
#include "quartz/rendering/texture/Texture.hpp"
#include "quartz/scene/doodad/Doodad.hpp"
#include "quartz/scene/scene/Scene.hpp"

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

/**
 * @todo 2025/08/18 Test that the doodad and the rigidbody are rotated in the same direction.
 *    If we set the rotation of the rigidbody, then the doodad should also rotate that way.
 *    If we set the rotation of the doodad, then the rigidbody should also rotate that way.
 */

UT_FUNCTION(test_construction) {
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    // no model, no rigidbody, no callbacks
    {
        const math::Vec3 position(1, 2, 3);
        const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
        const math::Vec3 scale(4, 5, 6);
        const math::Transform transform(position, rotation, scale);

        const quartz::scene::Doodad::Parameters parameters(
            std::nullopt,
            transform,
            std::nullopt,
            {},
            {},
            {}
        );

        const quartz::scene::Doodad doodad(renderingDevice, physicsManager, field, parameters);

        UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
        UT_REQUIRE_NOT(doodad.getModelOptional());
        UT_REQUIRE_NOT(doodad.getRigidBodyOptional());
    }

    // model, rigidbody, no op callbacks
    {
        const std::string objectFilepath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb");

        const math::Vec3 position(1, 2, 3);
        const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
        const math::Vec3 scale(4, 5, 6);
        const math::Transform transform(position, rotation, scale);

        const double sphereRadius_m = 43;
        const quartz::physics::Collider::Parameters colliderParameters(
            false,
            quartz::physics::Collider::CategoryProperties(0b01, 0b11),
            quartz::physics::SphereShape::Parameters(sphereRadius_m),
            {},
            {},
            {}
        );

        const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
        const bool enableGravity = true;
        const math::Vec3 angularAxisFactor(0, 0, 0);
        const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

        const quartz::scene::Doodad doodad(
            renderingDevice,
            physicsManager,
            field,
            objectFilepath,
            transform,
            rbParameters,
            {},
            {},
            {}
        );

        UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

        const std::optional<quartz::rendering::Model>& o_model = doodad.getModelOptional();
        UT_REQUIRE(o_model);
        const quartz::rendering::Model& model = *o_model;
        UT_CHECK_EQUAL(model.getMaterialMasterIndices().size(), 0);
        const quartz::rendering::Scene& defaultScene = model.getDefaultScene();
        UT_CHECK_EQUAL(defaultScene.getRootNodePtrs().size(), 1);

        const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
        UT_REQUIRE(o_rigidBody);
        const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
        UT_CHECK_EQUAL(rigidBody.getPosition(), position);
        UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
        const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
        UT_REQUIRE(o_collider);
        UT_CHECK_EQUAL(o_collider->getCategoryProperties(), colliderParameters.categoryProperties);
        UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
        UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
        UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
        UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
        UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);
        const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
        UT_REQUIRE(o_sphereShape);
        UT_CHECK_EQUAL_FLOATS(o_sphereShape->getRadius_m(), sphereRadius_m * scale.y); // scaled by transform's y element
        const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
        UT_REQUIRE(!o_boxShape);
    }

    // More complex model, rigidbody, no callbacks
    {
        const std::string objectFilepath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/models/glTF-Sample-Models/2.0/AntiqueCamera/glTF/AntiqueCamera.gltf");

        const math::Vec3 position(1, 2, 3);
        const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
        const math::Vec3 scale(4, 5, 6);
        const math::Transform transform(position, rotation, scale);

        const math::Vec3 boxHalfExtents_m(5, 2, 13);
        const quartz::physics::Collider::Parameters colliderParameters(
            false,
            quartz::physics::Collider::CategoryProperties(0b110101, 0b11000101),
            quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
            {},
            {},
            {}
        );

        const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
        const bool enableGravity = true;
        const math::Vec3 angularAxisFactor(1, 2, 3);
        const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

        const quartz::scene::Doodad doodad(
            renderingDevice,
            physicsManager,
            field,
            objectFilepath,
            transform,
            rbParameters,
            {},
            {},
            {}
        );

        UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

        const std::optional<quartz::rendering::Model>& o_model = doodad.getModelOptional();
        UT_REQUIRE(o_model);
        const quartz::rendering::Model& model = *o_model;
        UT_CHECK_EQUAL(model.getMaterialMasterIndices().size(), 2);
        const quartz::rendering::Scene& defaultScene = model.getDefaultScene();
        UT_CHECK_EQUAL(defaultScene.getRootNodePtrs().size(), 2);

        const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
        UT_REQUIRE(o_rigidBody);
        const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
        UT_CHECK_EQUAL(rigidBody.getPosition(), position);
        UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
        const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
        UT_REQUIRE(o_collider);
        UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
        UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
        UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
        UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
        UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);
        const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
        UT_REQUIRE(!o_sphereShape);
        const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
        UT_REQUIRE(o_boxShape);
        UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), boxHalfExtents_m * scale.abs());
        const std::array<math::Vec3, 8> boxLocalVertexPositions = o_boxShape->getLocalVertexPositions();
        UT_CHECK_EQUAL(boxLocalVertexPositions[0], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[1], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[2], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[3], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[4], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[5], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[6], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));
        UT_CHECK_EQUAL(boxLocalVertexPositions[7], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));
    }

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_awaken) {
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    bool awakened = false;
    quartz::scene::Doodad::AwakenCallback awakenCallback = [&] (UNUSED quartz::scene::Doodad::AwakenCallbackParameters callbackParameters) {
        awakened = true;

        UT_REQUIRE(callbackParameters.p_scene);
    };

    const quartz::scene::Doodad::Parameters parameters(
        std::nullopt,
        transform,
        std::nullopt,
        awakenCallback,
        {},
        {}
    );

    quartz::scene::Doodad doodad(renderingDevice, physicsManager, field, parameters);

    quartz::scene::Scene scene;

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_REQUIRE_NOT(doodad.getModelOptional());
    UT_REQUIRE_NOT(doodad.getRigidBodyOptional());

    doodad.awaken(&scene);

    UT_CHECK_TRUE(awakened);

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_updateCallback) {
    const std::string testName = "DOODAD_UT-test_updateCallback";
    quartz::rendering::Instance renderingInstance(testName, 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);
    quartz::rendering::Window renderingWindow(testName, 4, 4, renderingInstance, renderingDevice);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const math::Vec3 boxHalfExtents_m(5, 2, 13);
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b110101, 0b11000101),
        quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(1, 2, 3);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    const math::Vec3 newPosition(99, 100, 101);
    const math::Vec3 newScale(88, 77, 66);
    const math::Quaternion newRotation = math::Quaternion::fromDirectionVector(math::Vec3::Left);
    bool updated = false;
    quartz::scene::Doodad::UpdateCallback updateCallback = [&] (quartz::scene::Doodad::UpdateCallbackParameters callbackParameters) {
        updated = true;

        UT_REQUIRE(callbackParameters.p_doodad);

        std::optional<quartz::physics::RigidBody>& o_rigidBody = callbackParameters.p_doodad->getRigidBodyOptionalReference();
        UT_REQUIRE(o_rigidBody);

        quartz::physics::RigidBody& rigidBody = *o_rigidBody;

        rigidBody.setPosition(newPosition);
        rigidBody.setScale(newScale);
        rigidBody.setRotation(newRotation);
    };

    const quartz::scene::Doodad::Parameters parameters(
        std::nullopt,
        transform,
        rbParameters,
        {},
        {},
        updateCallback
    );

    quartz::scene::Doodad doodad(renderingDevice, physicsManager, field, parameters);

    quartz::managers::InputManager& inputManager = quartz::unit_test::InputManagerUnitTestClient::getInstance(renderingWindow.getGLFWwindowPtr());

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_REQUIRE_NOT(doodad.getModelOptional());
    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
    UT_CHECK_EQUAL(rigidBody.getPosition(), position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);
    const quartz::physics::Collider& collider = *o_collider;
    UT_CHECK_EQUAL(collider.getWorldPosition(), position);
    UT_CHECK_EQUAL(collider.getWorldRotation(), rotation);
    const std::optional<quartz::physics::BoxShape>& o_boxShape = collider.getBoxShapeOptional();
    UT_REQUIRE(o_boxShape);
    const quartz::physics::BoxShape& boxShape = *o_boxShape;
    UT_CHECK_EQUAL(boxShape.getHalfExtents_m(), boxHalfExtents_m * scale);
    const std::optional<quartz::physics::SphereShape>& o_sphereShape = collider.getSphereShapeOptional();
    UT_REQUIRE_NOT(o_sphereShape);

    UT_CHECK_FALSE(updated);

    doodad.update(
        inputManager,
        100.0,
        0.5,
        0.25
    );

    // The doodad's position and rotation should be updated because the position and rotation of the rigidbody has changed
    // THE SCALE OF THE DOODAD IS NOT EFFECTED BY THE SCALE OF THE COLLIDER
    UT_CHECK_EQUAL(doodad.getTransform().position, newPosition);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, newRotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_REQUIRE_NOT(doodad.getModelOptional());
    UT_REQUIRE(o_rigidBody);
    UT_CHECK_EQUAL(rigidBody.getPosition(), newPosition);
    UT_CHECK_EQUAL(rigidBody.getRotation(), newRotation);
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(collider.getWorldPosition(), newPosition);
    UT_CHECK_EQUAL(collider.getWorldRotation(), newRotation);
    UT_REQUIRE(o_boxShape);
    UT_CHECK_EQUAL(boxShape.getHalfExtents_m(), newScale);
    UT_REQUIRE_NOT(o_sphereShape);

    UT_CHECK_TRUE(updated);

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_fixedUpdateCallback) {
    const std::string testName = "DOODAD_UT-test_fixedUpdateCallback";
    quartz::rendering::Instance renderingInstance(testName, 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);
    quartz::rendering::Window renderingWindow(testName, 4, 4, renderingInstance, renderingDevice);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const double sphereRadius_m = 43;
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        quartz::physics::SphereShape::Parameters(sphereRadius_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(0, 0, 0);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    const math::Vec3 newPosition(99, 100, 101);
    const math::Vec3 newScale(88, 777, 66);
    const math::Quaternion newRotation = math::Quaternion::fromDirectionVector(math::Vec3::Left);
    bool updated = false;
    quartz::scene::Doodad::FixedUpdateCallback fixedUpdateCallback = [&] (quartz::scene::Doodad::FixedUpdateCallbackParameters callbackParameters) {
        updated = true;

        UT_REQUIRE(callbackParameters.p_doodad);

        std::optional<quartz::physics::RigidBody>& o_rigidBody = callbackParameters.p_doodad->getRigidBodyOptionalReference();
        UT_REQUIRE(o_rigidBody);

        quartz::physics::RigidBody& rigidBody = *o_rigidBody;

        rigidBody.setPosition(newPosition);
        rigidBody.setScale(newScale);
        rigidBody.setRotation(newRotation);
    };

    const quartz::scene::Doodad::Parameters parameters(
        std::nullopt,
        transform,
        rbParameters,
        {},
        fixedUpdateCallback,
        {}
    );

    quartz::scene::Doodad doodad(renderingDevice, physicsManager, field, parameters);

    quartz::managers::InputManager& inputManager = quartz::unit_test::InputManagerUnitTestClient::getInstance(renderingWindow.getGLFWwindowPtr());

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_REQUIRE_NOT(doodad.getModelOptional());
    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
    UT_CHECK_EQUAL(rigidBody.getPosition(), position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);
    const quartz::physics::Collider& collider = *o_collider;
    UT_CHECK_EQUAL(collider.getWorldPosition(), position);
    UT_CHECK_EQUAL(collider.getWorldRotation(), rotation);
    const std::optional<quartz::physics::BoxShape>& o_boxShape = collider.getBoxShapeOptional();
    UT_REQUIRE_NOT(o_boxShape);
    const std::optional<quartz::physics::SphereShape>& o_sphereShape = collider.getSphereShapeOptional();
    UT_REQUIRE(o_sphereShape);
    const quartz::physics::SphereShape& sphereShape = *o_sphereShape;
    UT_CHECK_EQUAL(sphereShape.getRadius_m(), sphereRadius_m * 5);

    UT_CHECK_FALSE(updated);

    doodad.fixedUpdate(
        inputManager,
        100.0,
        0.5,
        0.25
    );

    // The doodad will not be moved to the position and rotation of the rigid body and the collider
    // This is because we do not need to move the doodad during the physics step, because it's a graphical component
    // THE SCALE OF THE DOODAD IS NOT EFFECTED BY THE SCALE OF THE COLLIDER
    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_REQUIRE_NOT(doodad.getModelOptional());
    UT_REQUIRE(o_rigidBody);
    UT_CHECK_EQUAL(rigidBody.getPosition(), newPosition);
    UT_CHECK_EQUAL(rigidBody.getRotation(), newRotation);
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(collider.getWorldPosition(), newPosition);
    UT_CHECK_EQUAL(collider.getWorldRotation(), newRotation);
    UT_REQUIRE_NOT(o_boxShape);
    UT_REQUIRE(o_sphereShape);
    UT_CHECK_EQUAL(sphereShape.getRadius_m(), newScale.y);

    UT_CHECK_TRUE(updated);

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_setPosition) {
    // Ensure model and rigidBody are both effected correctly
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const std::string objectFilepath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb");

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const double sphereRadius_m = 43;
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        quartz::physics::SphereShape::Parameters(sphereRadius_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(0, 0, 0);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    quartz::scene::Doodad doodad(
        renderingDevice,
        physicsManager,
        field,
        objectFilepath,
        transform,
        rbParameters,
        {},
        {},
        {}
    );

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

    const std::optional<quartz::rendering::Model>& o_model = doodad.getModelOptional();
    UT_REQUIRE(o_model);
    const quartz::rendering::Model& model = *o_model;
    UT_CHECK_EQUAL(model.getMaterialMasterIndices().size(), 0);
    const quartz::rendering::Scene& defaultScene = model.getDefaultScene();
    UT_CHECK_EQUAL(defaultScene.getRootNodePtrs().size(), 1);

    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
    UT_CHECK_EQUAL(rigidBody.getPosition(), position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(o_collider->getCategoryProperties(), colliderParameters.categoryProperties);
    UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
    UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);
    const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
    UT_REQUIRE(o_sphereShape);
    UT_CHECK_EQUAL_FLOATS(o_sphereShape->getRadius_m(), sphereRadius_m * scale.y); // scaled by transform's y element
    const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
    UT_REQUIRE(!o_boxShape);

    const math::Vec3 updatedPosition(44, 55, 66);
    doodad.setPosition(updatedPosition);

    UT_CHECK_EQUAL(doodad.getTransform().position, updatedPosition);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_CHECK_EQUAL(rigidBody.getPosition(), updatedPosition);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_setRotation) {
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const std::string objectFilepath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb");

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const double sphereRadius_m = 43;
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        quartz::physics::SphereShape::Parameters(sphereRadius_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(0, 0, 0);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    quartz::scene::Doodad doodad(
        renderingDevice,
        physicsManager,
        field,
        objectFilepath,
        transform,
        rbParameters,
        {},
        {},
        {}
    );

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

    const std::optional<quartz::rendering::Model>& o_model = doodad.getModelOptional();
    UT_REQUIRE(o_model);
    const quartz::rendering::Model& model = *o_model;
    UT_CHECK_EQUAL(model.getMaterialMasterIndices().size(), 0);
    const quartz::rendering::Scene& defaultScene = model.getDefaultScene();
    UT_CHECK_EQUAL(defaultScene.getRootNodePtrs().size(), 1);

    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
    UT_CHECK_EQUAL(rigidBody.getPosition(), position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(o_collider->getCategoryProperties(), colliderParameters.categoryProperties);
    UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
    UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);
    const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
    UT_REQUIRE(o_sphereShape);
    UT_CHECK_EQUAL_FLOATS(o_sphereShape->getRadius_m(), sphereRadius_m * scale.y); // scaled by transform's y element
    const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
    UT_REQUIRE(!o_boxShape);

    const math::Quaternion updatedRotation = math::Quaternion::fromDirectionVector(math::Vec3::Left);
    doodad.setRotation(updatedRotation);

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, updatedRotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_CHECK_EQUAL(rigidBody.getPosition(), transform.position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), updatedRotation);

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_setScale_sphere) {
    // Ensure model and rigidBody are both effected correctly
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const std::string objectFilepath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb");

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const double sphereRadius_m = 43;
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        quartz::physics::SphereShape::Parameters(sphereRadius_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(0, 0, 0);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    quartz::scene::Doodad doodad(
        renderingDevice,
        physicsManager,
        field,
        objectFilepath,
        transform,
        rbParameters,
        {},
        {},
        {}
    );

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

    const std::optional<quartz::rendering::Model>& o_model = doodad.getModelOptional();
    UT_REQUIRE(o_model);
    const quartz::rendering::Model& model = *o_model;
    UT_CHECK_EQUAL(model.getMaterialMasterIndices().size(), 0);
    const quartz::rendering::Scene& defaultScene = model.getDefaultScene();
    UT_CHECK_EQUAL(defaultScene.getRootNodePtrs().size(), 1);

    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
    UT_CHECK_EQUAL(rigidBody.getPosition(), position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(o_collider->getCategoryProperties(), colliderParameters.categoryProperties);
    UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
    UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);
    const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
    UT_REQUIRE(o_sphereShape);
    UT_CHECK_EQUAL_FLOATS(o_sphereShape->getRadius_m(), sphereRadius_m * scale.y); // scaled by transform's y element
    const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
    UT_REQUIRE(!o_boxShape);

    const math::Vec3 updatedScale(-33, -55, 77);
    doodad.setScale(updatedScale);

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, updatedScale);
    UT_CHECK_EQUAL(rigidBody.getPosition(), transform.position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), transform.rotation);
    UT_CHECK_EQUAL(o_sphereShape->getRadius_m(), std::abs(updatedScale.y));

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_setScale_box) {
    // Ensure model and rigidBody are both effected correctly
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const std::string objectFilepath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/models/unit_models/unit_cube/glb/unit_cube.glb");

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const math::Vec3 boxHalfExtents_m(5, 2, 13);
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b110101, 0b11000101),
        quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(1, 2, 3);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    quartz::scene::Doodad doodad(
        renderingDevice,
        physicsManager,
        field,
        objectFilepath,
        transform,
        rbParameters,
        {},
        {},
        {}
    );

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

    const std::optional<quartz::rendering::Model>& o_model = doodad.getModelOptional();
    UT_REQUIRE(o_model);
    const quartz::rendering::Model& model = *o_model;
    UT_CHECK_EQUAL(model.getMaterialMasterIndices().size(), 0);
    const quartz::rendering::Scene& defaultScene = model.getDefaultScene();
    UT_CHECK_EQUAL(defaultScene.getRootNodePtrs().size(), 1);

    const std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptional();
    UT_REQUIRE(o_rigidBody);
    const quartz::physics::RigidBody& rigidBody = *o_rigidBody;
    UT_CHECK_EQUAL(rigidBody.getPosition(), position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(o_collider->getCategoryProperties(), colliderParameters.categoryProperties);
    UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
    UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);
    const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
    UT_REQUIRE_NOT(o_sphereShape);
    const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
    UT_REQUIRE(o_boxShape);
    UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), boxHalfExtents_m * scale.abs());
    const std::array<math::Vec3, 8> boxLocalVertexPositions = o_boxShape->getLocalVertexPositions();
    UT_CHECK_EQUAL(boxLocalVertexPositions[0], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
    UT_CHECK_EQUAL(boxLocalVertexPositions[1], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
    UT_CHECK_EQUAL(boxLocalVertexPositions[2], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
    UT_CHECK_EQUAL(boxLocalVertexPositions[3], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y),  1 * boxHalfExtents_m.z * std::abs(scale.z)));
    UT_CHECK_EQUAL(boxLocalVertexPositions[4], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));
    UT_CHECK_EQUAL(boxLocalVertexPositions[5], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x), -1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));
    UT_CHECK_EQUAL(boxLocalVertexPositions[6], math::Vec3( 1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));
    UT_CHECK_EQUAL(boxLocalVertexPositions[7], math::Vec3(-1 * boxHalfExtents_m.x * std::abs(scale.x),  1 * boxHalfExtents_m.y * std::abs(scale.y), -1 * boxHalfExtents_m.z * std::abs(scale.z)));

    const math::Vec3 updatedScale(-33, -55, 77);
    doodad.setScale(updatedScale);

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, updatedScale);
    UT_CHECK_EQUAL(rigidBody.getPosition(), transform.position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), transform.rotation);
    UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), updatedScale.abs());

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_snapToRigidBody) {
    // Ensure that if the rigidBody is at a differing position and rotation to the doodad,
    // the doodad will take that position and rotation as well
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const math::Vec3 boxHalfExtents_m(5, 2, 13);
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b110101, 0b11000101),
        quartz::physics::BoxShape::Parameters(boxHalfExtents_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(1, 2, 3);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    quartz::scene::Doodad doodad(
        renderingDevice,
        physicsManager,
        field,
        std::nullopt,
        transform,
        rbParameters,
        {},
        {},
        {}
    );

    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

    const std::optional<quartz::rendering::Model>& o_model = doodad.getModelOptional();
    UT_REQUIRE_NOT(o_model);

    std::optional<quartz::physics::RigidBody>& o_rigidBody = doodad.getRigidBodyOptionalReference();
    UT_REQUIRE(o_rigidBody);
    quartz::physics::RigidBody& rigidBody = *o_rigidBody;
    UT_CHECK_EQUAL(rigidBody.getPosition(), position);
    UT_CHECK_EQUAL(rigidBody.getRotation(), rotation);
    const std::optional<quartz::physics::Collider>& o_collider = rigidBody.getColliderOptional();
    UT_REQUIRE(o_collider);
    UT_CHECK_EQUAL(o_collider->getCategoryProperties(), colliderParameters.categoryProperties);
    UT_CHECK_EQUAL(o_collider->getIsTrigger(), colliderParameters.isTrigger);
    UT_CHECK_EQUAL(o_collider->getLocalPosition(), math::Vec3(0));
    UT_CHECK_EQUAL(o_collider->getLocalRotation(), math::Quaternion());
    UT_CHECK_EQUAL(o_collider->getWorldPosition(), position);
    UT_CHECK_EQUAL(o_collider->getWorldRotation(), rotation);
    const std::optional<quartz::physics::SphereShape>& o_sphereShape = o_collider->getSphereShapeOptional();
    UT_REQUIRE_NOT(o_sphereShape);
    const std::optional<quartz::physics::BoxShape>& o_boxShape = o_collider->getBoxShapeOptional();
    UT_REQUIRE(o_boxShape);
    UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), boxHalfExtents_m * scale.abs());

    const math::Vec3 updatedPosition(101, 102, 103);
    const math::Quaternion updatedRotation = math::Quaternion::fromDirectionVector(math::Vec3::Backward);
    const math::Vec3 updatedScale(-33, -55, 77);

    o_rigidBody->setPosition(updatedPosition);
    o_rigidBody->setRotation(updatedRotation);
    o_rigidBody->setScale(updatedScale);

    // Doodad should still have original values while rigidbody and collider have been updated
    UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_CHECK_EQUAL(rigidBody.getPosition(), updatedPosition);
    UT_CHECK_EQUAL(rigidBody.getRotation(), updatedRotation);
    UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), updatedScale.abs());

    doodad.snapToRigidBody();

    // Doodad should still have original scale while position and rotation have been updated
    UT_CHECK_EQUAL(doodad.getTransform().position, updatedPosition);
    UT_CHECK_EQUAL(doodad.getTransform().rotation, updatedRotation);
    UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);
    UT_CHECK_EQUAL(rigidBody.getPosition(), updatedPosition);
    UT_CHECK_EQUAL(rigidBody.getRotation(), updatedRotation);
    UT_CHECK_EQUAL(o_boxShape->getHalfExtents_m(), updatedScale.abs());

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_physics) {
    // When the rigid body moves and rotates, so do we - but we don't scale
    // Set the position
    // Set an update callback which modifies the position, rotation, and scale
    // Invoke doodad.update
    // Ensure the transformation matrix matches what we expect it to be

    const std::string testName = "DOODAD_UT-test_physics";
    quartz::rendering::Instance renderingInstance(testName, 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);
    quartz::rendering::Window renderingWindow(testName, 4, 4, renderingInstance, renderingDevice);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();
    
    quartz::managers::InputManager& inputManager = quartz::unit_test::InputManagerUnitTestClient::getInstance(renderingWindow.getGLFWwindowPtr());

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const double sphereRadius_m = 43;
    const quartz::physics::Collider::Parameters colliderParameters(
        false,
        quartz::physics::Collider::CategoryProperties(0b01, 0b11),
        quartz::physics::SphereShape::Parameters(sphereRadius_m),
        {},
        {},
        {}
    );

    const quartz::physics::RigidBody::BodyType bodyType = quartz::physics::RigidBody::BodyType::Static;
    const bool enableGravity = true;
    const math::Vec3 angularAxisFactor(0, 0, 0);
    const quartz::physics::RigidBody::Parameters rbParameters(bodyType, enableGravity, angularAxisFactor, colliderParameters);

    const math::Vec3 updatedPosition(101, 102, -97);
    const math::Quaternion updatedRotation = math::Quaternion::fromDirectionVector(math::Vec3::Down);
    const math::Vec3 updatedScale(40, 50, 60);
    const quartz::scene::Doodad::UpdateCallback updateCallback = [&] (UNUSED quartz::scene::Doodad::UpdateCallbackParameters callbackParameters) {
        // We don't actually have to do anything in the update step, because we will be updating the rigid body's position
    };

    const std::vector<double> interpolationFactors = {
        0.0,
        1.0,
        0.5,
        0.42,
        0.71
    };

    // No interpolation, use original position
    for (uint32_t iInterp = 0; iInterp < interpolationFactors.size(); ++iInterp) {
        const double interpolationFactor = interpolationFactors[iInterp];

        quartz::scene::Doodad doodad(
            renderingDevice,
            physicsManager,
            field,
            std::nullopt,
            transform,
            rbParameters,
            {},
            {},
            updateCallback
        );

        // Initial transform should match
        UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

        // Initial transformation matrix should match as well
        UT_CHECK_EQUAL(doodad.getTransformationMatrix(), transform.calculateTransformationMatrix());

        // Update the position of the rigid body
        quartz::physics::RigidBody& rigidBody = *doodad.getRigidBodyOptionalReference();
        rigidBody.setPosition(updatedPosition);
        rigidBody.setRotation(updatedRotation);
        rigidBody.setScale(updatedScale);

        // Update the doodad to interpolate between the two positions
        doodad.update(
            inputManager,
            100.0,
            5.0,
            interpolationFactor
        );

        // Transform should actually be fully updated, besides the scale
        UT_CHECK_EQUAL(doodad.getTransform().position, updatedPosition);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, updatedRotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

        // Transformation matrix should match all of our updated values
        const math::Transform expectedTransform(
            math::Vec3::lerp(position, updatedPosition, interpolationFactor),
            math::Quaternion::slerp(rotation, updatedRotation, interpolationFactor),
            scale 
        );
        const math::Mat4 expectedMatrix = expectedTransform.calculateTransformationMatrix();
        const math::Mat4 actualMatrix = doodad.getTransformationMatrix();
        for (uint32_t iCol = 0; iCol < 4; ++iCol) {
            const math::Vec4& colExpected = expectedMatrix.cols[iCol];
            const math::Vec4& colActual = actualMatrix.cols[iCol];
            for (uint32_t iRow = 0; iRow < 4; ++iRow) {
                UT_CHECK_EQUAL_FLOATS(colActual[iRow], colExpected[iRow]);
            }
        }
    }

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_transformationMatrix) {
    // Set the position
    // Set an update callback which modifies the position, rotation, and scale
    // Invoke doodad.update
    // Ensure the transformation matrix matches what we expect it to be

    const std::string testName = "DOODAD_UT_test_transformationMatrix";
    quartz::rendering::Instance renderingInstance(testName, 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);
    quartz::rendering::Window renderingWindow(testName, 4, 4, renderingInstance, renderingDevice);

    quartz::managers::PhysicsManager& physicsManager = quartz::unit_test::PhysicsManagerUnitTestClient::getInstance();
    std::optional<quartz::physics::Field> field = quartz::unit_test::PhysicsManagerUnitTestClient::createField();
    
    quartz::managers::InputManager& inputManager = quartz::unit_test::InputManagerUnitTestClient::getInstance(renderingWindow.getGLFWwindowPtr());

    const math::Vec3 position(1, 2, 3);
    const math::Quaternion rotation = math::Quaternion::fromDirectionVector(math::Vec3::Right);
    const math::Vec3 scale(4, 5, 6);
    const math::Transform transform(position, rotation, scale);

    const math::Vec3 updatedPosition(101, 102, -97);
    const math::Quaternion updatedRotation = math::Quaternion::fromDirectionVector(math::Vec3::Down);
    const math::Vec3 updatedScale(40, 50, 60);
    const quartz::scene::Doodad::UpdateCallback updateCallback = [&] (quartz::scene::Doodad::UpdateCallbackParameters callbackParameters) {
        callbackParameters.p_doodad->setPosition(updatedPosition);
        callbackParameters.p_doodad->setRotation(updatedRotation);
        callbackParameters.p_doodad->setScale(updatedScale);
    };

    // Interpolation factor does not matter because we don't have a rigid body
    {
        const double interpolationFactor = 0.42;

        quartz::scene::Doodad doodad(
            renderingDevice,
            physicsManager,
            field,
            std::nullopt,
            transform,
            std::nullopt,
            {},
            {},
            updateCallback
        );

        // Initial transform should match
        UT_CHECK_EQUAL(doodad.getTransform().position, transform.position);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, transform.rotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, transform.scale);

        // Initial transformation matrix should match as well
        UT_CHECK_EQUAL(doodad.getTransformationMatrix(), transform.calculateTransformationMatrix());

        doodad.update(
            inputManager,
            100.0,
            5.0,
            interpolationFactor
        );

        // Transform should actually be fully updated
        UT_CHECK_EQUAL(doodad.getTransform().position, updatedPosition);
        UT_CHECK_EQUAL(doodad.getTransform().rotation, updatedRotation);
        UT_CHECK_EQUAL(doodad.getTransform().scale, updatedScale);

        // Transformation matrix should match all of our updated values
        const math::Transform expectedTransform(
            updatedPosition,
            updatedRotation,
            updatedScale
        );
        const math::Mat4 expectedMatrix = expectedTransform.calculateTransformationMatrix();
        const math::Mat4 actualMatrix = doodad.getTransformationMatrix();
        for (uint32_t iCol = 0; iCol < 4; ++iCol) {
            const math::Vec4& colExpected = expectedMatrix.cols[iCol];
            const math::Vec4& colActual = actualMatrix.cols[iCol];
            for (uint32_t iRow = 0; iRow < 4; ++iRow) {
                UT_CHECK_EQUAL_FLOATS(colActual[iRow], colExpected[iRow]);
            }
        }
    }

    quartz::unit_test::PhysicsManagerUnitTestClient::destroyField(*field);
    quartz::rendering::Texture::cleanUpAllTextures();
}

UT_FUNCTION(test_fixTransform) {
    const math::Vec3 p1(1, 2, 3);
    const math::Quaternion r1(1, 2, 3, 4);
    const math::Vec3 s1(3, 2, 1);
    const math::Transform t1(p1, r1, s1);
    const math::Transform t1f = quartz::scene::Doodad::fixTransform(t1);
    UT_CHECK_EQUAL(t1f.position, t1.position);
    UT_CHECK_EQUAL(t1f.rotation, t1.rotation);
    UT_CHECK_EQUAL(t1f.scale, t1.scale);

    const math::Quaternion r2(0, 0, 0, 0);
    const math::Transform t2(p1, r2, s1);
    const math::Transform t2f = quartz::scene::Doodad::fixTransform(t2);
    UT_CHECK_EQUAL(t2f.position, t2.position);
    UT_CHECK_NOT_EQUAL(t2f.rotation, t2.rotation);
    UT_CHECK_EQUAL(t2f.rotation, math::Quaternion::fromDirectionVector(math::Vec3::Forward));
    UT_CHECK_EQUAL(t2f.scale, t2.scale);
}

UT_FUNCTION(test_calculateTransformationMatrix) {
    {
        const math::Transform a(
            {3, 3, 3},
            math::Quaternion::fromDirectionVector(math::Vec3::Forward),
            {1, 1, 1}
        );

        const math::Transform b(
            {-3, -3, -3},
            math::Quaternion::fromDirectionVector(math::Vec3::Backward),
            {10, 10, 10}
        );

        const math::Mat4 matA = quartz::scene::Doodad::calculateTransformationmatrix(a, b, 0);
        UT_CHECK_EQUAL(matA, a.calculateTransformationMatrix());

        const math::Mat4 matB = quartz::scene::Doodad::calculateTransformationmatrix(a, b, 1);
        UT_CHECK_EQUAL(matB, b.calculateTransformationMatrix());
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction);
    REGISTER_UT_FUNCTION(test_awaken);
    REGISTER_UT_FUNCTION(test_updateCallback);
    REGISTER_UT_FUNCTION(test_fixedUpdateCallback);
    REGISTER_UT_FUNCTION(test_setPosition);
    REGISTER_UT_FUNCTION(test_setRotation);
    REGISTER_UT_FUNCTION(test_setScale_sphere);
    REGISTER_UT_FUNCTION(test_setScale_box);
    REGISTER_UT_FUNCTION(test_snapToRigidBody);
    REGISTER_UT_FUNCTION(test_physics);
    REGISTER_UT_FUNCTION(test_transformationMatrix);
    REGISTER_UT_FUNCTION(test_fixTransform);
    REGISTER_UT_FUNCTION(test_calculateTransformationMatrix);
    UT_RUN_TESTS();
}

