#include <optional>
#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "math/algorithms/Algorithms.hpp"
#include "math/transform/Mat4.hpp"
#include "math/transform/Quaternion.hpp"

#include "quartz/physics/field/Field.hpp"
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
    std::optional<quartz::physics::Field>& o_field,
    const std::optional<std::string>& o_objectFilepath,
    const math::Transform& transform,
    const std::optional<quartz::physics::RigidBody::Parameters>& o_rigidBodyParameters,
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
            std::optional<quartz::physics::RigidBody>(o_field->createRigidBody(physicsManager, m_transform, *o_rigidBodyParameters)) :
            std::nullopt
    ),
    m_fixedUpdateCallback(fixedUpdateCallback),
    m_updateCallback(updateCallback)
{
    LOG_FUNCTION_CALL_TRACEthis("");
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(SCENE, "  position = {}", m_transform.position.toString());
    LOG_TRACE(SCENE, "  rotation = {}", m_transform.rotation.toString());
    LOG_TRACE(SCENE, "  scale    = {}", m_transform.scale.toString());
}

quartz::scene::Doodad::Doodad(
    UNUSED const quartz::rendering::Device& renderingDevice,
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
            std::optional<quartz::physics::RigidBody>(o_field->createRigidBody(physicsManager, m_transform, *doodadParameters.o_rigidBodyParameters)) :
            std::nullopt
    ),
    m_fixedUpdateCallback(doodadParameters.fixedUpdateCallback),
    m_updateCallback(doodadParameters.updateCallback)
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
    mo_model(std::move(other.mo_model)),
    m_transform(other.m_transform),
    m_transformationMatrix(other.m_transformationMatrix),
    mo_rigidBody(std::move(other.mo_rigidBody)),
    m_fixedUpdateCallback(std::move(other.m_fixedUpdateCallback)),
    m_updateCallback(std::move(other.m_updateCallback))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Doodad::~Doodad() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Doodad::fixedUpdate(
    const quartz::managers::InputManager& inputManager
) {
    /**
     * @todo 2024/06/21 Update m_currentTransform here????
     */

    if (m_fixedUpdateCallback) {
        auto safeFixedUpdateCallback = [&] () { m_fixedUpdateCallback(this, inputManager); };
        safeFixedUpdateCallback();
    }

//    mp_rigidBody->applyLocalForceAtCenterOfMass({1.0, 0.0, 0.0});
}

void
quartz::scene::Doodad::update(
    const quartz::managers::InputManager& inputManager,
    UNUSED const double frameTimeDelta,
    const double frameInterpolationFactor
) {
    if (m_updateCallback) {
        auto safeUpdateCallback = [&] () noexcept { m_updateCallback(this, inputManager, frameTimeDelta, frameInterpolationFactor); };
        safeUpdateCallback();
    }

    math::Transform currentTransform;
    if (mo_rigidBody) {
        currentTransform.position = mo_rigidBody->getPosition();
        currentTransform.rotation = mo_rigidBody->getOrientation();
        currentTransform.scale = m_transform.scale;
    } else {
        currentTransform = m_transform;
    }

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
