#include <cstdlib>
#include <stdexcept>

#include <reactphysics3d/reactphysics3d.h>

#include "math/Loggers.hpp"

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/core.hpp"
#include "quartz/application/Application.hpp"

#include "demo_app/core.hpp"
#include "demo_app/Loggers.hpp"
#include "demo_app/SceneParameters.hpp"

int main() {
    constexpr bool shouldLogPreamble = false;

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
        {"GENERAL", util::Logger::Level::trace},
        {"BIGBOY", util::Logger::Level::trace},

        // math
        {"TRANSFORM", util::Logger::Level::trace},

        // util
        {"FILESYSTEM", util::Logger::Level::trace},

        // quartz
        {"APPLICATION", util::Logger::Level::trace},

        // managers
        {"INPUTMAN", util::Logger::Level::trace},
        {"PHYSICSMAN", util::Logger::Level::trace},

        // physics
        {"COLLIDER", util::Logger::Level::trace},
        {"COLLIDER_BOX", util::Logger::Level::trace},
        {"COLLIDER_SPHERE", util::Logger::Level::trace},
        {"RIGIDBODY", util::Logger::Level::trace},

        // rendering
        {"INPUTMAN", util::Logger::Level::trace},
        {"PHYSICSMAN", util::Logger::Level::trace},

        // rendering
        {"BUFFER", util::Logger::Level::trace},
        {"BUFFER_MAPPED", util::Logger::Level::trace},
        {"BUFFER_IMAGE", util::Logger::Level::trace},
        {"BUFFER_STAGED", util::Logger::Level::trace},
        {"CONTEXT", util::Logger::Level::trace},
        {"CUBEMAP", util::Logger::Level::trace},
        {"DEPTHBUFFER", util::Logger::Level::trace},
        {"DEVICE", util::Logger::Level::trace},
        {"IMAGE", util::Logger::Level::trace},
        {"INSTANCE", util::Logger::Level::trace},
        {"MATERIAL", util::Logger::Level::trace},
        {"MODEL", util::Logger::Level::trace},
        {"MODEL_MESH", util::Logger::Level::trace},
        {"MODEL_PRIMITIVE", util::Logger::Level::trace},
        {"MODEL_NODE", util::Logger::Level::trace},
        {"MODEL_SCENE", util::Logger::Level::trace},
        {"PIPELINE", util::Logger::Level::trace},
        {"RENDERPASS", util::Logger::Level::trace},
        {"SWAPCHAIN", util::Logger::Level::trace},
        {"TEXTURE", util::Logger::Level::trace},
        {"VULKAN", util::Logger::Level::trace},
        {"VULKANUTIL", util::Logger::Level::trace},
        {"WINDOW", util::Logger::Level::trace},

        // scene
        {"CAMERA", util::Logger::Level::trace},
        {"DOODAD", util::Logger::Level::trace},
        {"SCENE", util::Logger::Level::trace},
        {"SKYBOX", util::Logger::Level::trace},
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

#ifdef QUARTZ_RELEASE
    const bool validationLayersEnabled = false;
#else
    const bool validationLayersEnabled = true;
#endif

    if (!glfwInit()) {
        LOG_CRITICAL(GENERAL, "Failed to initialize GLFW");
        return EXIT_FAILURE;
    }
    LOG_INFO(GENERAL, "GLFW initialized");

    quartz::Application application(
        APPLICATION_NAME,
        APPLICATION_MAJOR_VERSION,
        APPLICATION_MINOR_VERSION,
        APPLICATION_PATCH_VERSION,
        800,
        600,
        validationLayersEnabled,
        getSceneParameters()
    );

    try {
        application.run();
    } catch (const std::exception& e) {
        LOG_CRITICAL(GENERAL, "Caught exception");
        LOG_CRITICAL(GENERAL, "{}", e.what());
        return EXIT_FAILURE;
    }

    LOG_TRACE(GENERAL, "Terminating application");

    return EXIT_SUCCESS;
}
