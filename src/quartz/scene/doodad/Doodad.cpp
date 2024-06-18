#include <string>

#include <glm/gtc/matrix_transform.hpp>

#include "math/transform/Mat4.hpp"

#include "quartz/scene/doodad/Doodad.hpp"

reactphysics3d::RigidBody*
quartz::scene::Doodad::createRigidBodyPtr(
    reactphysics3d::PhysicsWorld* p_physicsWorld,
    const quartz::scene::Transform& transform
) {
    if (!p_physicsWorld) {
        return nullptr;
    }

    const reactphysics3d::Quaternion quat; /** @todo 2024/06/17 Create implementation for conversion from axis angle rotation to quaternion */
    const reactphysics3d::Transform rp3dTransform(transform.position, quat);
    return p_physicsWorld->createRigidBody(rp3dTransform);
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
    m_transform(transform),
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
    m_transform(other.m_transform),
    m_transformationMatrix(other.m_transformationMatrix),
    mp_rigidBody(std::move(other.mp_rigidBody))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Doodad::~Doodad() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Doodad::update(
    UNUSED const double tickTimeDelta
) {
    m_transformationMatrix = math::Mat4(1.0f);

    m_transformationMatrix.translate(m_transform.position);

    m_transformationMatrix.rotate(m_transform.rotationAxis, glm::radians(m_transform.rotationAmountDegrees));

    m_transformationMatrix.scale(m_transform.scale);
}