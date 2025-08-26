#include <optional>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"

#include "util/logger/Logger.hpp"

#include "quartz/physics/field/Field.hpp"
#include "quartz/scene/doodad/Doodad.hpp"

void
quartz::scene::Doodad::noopAwakenCallback(
    UNUSED quartz::scene::Doodad::AwakenCallbackParameters parameters
) {}

void
quartz::scene::Doodad::noopFixedUpdateCallback(
    UNUSED quartz::scene::Doodad::FixedUpdateCallbackParameters parameters
) {}

void
quartz::scene::Doodad::noopUpdateCallback(
    UNUSED quartz::scene::Doodad::UpdateCallbackParameters parameters
) {}

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

math::Mat4
quartz::scene::Doodad::calculateTransformationmatrix(
    const math::Transform &currentTransform,
    const math::Transform &nextTransform,
    const double interpolationFactor
) {
    const math::Vec3 interpolatedPosition = math::lerp(
        currentTransform.position,
        nextTransform.position,
        interpolationFactor
    );

    const math::Quaternion interpolatedRotation = math::Quaternion::slerp(
        currentTransform.rotation.normalize(),
        nextTransform.rotation.normalize(),
        interpolationFactor
    );

    const math::Vec3 interpolatedScale = math::lerp(
        currentTransform.scale,
        nextTransform.scale,
        interpolationFactor
    );

#if false
    const math::Mat4 translationMatrix = math::Mat4::translate(math::Mat4(1.0f), interpolatedPosition);
    const math::Mat4 rotationMatrix = interpolatedRotation.getRotationMatrix();
    const math::Mat4 scaleMatrix = math::Mat4::scale(math::Mat4(1.0), interpolatedScale);

    const math::Mat4 transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;

    return transformationMatrix;
#else
    const math::Transform interpolatedTransform(
        interpolatedPosition,
        interpolatedRotation,
        interpolatedScale
    );

    return interpolatedTransform.calculateTransformationMatrix();
#endif
}

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    std::optional<quartz::physics::Field>& o_field,
    const std::optional<std::string>& o_objectFilepath,
    const math::Transform& transform,
    const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters,
    const quartz::scene::Doodad::AwakenCallback& awakenCallback,
    const quartz::scene::Doodad::FixedUpdateCallback& fixedUpdateCallback,
    const quartz::scene::Doodad::UpdateCallback& updateCallback
) :
    mo_model(
        o_objectFilepath ?
            std::optional<quartz::rendering::Model>(quartz::rendering::Model(renderingDevice, *o_objectFilepath)) :
            std::nullopt
    ),
    m_transform(quartz::scene::Doodad::fixTransform(transform)),
    m_transformationMatrix(),
    mo_rigidBody(
        (o_field && o_rigidBodyParameters) ?
            std::optional<quartz::physics::RigidBody>(physicsManager.createRigidBody(*o_field, m_transform, *o_rigidBodyParameters)) :
            std::nullopt
    ),
    m_awakenCallback(awakenCallback ? awakenCallback : quartz::scene::Doodad::noopAwakenCallback),
    m_fixedUpdateCallback(fixedUpdateCallback ? fixedUpdateCallback : quartz::scene::Doodad::noopFixedUpdateCallback),
    m_updateCallback(updateCallback ? updateCallback : quartz::scene::Doodad::noopUpdateCallback)
{
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(DOODAD, "  position = {}", m_transform.position.toString());
    LOG_TRACE(DOODAD, "  rotation = {}", m_transform.rotation.toString());
    LOG_TRACE(DOODAD, "  scale    = {}", m_transform.scale.toString());
}

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    std::optional<quartz::physics::Field>& o_field,
    const quartz::scene::Doodad::Parameters& doodadParameters
) :
    mo_model(
        doodadParameters.o_objectFilepath ?
            std::optional<quartz::rendering::Model>(quartz::rendering::Model(renderingDevice, *doodadParameters.o_objectFilepath)) :
            std::nullopt
    ),
    m_transform(quartz::scene::Doodad::fixTransform(doodadParameters.transform)),
    m_transformationMatrix(),
    mo_rigidBody(
        (o_field && doodadParameters.o_rigidBodyParameters) ?
            std::optional<quartz::physics::RigidBody>(physicsManager.createRigidBody(*o_field, m_transform, *doodadParameters.o_rigidBodyParameters)) :
            std::nullopt
    ),
    m_awakenCallback(doodadParameters.awakenCallback ? doodadParameters.awakenCallback : quartz::scene::Doodad::noopAwakenCallback),
    m_fixedUpdateCallback(doodadParameters.fixedUpdateCallback ? doodadParameters.fixedUpdateCallback : quartz::scene::Doodad::noopFixedUpdateCallback),
    m_updateCallback(doodadParameters.updateCallback ? doodadParameters.updateCallback : quartz::scene::Doodad::noopUpdateCallback)
{
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(DOODAD, "  position = {}", m_transform.position.toString());
    LOG_TRACE(DOODAD, "  rotation = {}", m_transform.rotation.toString());
    LOG_TRACE(DOODAD, "  scale    = {}", m_transform.scale.toString());
}

