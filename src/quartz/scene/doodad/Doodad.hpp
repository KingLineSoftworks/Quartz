#pragma once

#include <functional>
#include <optional>
#include <string>

#include "math/transform/Mat4.hpp"
#include "math/transform/Transform.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/field/Field.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/scene/Loggers.hpp"

namespace quartz {
namespace scene {
    class Scene; // need this declaration so we can use it in our awaken functions
    class Doodad;
}
}

class quartz::scene::Doodad {
public: // aliases
    struct AwakenCallbackParameters {
        AwakenCallbackParameters(
            quartz::scene::Scene* const p_scene_
        ) :
            p_scene(p_scene_)
        {}

        quartz::scene::Scene* const p_scene;
    };

    struct FixedUpdateCallbackParameters {
        FixedUpdateCallbackParameters(
            Doodad* const p_doodad_,
            const quartz::managers::InputManager& inputManager_,
            const double totalElapsedTime_,
            const double tickTimeDelta_,
            const double ticksPerSecond_
        ) :
            p_doodad(p_doodad_),
            inputManager(inputManager_),
            totalElapsedTime(totalElapsedTime_),
            tickTimeDelta(tickTimeDelta_),
            ticksPerSecond(ticksPerSecond_)
        {}

        Doodad* const p_doodad;
        const quartz::managers::InputManager& inputManager;
        const double totalElapsedTime;
        const double tickTimeDelta;
        const double ticksPerSecond;
    };

    struct UpdateCallbackParameters {
        UpdateCallbackParameters(
            Doodad* const p_doodad_,
            const quartz::managers::InputManager& inputManager_,
            const double totalElapsedTime_,
            const double frameTimeDelta_,
            const double frameInterpolationFactor_
        ) :
            p_doodad(p_doodad_),
            inputManager(inputManager_),
            totalElapsedTime(totalElapsedTime_),
            frameTimeDelta(frameTimeDelta_),
            frameInterpolationFactor(frameInterpolationFactor_)
        {}

        Doodad* const p_doodad;
        const quartz::managers::InputManager& inputManager;
        const double totalElapsedTime;
        const double frameTimeDelta;
        const double frameInterpolationFactor;
    };

    using AwakenCallback = std::function<void(AwakenCallbackParameters parameters)>;
    using FixedUpdateCallback = std::function<void(FixedUpdateCallbackParameters parameters)>;
    using UpdateCallback = std::function<void(UpdateCallbackParameters parameters)>;

public: // classes
    struct Parameters {
        Parameters(
            const std::optional<std::string>& o_objectFilepath_,
            const math::Transform& transform_,
            const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters_,
            const AwakenCallback& awakenCallback_,
            const FixedUpdateCallback& fixedUpdateCallback_,
            const UpdateCallback& updateCallback_
        ) :
            o_objectFilepath(o_objectFilepath_),
            transform(transform_),
            o_rigidBodyParameters(o_rigidBodyParameters_),
            awakenCallback(awakenCallback_),
            fixedUpdateCallback(fixedUpdateCallback_),
            updateCallback(updateCallback_)
        {}

        std::optional<std::string> o_objectFilepath;
        math::Transform transform;
        std::optional<quartz::physics::RigidBody::Parameters> o_rigidBodyParameters;
        AwakenCallback awakenCallback;
        FixedUpdateCallback fixedUpdateCallback;
        UpdateCallback updateCallback;
    };

public: // member functions
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        std::optional<quartz::physics::Field>& o_field,
        const std::optional<std::string>& o_objectFilepath,
        const math::Transform& transform,
        const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters,
        const quartz::scene::Doodad::AwakenCallback& awakenCallback,
        const quartz::scene::Doodad::FixedUpdateCallback& fixedUpdateCallback,
        const quartz::scene::Doodad::UpdateCallback& updateCallback
    );
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        std::optional<quartz::physics::Field>& o_field,
        const quartz::scene::Doodad::Parameters& doodadParameters
    );
    Doodad(Doodad&& other);
    ~Doodad();

    USE_LOGGER(DOODAD);

    const std::optional<quartz::rendering::Model>& getModelOptional() const { return mo_model; }
    const math::Transform& getTransform() const { return m_transform; }
    const math::Mat4& getTransformationMatrix() const { return m_transformationMatrix; }
    const std::optional<quartz::physics::RigidBody>& getRigidBodyOptional() const { return mo_rigidBody; }

    std::optional<quartz::physics::RigidBody>& getRigidBodyOptionalReference() { return mo_rigidBody; }

    void setPosition(const math::Vec3& position);
    void setRotation(const math::Quaternion& rotation);
    void setScale(const math::Vec3& scale);
    void snapToRigidBody();
    
    /**
     * @todo 2024/11/25 Make these update functions private and allow the quartz::scene::Scene class
     *    to be a friend so we can still allow it to invoke these (as well as quartz::unit_test::UnitTestClient)
     */
    void awaken(
        quartz::scene::Scene* const p_scene
    );
    void fixedUpdate(
        const quartz::managers::InputManager& inputManager,
        const double totalElapsedTime,
        const double tickTimeDelta,
        const double ticksPerSecond
    );
    void update(
        const quartz::managers::InputManager& inputManager,
        const double totalElapsedTime,
        const double frameTimeDelta,
        const double frameInterpolationFactor
    );

private: // static functions
    static void noopAwakenCallback(AwakenCallbackParameters parameters);
    static void noopFixedUpdateCallback(FixedUpdateCallbackParameters parameters);
    static void noopUpdateCallback(UpdateCallbackParameters parameters);
    static math::Transform fixTransform(const math::Transform& transform);

private: // member variables
    std::optional<quartz::rendering::Model> mo_model;

    math::Transform m_transform;
    math::Mat4 m_transformationMatrix;

    std::optional<quartz::physics::RigidBody> mo_rigidBody;

    AwakenCallback m_awakenCallback;
    FixedUpdateCallback m_fixedUpdateCallback;
    UpdateCallback m_updateCallback;
};

