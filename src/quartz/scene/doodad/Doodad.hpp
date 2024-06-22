#pragma once

#include <optional>
#include <string>

#include <reactphysics3d/reactphysics3d.h>

#include "math/transform/Mat4.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/model/Model.hpp"
#include "quartz/scene/Loggers.hpp"
#include "quartz/scene/doodad/PhysicsProperties.hpp"
#include "quartz/scene/doodad/Transform.hpp"

namespace quartz {
namespace scene {
    class Doodad;
}
}

class quartz::scene::Doodad {
public: //

public: // member functions
    Doodad(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        const std::string& objectFilepath,
        const std::optional<quartz::scene::PhysicsProperties>& o_physicsProperties,
        const quartz::scene::Transform& transform,
        reactphysics3d::PhysicsWorld* p_physicsWorld
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
    static quartz::scene::Transform fixTransform(const quartz::scene::Transform& transform);
    static reactphysics3d::RigidBody* createRigidBodyPtr(
        reactphysics3d::PhysicsWorld* p_physicsWorld,
        const std::optional<quartz::scene::PhysicsProperties>& o_physicsProperties,
        const quartz::scene::Transform& transform
    );
    static reactphysics3d::Collider* createColliderPtr(
        quartz::managers::PhysicsManager& physicsManager,
        reactphysics3d::RigidBody* p_rigidBody,
        const quartz::scene::Transform& transform
    );

private: // member variables
    quartz::rendering::Model m_model;

    quartz::scene::Transform m_transform;
    math::Mat4 m_transformationMatrix;

    reactphysics3d::RigidBody* mp_rigidBody;
    reactphysics3d::Collider* mp_collider;
};