quartz::scene::Doodad::Doodad(
    quartz::scene::Doodad&& other
) :
    mo_model(std::move(other.mo_model)),
    m_transform(other.m_transform),
    m_transformationMatrix(other.m_transformationMatrix),
    mo_rigidBody(std::move(other.mo_rigidBody)),
    m_awakenCallback(std::move(other.m_awakenCallback)),
    m_fixedUpdateCallback(std::move(other.m_fixedUpdateCallback)),
    m_updateCallback(std::move(other.m_updateCallback))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Doodad::~Doodad() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Doodad::setPosition(
    const math::Vec3& position
) {
    m_transform.position = position;

    if (mo_rigidBody) {
        mo_rigidBody->setPosition(position);
    }
}

void
quartz::scene::Doodad::setRotation(
    const math::Quaternion& rotation
) {
    m_transform.rotation = rotation;

    if (mo_rigidBody) {
        mo_rigidBody->setRotation(rotation);
    }
}

void
quartz::scene::Doodad::setScale(
    const math::Vec3& scale
) {
    m_transform.scale = scale;

    if (mo_rigidBody) {
        mo_rigidBody->setScale(scale);
    }
}

void
quartz::scene::Doodad::snapToRigidBody() {
    if (!mo_rigidBody) {
        return;
    }

    m_transform.position = mo_rigidBody->getPosition();
    m_transform.rotation = mo_rigidBody->getRotation();
}

void
quartz::scene::Doodad::awaken(
    quartz::scene::Scene* p_scene
) {
    LOG_FUNCTION_CALL_DEBUGthis("");
    m_awakenCallback({p_scene});
}

void
quartz::scene::Doodad::fixedUpdate(
    const quartz::managers::InputManager& inputManager,
    const double totalElapsedTime,
    const double tickTimeDelta,
    const double ticksPerSecond
) {
    m_fixedUpdateCallback({this, inputManager, totalElapsedTime, tickTimeDelta, ticksPerSecond});
}

void
quartz::scene::Doodad::update(
    const quartz::managers::InputManager& inputManager,
    const double totalElapsedTime,
    const double frameTimeDelta,
    const double frameInterpolationFactor
) {
    m_updateCallback({this, inputManager, totalElapsedTime, frameTimeDelta, frameInterpolationFactor});

    /**
     * @detail 2024/12/01 We want to update the rigid body's positon and rotation when we update the doodad's
     *    position and rotation in the update callback. This will set the currentTransform to be exactly the
     *    same as our m_transform. This will cause the interpolation between currentTransform and m_transform
     *    to be the same as just choosing one, because they will be equal.
     *    So, we want to still interpolate between the rigid body's transform and our m_transform,
     *    in case we did not update our transform directly via the update callback. If we didn't update our
     *    transform directly via the update callback then we will 
     */

    math::Transform currentTransform;
    if (mo_rigidBody) {
        currentTransform.position = mo_rigidBody->getPosition();
        currentTransform.rotation = mo_rigidBody->getRotation();
        currentTransform.scale = m_transform.scale;
    } else {
        currentTransform = m_transform;
    }

#if false
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

    const math::Mat4 translationMatrix = math::Mat4::translate(math::Mat4(1.0f), interpolatedPosition);
    const math::Mat4 rotationMatrix = interpolatedRotation.getRotationMatrix();
    const math::Mat4 scaleMatrix = math::Mat4::scale(math::Mat4(1.0), interpolatedScale);

    m_transformationMatrix = translationMatrix * rotationMatrix * scaleMatrix;
#else
    m_transformationMatrix = quartz::scene::Doodad::calculateTransformationmatrix(
        m_transform,
        currentTransform,
        frameInterpolationFactor
    );
#endif

    m_transform = currentTransform;
}

