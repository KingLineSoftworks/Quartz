#include <string>

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

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
    LOG_TRACEthis("Constructing doodad with transform:");
    LOG_TRACE(SCENE, "  position         = {}", glm::to_string(transform.position));
    LOG_TRACE(SCENE, "  rotation degrees = {}", transform.rotationAmountDegrees);
    LOG_TRACE(SCENE, "  rotation axis    = {}", glm::to_string(transform.rotationAxis));
    LOG_TRACE(SCENE, "  scale            = {}", glm::to_string(transform.scale));
}

quartz::scene::Doodad::Doodad(
    quartz::scene::Doodad&& other
) :
    m_model(std::move(other.m_model)),
    m_transform(other.m_transform),
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
    LOG_FUNCTION_SCOPE_TRACEthis("");
    m_transformationMatrix = glm::mat4(1.0f);

//    LOG_TRACEthis("initial transformation matrix:");
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[0]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[1]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[2]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[3]));

    m_transformationMatrix = glm::translate(
        m_transformationMatrix,
        m_transform.position
    );

//    LOG_TRACEthis("");
//    LOG_TRACEthis("translated transformation matrix:");
//    LOG_TRACEthis("using {}", glm::to_string(m_transform.position));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[0]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[1]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[2]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[3]));

    m_transformationMatrix = glm::rotate(
        m_transformationMatrix,
        glm::radians(m_transform.rotationAmountDegrees),
        m_transform.rotationAxis
    );

//    LOG_TRACEthis("");
//    LOG_TRACEthis("rotated transformation matrix:");
//    LOG_TRACEthis("using {} around {}", m_transform.rotationAmountDegrees, glm::to_string(m_transform.rotationAxis));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[0]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[1]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[2]));
//    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[3]));

    m_transformationMatrix = glm::scale(
        m_transformationMatrix,
        m_transform.scale
    );

//    LOG_TRACEthis("");
    LOG_TRACEthis("scaled transformation matrix:");
    LOG_TRACEthis("using {}", glm::to_string(m_transform.scale));
    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[0]));
    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[1]));
    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[2]));
    LOG_TRACEthis("{}", glm::to_string(m_transformationMatrix[3]));
}