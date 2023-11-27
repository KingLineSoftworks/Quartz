#include "quartz/rendering/mesh/Primitive.hpp"

quartz::rendering::Primitive::Primitive(
    const uint32_t startIndex,
    const uint32_t indexCount
) :
    m_startIndex(startIndex),
    m_indexCount(indexCount)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::Primitive(
    const quartz::rendering::Primitive& other
) :
    m_startIndex(other.m_startIndex),
    m_indexCount(other.m_indexCount)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::Primitive(
    quartz::rendering::Primitive&& other
) :
    m_startIndex(other.m_startIndex),
    m_indexCount(other.m_indexCount)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Primitive::~Primitive()
{
    LOG_FUNCTION_CALL_TRACEthis("");
}