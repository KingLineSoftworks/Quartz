#include <string>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "quartz/scene/doodad/Doodad.hpp"

quartz::scene::Doodad::Doodad(
    const quartz::rendering::Device& renderingDevice,
    const std::string& objectFilepath,
    const std::string& textureFilepath
) :
    m_model(
        renderingDevice,
        objectFilepath,
        textureFilepath
    ),
    m_modelMatrix()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Doodad::Doodad(
    quartz::scene::Doodad&& other
) :
    m_model(std::move(other.m_model)),
    m_modelMatrix(other.m_modelMatrix)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::scene::Doodad::~Doodad() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void
quartz::scene::Doodad::update(
    const float executionDurationTimeCount
) {
    m_modelMatrix = glm::rotate(
        glm::mat4(1.0f),
        executionDurationTimeCount * glm::radians(90.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
}