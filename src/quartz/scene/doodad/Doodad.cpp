#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"

#include "quartz/scene/doodad/Doodad.hpp"

math::Transform
quartz::scene::Doodad::fixTransform(
    const math::Transform& transform
) {
    math::Transform fixedTransform = transform;

    if (fixedTransform.rotation.magnitude() == 0.0f) {
        fixedTransform.rotation = math::Quaternion::fromAxisAngleRotation(math::Vec3(0.0f, 1.0f, 0.0f), 0.0f);
    }

    return fixedTransform;
}

reactphysics3d::RigidBody*
quartz::scene::Doodad::createRigidBodyPtr(
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const std::optional<quartz::scene::PhysicsProperties>& o_physicsProperties,
    const math::Transform& transform
) {
    LOG_FUNCTION_SCOPE_TRACE(DOODAD, "");

    if (!p_physicsWorld) {
        LOG_DEBUG(DOODAD, "Invalid physics world pointer supplied, not creating rigidbody");
        return nullptr;
    }

    if (!o_physicsProperties) {
        LOG_DEBUG(DOODAD, "No physics properties supplied, not creating rigidbody");
        return nullptr;
    }

    /**
     * @todo 2024/06/20 Take in a physics settings struct (or something similar) so we have all the properties
     *    we need to do things like the following:
     * @todo 2024/06/20 Create a collider for the rigid body
     * @todo 2024/06/20 Set the type of the rigid body (static, kinematic, dynamic)
     * @todo 2024/06/20 Determine if we want to interact with gravity
     * @todo 2024/06/20 Set the angular and linear velocity damping to simulate things like air friction when there is no collision happening
     */

    const reactphysics3d::Transform rp3dTransform(transform.position, transform.rotation);
    reactphysics3d::RigidBody* p_rigidBody = p_physicsWorld->createRigidBody(rp3dTransform);

    const std::string bodyTypeString = quartz::scene::PhysicsProperties::getBodyTypeString(o_physicsProperties->bodyType);
    LOG_TRACE(DOODAD, "Using body type : {}", bodyTypeString);
    LOG_TRACE(DOODAD, "Enabling gravity: {}", o_physicsProperties->enableGravity);
    p_rigidBody->setType(o_physicsProperties->bodyType);
    p_rigidBody->enableGravity(o_physicsProperties->enableGravity);
    p_rigidBody->setLinearDamping(0.0);
    p_rigidBody->setAngularDamping(0.0);
    p_rigidBody->setIsAllowedToSleep(true);

    return p_rigidBody;
}

reactphysics3d::Collider*
quartz::scene::Doodad::createColliderPtr(
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::RigidBody* p_rigidBody,
    UNUSED const math::Transform& transform
) {
    const math::Vec3 colliderHalfExtents = transform.scale; // because box is by default 1 unit in each direction
    reactphysics3d::BoxShape* p_colliderShape = physicsManager.createBoxShapePtr(colliderHalfExtents);
    LOG_TRACE(DOODAD, "Creating box collider with half extents of {}", colliderHalfExtents.toString());
    LOG_TRACE(DOODAD, "  with {} vertices", p_colliderShape->getNbVertices());
    for (uint32_t i = 0; i < p_colliderShape->getNbVertices(); ++i) {
        UNUSED const uint32_t vertexIndex = p_colliderShape->getVertex(i).vertexPointIndex;
        UNUSED const math::Vec3 vertexPosition = p_colliderShape->getVertexPosition(i);
        LOG_TRACE(DOODAD, "    vertex {} -> index {} with position {}", i, vertexIndex, vertexPosition.toString());
    }

    reactphysics3d::Transform colliderTransform = reactphysics3d::Transform::identity(); // transform relative to the body, not the world
    reactphysics3d::Collider* p_collider = p_rigidBody->addCollider(p_colliderShape, colliderTransform);

    return p_collider;
}

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const std::string& objectFilepath,
    const math::Transform& transform,
    const std::optional<quartz::scene::PhysicsProperties>& o_physicsProperties,
    const quartz::physics::RigidBody::Parameters& rigidBodyParameters
) :
    m_model(
        renderingDevice,
        objectFilepath
    ),
    m_transform(quartz::scene::Doodad::fixTransform(transform)),
    m_transformationMatrix(),
    mp_rigidBody(quartz::scene::Doodad::createRigidBodyPtr(p_physicsWorld, o_physicsProperties, m_transform)),
    mp_collider(quartz::scene::Doodad::createColliderPtr(physicsManager, mp_rigidBody, m_transform)),
    mo_rigidBody({physicsManager, p_physicsWorld, m_transform, rigidBodyParameters})
{
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(SCENE, "  position = {}", transform.position.toString());
    LOG_TRACE(SCENE, "  rotation = {}", transform.rotation.toString());
    LOG_TRACE(SCENE, "  scale    = {}", transform.scale.toString());
}

quartz::scene::Doodad::Doodad(
    quartz::scene::Doodad&& other
) :
    m_model(std::move(other.m_model)),
    m_transform(other.m_transform),
    m_transformationMatrix(other.m_transformationMatrix),
    mp_rigidBody(std::move(other.mp_rigidBody)),
    mo_rigidBody(std::move(other.mo_rigidBody))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Doodad::~Doodad() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Doodad::fixedUpdate() {
    /**
     * @todo 2024/06/20 Call the fixed update callback given to the doodad that actual contains
     *    the logic that we execute here
     *
     * @todo 2024/06/21 Update m_currentTransform here????
     */

//    mp_rigidBody->applyLocalForceAtCenterOfMass({1.0, 0.0, 0.0});
}

void
quartz::scene::Doodad::update(
    UNUSED const double frameTimeDelta,
    UNUSED const double frameInterpolationFactor
) {
    math::Transform currentTransform;
    currentTransform.position = mp_rigidBody->getTransform().getPosition();
    currentTransform.rotation = mp_rigidBody->getTransform().getOrientation();
    currentTransform.scale = m_transform.scale;

    const math::Vec3 interpolatedPosition = math::lerp(
        m_transform.position,
        currentTransform.position,
        frameInterpolationFactor
    );

    const math::Quaternion interpolatedRotation = math::Quaternion::slerp(
        m_transform.rotation.normalize(),
        currentTransform.rotation.normalize(),
        frameInterpolationFactor
    );

    const math::Vec3 interpolatedScale = math::lerp(
        m_transform.scale,
        currentTransform.scale,
        frameInterpolationFactor
    );

    math::Mat4 translationMatrix = math::Mat4::translate(math::Mat4(1.0f), interpolatedPosition);
    math::Mat4 rotationMatrix = interpolatedRotation.getRotationMatrix();
    math::Mat4 scaleMatrix = math::Mat4::scale(math::Mat4(1.0), interpolatedScale);

    m_transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    m_transform = currentTransform;
}