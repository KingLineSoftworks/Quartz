#pragma once

#include "quartz/rendering/cube_map/CubeMap.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/scene/Loggers.hpp"

namespace quartz {
namespace scene {
    class SkyBox;
}
}

class quartz::scene::SkyBox {
public: // member functions
    SkyBox();
    SkyBox(
        const quartz::rendering::Device& renderingDevice,
        const std::string& frontFilepath,
        const std::string& backFilepath,
        const std::string& upFilepath,
        const std::string& downFilepath,
        const std::string& rightFilepath,
        const std::string& leftFilepath
    );
    SkyBox(SkyBox&& other);
    ~SkyBox();

    SkyBox& operator=(SkyBox&& other);

    USE_LOGGER(SKYBOX);

    const quartz::rendering::CubeMap& getCubeMap() const { return m_cubeMap; }

private: // member variables
    quartz::rendering::CubeMap m_cubeMap;
};
