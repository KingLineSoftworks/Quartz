#include "math/Loggers.hpp"

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/core.hpp"
#include "quartz/Loggers.hpp"
#include "quartz/managers/Loggers.hpp"
#include "quartz/physics/Loggers.hpp"
#include "quartz/rendering/Loggers.hpp"
#include "quartz/scene/Loggers.hpp"
#include "quartz/application/Application.hpp"

#include "demo_app/core.hpp"
#include "demo_app/Loggers.hpp"
#include "demo_app/Boilerplate.hpp"

int
DoBoilerplateStuff(
    const bool shouldLogPreamble
) {
    ASSERT_QUARTZ_VERSION();
    ASSERT_APPLICATION_VERSION();

    util::Logger::setShouldLogPreamble(shouldLogPreamble);

    REGISTER_LOGGER_GROUP(MATH);
    REGISTER_LOGGER_GROUP(UTIL);
    REGISTER_LOGGER_GROUP(QUARTZ);
    REGISTER_LOGGER_GROUP(QUARTZ_MANAGERS);
    REGISTER_LOGGER_GROUP(QUARTZ_PHYSICS);
    REGISTER_LOGGER_GROUP(QUARTZ_RENDERING);
    REGISTER_LOGGER_GROUP(QUARTZ_SCENE);
    REGISTER_LOGGER_GROUP(DEMO_APP);

    util::Logger::setLevels({
        // demo app
        {"GENERAL", util::Logger::Level::info},
        {"PLAYER", util::Logger::Level::info},

        // math
        {"TRANSFORM", util::Logger::Level::info},

        // util
        {"FILESYSTEM", util::Logger::Level::info},

        // quartz
        {"APPLICATION", util::Logger::Level::info},

        // managers
        {"INPUTMAN", util::Logger::Level::info},
        {"PHYSICSMAN", util::Logger::Level::info},

        // physics
        {"COLLIDER", util::Logger::Level::info},
        {"SHAPE_BOX", util::Logger::Level::info},
        {"SHAPE_SPHERE", util::Logger::Level::info},
        {"FIELD", util::Logger::Level::info},
        {"RIGIDBODY", util::Logger::Level::info},

        // rendering
        {"INPUTMAN", util::Logger::Level::info},
        {"PHYSICSMAN", util::Logger::Level::info},
        {"SCENEMAN", util::Logger::Level::info},

        // rendering
        {"BUFFER", util::Logger::Level::info},
        {"BUFFER_MAPPED", util::Logger::Level::info},
        {"BUFFER_IMAGE", util::Logger::Level::info},
        {"BUFFER_STAGED", util::Logger::Level::info},
        {"CONTEXT", util::Logger::Level::info},
        {"CUBEMAP", util::Logger::Level::info},
        {"DEPTHBUFFER", util::Logger::Level::info},
        {"DEVICE", util::Logger::Level::info},
        {"IMAGE", util::Logger::Level::info},
        {"INSTANCE", util::Logger::Level::info},
        {"MATERIAL", util::Logger::Level::info},
        {"MODEL", util::Logger::Level::info},
        {"MODEL_MESH", util::Logger::Level::info},
        {"MODEL_PRIMITIVE", util::Logger::Level::info},
        {"MODEL_NODE", util::Logger::Level::info},
        {"MODEL_SCENE", util::Logger::Level::info},
        {"PIPELINE", util::Logger::Level::info},
        {"RENDERPASS", util::Logger::Level::info},
        {"SWAPCHAIN", util::Logger::Level::info},
        {"TEXTURE", util::Logger::Level::info},
        {"VULKAN", util::Logger::Level::info},
        {"VULKANUTIL", util::Logger::Level::info},
        {"WINDOW", util::Logger::Level::info},

        // scene
        {"CAMERA", util::Logger::Level::debug},
        {"DOODAD", util::Logger::Level::debug},
        {"SCENE", util::Logger::Level::debug},
        {"SKYBOX", util::Logger::Level::info},
    });

    if (shouldLogPreamble) {
        LOG_INFO(GENERAL, "Quartz version   : {}.{}.{}", QUARTZ_MAJOR_VERSION, QUARTZ_MINOR_VERSION, QUARTZ_PATCH_VERSION);
        LOG_INFO(GENERAL, "Demo app version : {}.{}.{}", APPLICATION_MAJOR_VERSION, APPLICATION_MINOR_VERSION, APPLICATION_PATCH_VERSION);

#ifdef DEBUG
        LOG_INFO(GENERAL, "Quartz built with DEBUG definition");
#endif
#ifdef _DEBUG
        LOG_INFO(GENERAL, "Quartz built with _DEBUG definition");
#endif
#ifdef NDEBUG
        LOG_INFO(GENERAL, "Quartz built with NDEBUG definition");
#endif
#ifdef RELEASE
        LOG_INFO(GENERAL, "Quartz built with RELEASE definition");
#endif

#ifdef QUARTZ_DEBUG
        LOG_INFO(GENERAL, "Quartz built in debug mode ( QUARTZ_DEBUG )");
#endif
#ifdef QUARTZ_TEST
        LOG_INFO(GENERAL, "Quartz built in test mode ( QUARTZ_TEST )");
#endif
#ifdef QUARTZ_RELEASE
        LOG_INFO(GENERAL, "Quartz built in release mode ( QUARTZ_RELEASE )");
#endif

#ifdef ON_MAC
        LOG_INFO(GENERAL, "On Mac ( ON_MAC )");
        LOG_INFO(GENERAL, "*Bad* Mac version defined as {} ( MAC_VERSION_BAD )", MAC_VERSION_BAD);
        LOG_INFO(GENERAL, "Mac version defined as {} ( MAC_VERSION )", MAC_VERSION);
#endif

#ifdef ON_LINUX
        LOG_INFO(GENERAL, "On Linux ( ON_LINUX )");
#endif
    }

    if (!glfwInit()) {
        LOG_CRITICAL(GENERAL, "Failed to initialize GLFW");
        return EXIT_FAILURE;
    }
    LOG_INFO(GENERAL, "GLFW initialized");

    return 0;
}
