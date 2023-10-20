#include "quartz/rendering/context/Model.hpp"

quartz::rendering::Model::Model(
    const quartz::rendering::Device& renderingDevice,
    const std::string& textureFilepath
) :
    m_mesh(renderingDevice),
    m_texture(renderingDevice, textureFilepath)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::Model(quartz::rendering::Model&& other) :
    m_mesh(std::move(other.m_mesh)),
    m_texture(std::move(other.m_texture))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Model::~Model() {
    LOG_FUNCTION_CALL_TRACEthis("");
}