#include "quartz/scene/sky_box/SkyBox.hpp"

quartz::scene::SkyBox::SkyBox() :
    m_cubeMap()
{}

quartz::scene::SkyBox::SkyBox(
    const quartz::rendering::Device& renderingDevice,
    const std::string& frontFilepath,
    const std::string& backFilepath,
    const std::string& upFilepath,
    const std::string& downFilepath,
    const std::string& rightFilepath,
    const std::string& leftFilepath

) :
    m_cubeMap(
        renderingDevice,
        frontFilepath,
        backFilepath,
        upFilepath,
        downFilepath,
        rightFilepath,
        leftFilepath
    )
{}

quartz::scene::SkyBox::SkyBox(
    quartz::scene::SkyBox&& other
) :
    m_cubeMap(std::move(other.m_cubeMap))
{}

quartz::scene::SkyBox::~SkyBox() {}

quartz::scene::SkyBox&
quartz::scene::SkyBox::operator=(
    quartz::scene::SkyBox&& other
) {
    if (this == &other) {
        return *this;
    }

    m_cubeMap = std::move(other.m_cubeMap);

    return *this;
}
