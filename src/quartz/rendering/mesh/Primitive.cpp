#include "quartz/rendering/mesh/Primitive.hpp"

quartz::rendering::Primitive::Primitive(
    const std::vector<uint32_t>& indices
) :
    m_indices(indices)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::Primitive(
    const quartz::rendering::Primitive& other
) :
    m_indices(other.m_indices)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::Primitive(
    quartz::rendering::Primitive&& other
) :
    m_indices(other.m_indices)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::~Primitive()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}