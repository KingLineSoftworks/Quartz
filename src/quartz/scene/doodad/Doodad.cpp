#include <string>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "quartz/scene/doodad/Doodad.hpp"

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath,
    const quartz::scene::Transform& transform
) :
    m_model(
        renderingDevice,
        objectFilepath
    ),
    m_transform(transform),
    m_transformationMatrix()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Doodad::Doodad(
    quartz::scene::Doodad&& other
) :
    m_model(std::move(other.m_model)),
    m_transformationMatrix(other.m_transformationMatrix)
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
    m_transformationMatrix = glm::mat4(1.0f);

    m_transformationMatrix = glm::translate(
        m_transformationMatrix,
        m_transform.position
    );

    m_transformationMatrix = glm::rotate(
        m_transformationMatrix,
        glm::radians(m_transform.rotationAmountDegrees),
        m_transform.rotationAxis
    );

    m_transformationMatrix = glm::scale(
        m_transformationMatrix,
        m_transform.scale
    );
}