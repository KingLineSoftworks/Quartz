#pragma once

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/context/Buffer.hpp"
#include "quartz/rendering/context/Device.hpp"

namespace quartz {
namespace rendering {
    class Texture;
}
}

class quartz::rendering::Texture {
public: // member functions
    Texture(
        const quartz::rendering::Device& renderingDevice
    );
    ~Texture();

    USE_LOGGER(TEXTURE);

private: // static functions

private: // member variables

};