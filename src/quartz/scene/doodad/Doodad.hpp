#pragma once

#include <optional>
#include <string>
#include <variant>

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Mat4.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/physics/rigid_body/RigidBody.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/scene/Loggers.hpp"
#include "quartz/scene/doodad/PhysicsProperties.hpp"
#include "math/transform/Transform.hpp"

namespace quartz {
namespace scene {
    class Doodad;
}
}

class quartz::scene::Doodad {
public: // classes
    struct Parameters {
        Parameters(
            std::string objectFilepath_,
            math::Transform transform_,
            std::optional<quartz::scene::PhysicsProperties> physicsProperties_,
            quartz::physics::RigidBody::Parameters rigidBodyParameters_
        ) :
            objectFilepath(objectFilepath_),
            transform(transform_),
            physicsProperties(physicsProperties_),
            rigidBodyParameters(rigidBodyParameters_)
        {}

        std::string objectFilepath;
        math::Transform transform;
        std::optional<quartz::scene::PhysicsProperties> physicsProperties;
        quartz::physics::RigidBody::Parameters rigidBodyParameters;
    };

public: // member functions
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const std::string& objectFilepath,
        const math::Transform& transform,
        const std::optional<quartz::scene::PhysicsProperties>& o_physicsProperties,
        const quartz::physics::RigidBody::Parameters& rigidBodyParameters
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
    static reactphysics3d::RigidBody* createRigidBodyPtr(
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const std::optional<quartz::scene::PhysicsProperties>& o_physicsProperties,
        const math::Transform& transform
    );
    static reactphysics3d::Collider* createColliderPtr(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const math::Transform& transform
    );

private: // member variables
    quartz::rendering::Model m_model;

    math::Transform m_transform;
    math::Mat4 m_transformationMatrix;

    reactphysics3d::RigidBody* mp_rigidBody;
    reactphysics3d::Collider* mp_collider;
    std::optional<quartz::physics::RigidBody> mo_rigidBody;
};