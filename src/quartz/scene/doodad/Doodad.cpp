#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"
#include "quartz/physics/realm/Realm.hpp"

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

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    quartz::physics::Realm& physicsRealm,
    const std::string& objectFilepath,
    const math::Transform& transform,
    const quartz::physics::RigidBody::Parameters& rigidBodyParameters
) :
    m_model(
        renderingDevice,
        objectFilepath
    ),
    m_transform(quartz::scene::Doodad::fixTransform(transform)),
    m_transformationMatrix(),
    mo_rigidBody(physicsRealm.createRigidBody(physicsManager, m_transform, rigidBodyParameters))
{
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(SCENE, "  position = {}", m_transform.position.toString());
    LOG_TRACE(SCENE, "  rotation = {}", m_transform.rotation.toString());
    LOG_TRACE(SCENE, "  scale    = {}", m_transform.scale.toString());
}

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    quartz::physics::Realm& physicsRealm,
    quartz::scene::Doodad::Parameters& doodadParameters
) :
    m_model(
        renderingDevice,
        doodadParameters.objectFilepath
    ),
    m_transform(quartz::scene::Doodad::fixTransform(doodadParameters.transform)),
    m_transformationMatrix(),
    mo_rigidBody(physicsRealm.createRigidBody(physicsManager, m_transform, doodadParameters.rigidBodyParameters))
{
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(SCENE, "  position = {}", m_transform.position.toString());
    LOG_TRACE(SCENE, "  rotation = {}", m_transform.rotation.toString());
    LOG_TRACE(SCENE, "  scale    = {}", m_transform.scale.toString());
}

quartz::scene::Doodad::Doodad(
    quartz::scene::Doodad&& other
) :
    m_model(std::move(other.m_model)),
    m_transform(other.m_transform),
    m_transformationMatrix(other.m_transformationMatrix),
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
     * @todo 2024/06/20 Call the fixed update callback given to the doodad which actually contains
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
    /** @todo 2024/11/06 Ensure optional rigidbody is valid before trying to get its members */
    currentTransform.position = mo_rigidBody->getPosition();
    currentTransform.rotation = mo_rigidBody->getOrientation();
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
