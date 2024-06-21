#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"

#include "quartz/scene/doodad/Doodad.hpp"

reactphysics3d::RigidBody*
quartz::scene::Doodad::createRigidBodyPtr(
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const quartz::scene::Transform& transform
) {
    if (!p_physicsWorld) {
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

    const math::Quaternion quat = math::Quaternion::fromAxisAngleRotation(transform.rotationAxis.normalize(), transform.rotationAmountDegrees);
    const reactphysics3d::Transform rp3dTransform(transform.position, quat);
    reactphysics3d::RigidBody* p_rigidBody = p_physicsWorld->createRigidBody(rp3dTransform);

    p_rigidBody->setType(reactphysics3d::BodyType::DYNAMIC);
    p_rigidBody->enableGravity(true);
    p_rigidBody->setLinearDamping(0.0);
    p_rigidBody->setAngularDamping(0.0);
    p_rigidBody->setIsAllowedToSleep(true);

    return p_rigidBody;
}

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath,
    const quartz::scene::Transform& transform,
    reactphysics3d::PhysicsWorld* p_physicsWorld
) :
    m_model(
        renderingDevice,
        objectFilepath
    ),
    m_previousTransform(transform),
    m_currentTransform(transform),
    m_transformationMatrix(),
    mp_rigidBody(quartz::scene::Doodad::createRigidBodyPtr(p_physicsWorld, transform))
{
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(SCENE, "  position         = {}", transform.position.toString());
    LOG_TRACE(SCENE, "  rotation degrees = {}", transform.rotationAmountDegrees);
    LOG_TRACE(SCENE, "  rotation axis    = {}", transform.rotationAxis.toString());
    LOG_TRACE(SCENE, "  scale            = {}", transform.scale.toString());
}

quartz::scene::Doodad::Doodad(
    quartz::scene::Doodad&& other
) :
    m_model(std::move(other.m_model)),
    m_currentTransform(other.m_currentTransform),
    m_transformationMatrix(other.m_transformationMatrix),
    mp_rigidBody(std::move(other.mp_rigidBody))
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
     */

    mp_rigidBody->applyLocalForceAtCenterOfMass({1.0, 0.0, 0.0});
}

void
quartz::scene::Doodad::update(
    UNUSED const double frameTimeDelta,
    UNUSED const double frameInterpolationFactor
) {
    const reactphysics3d::Transform& currentRP3DTransform = mp_rigidBody->getTransform();
    const math::Quaternion currentQuat = currentRP3DTransform.getOrientation();

    m_currentTransform.position = currentRP3DTransform.getPosition();
    m_currentTransform.rotationAmountDegrees = currentQuat.getAngleDegrees();
    m_currentTransform.rotationAxis = currentQuat.getAxis(m_currentTransform.rotationAmountDegrees);

    const math::Vec3 interpolatedPosition = math::lerp(
        m_previousTransform.position,
        m_currentTransform.position,
        frameInterpolationFactor
    );

    const math::Quaternion previousQuat = math::Quaternion::fromAxisAngleRotation(
        m_previousTransform.rotationAxis.normalize(),
        m_previousTransform.rotationAmountDegrees
    );
    const math::Quaternion interpolatedQuat = math::Quaternion::slerpShortestPath(
        previousQuat,
        currentQuat,
        frameInterpolationFactor
    );
    const float interpolatedRotationAmountDegrees = interpolatedQuat.getAngleDegrees();
    const math::Vec3 interpolatedRotationAxis = interpolatedQuat.getAxis(interpolatedRotationAmountDegrees);

    const math::Vec3 interpolatedScale = math::lerp(
        m_previousTransform.scale,
        m_currentTransform.scale,
        frameInterpolationFactor
    );

    m_transformationMatrix = math::Mat4(1.0f);
    m_transformationMatrix.translate(interpolatedPosition);
    m_transformationMatrix.rotate(interpolatedRotationAxis, glm::radians(interpolatedRotationAmountDegrees));
    m_transformationMatrix.scale(interpolatedScale);

    m_previousTransform = m_currentTransform;
}