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
    class Doodad;
}
}

class quartz::scene::Doodad {
public: // aliases
    using FixedUpdateCallback = std::function<void(
        Doodad* const p_doodad,
        const quartz::managers::InputManager& inputManager
    )>;
    using UpdateCallback = std::function<void(
        Doodad* const p_doodad,
        const quartz::managers::InputManager& inputManager,
        const double frameTimeDelta,
        const double frameInterpolationFactor
    )>;

public: // classes
    struct Parameters {
        Parameters(
            const std::string& objectFilepath_,
            const math::Transform& transform_,
            const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters_,
            const FixedUpdateCallback& fixedUpdateCallback_,
            const UpdateCallback& updateCallback_
        ) :
            objectFilepath(objectFilepath_),
            transform(transform_),
            o_rigidBodyParameters(o_rigidBodyParameters_),
            fixedUpdateCallback(fixedUpdateCallback_),
            updateCallback(updateCallback_)
        {}

        std::string objectFilepath;
        math::Transform transform;
        std::optional<quartz::physics::RigidBody::Parameters> o_rigidBodyParameters;
        FixedUpdateCallback fixedUpdateCallback;
        UpdateCallback updateCallback;
    };

public: // member functions
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        std::optional<quartz::physics::Field>& o_field,
        const std::string& objectFilepath,
        const math::Transform& transform,
        const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters,
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

    const quartz::rendering::Model& getModel() const { return m_model; }
    const math::Transform& getTransform() const { return m_transform; }
    const math::Mat4& getTransformationMatrix() const { return m_transformationMatrix; }
    const std::optional<quartz::physics::RigidBody>& getRigidBodyOptional() const { return mo_rigidBody; }

    std::optional<quartz::physics::RigidBody>& getRigidBodyOptionalReference() { return mo_rigidBody; }

    void fixedUpdate(const quartz::managers::InputManager& inputManager);
    void update(
        const quartz::managers::InputManager& inputManager,
        const double frameTimeDelta,
        const double frameInterpolationFactor
    );

private: // static functions
    static math::Transform fixTransform(const math::Transform& transform);

private: // member variables
    quartz::rendering::Model m_model;

    math::Transform m_transform;
    math::Mat4 m_transformationMatrix;

    std::optional<quartz::physics::RigidBody> mo_rigidBody;

    FixedUpdateCallback m_fixedUpdateCallback;
    UpdateCallback m_updateCallback;
};
