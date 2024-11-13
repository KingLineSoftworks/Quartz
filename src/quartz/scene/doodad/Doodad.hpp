#pragma once

#include <functional>
#include <optional>
#include <string>

#include "math/transform/Mat4.hpp"
#include "math/transform/Transform.hpp"

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
    using UpdateCallback = std::function<void()>;

public: // classes
    struct Parameters {
        Parameters(
            const std::string& objectFilepath_,
            const math::Transform& transform_,
            const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters_
        ) :
            objectFilepath(objectFilepath_),
            transform(transform_),
            o_rigidBodyParameters(o_rigidBodyParameters_)
        {}

        std::string objectFilepath;
        math::Transform transform;
        std::optional<quartz::physics::RigidBody::Parameters> o_rigidBodyParameters;
    };

public: // member functions
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        std::optional<quartz::physics::Field>& o_field,
        const std::string& objectFilepath,
        const math::Transform& transform,
        const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters
    );
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        std::optional<quartz::physics::Field>& o_field,
        quartz::scene::Doodad::Parameters& doodadParameters
    );
    Doodad(Doodad&& other);
    ~Doodad();

    USE_LOGGER(DOODAD);

    const quartz::rendering::Model& getModel() const { return m_model; }
    const math::Mat4& getTransformationMatrix() const { return m_transformationMatrix; }

    void fixedUpdate();
    void update(
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

    UpdateCallback m_fixedUpdateCallback;
    UpdateCallback m_updateCallback;
};
