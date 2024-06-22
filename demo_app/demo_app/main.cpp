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

void printVec3(const glm::vec3& vec3) {
    LOG_TRACE(BIGBOY, "Vector3Union glm cast  : {}, {}, {}", vec3.x, vec3.y, vec3.z);
}

void printVector3(const reactphysics3d::Vector3 vec3) {
    LOG_TRACE(BIGBOY, "Vector3Union rp3d cast : {}, {}, {}", vec3.x, vec3.y, vec3.z);
}

int main() {
    constexpr bool shouldLogPreamble = false;

    ASSERT_QUARTZ_VERSION();
    ASSERT_APPLICATION_VERSION();

    util::Logger::setShouldLogPreamble(shouldLogPreamble);

    REGISTER_LOGGER_GROUP(MATH);
    REGISTER_LOGGER_GROUP(UTIL);
    REGISTER_LOGGER_GROUP(QUARTZ);
    REGISTER_LOGGER_GROUP(QUARTZ_MANAGERS);
    REGISTER_LOGGER_GROUP(QUARTZ_RENDERING);
    REGISTER_LOGGER_GROUP(QUARTZ_SCENE);
    REGISTER_LOGGER_GROUP(DEMO_APP);

    util::Logger::setLevels({
        // demo app
        {"GENERAL", util::Logger::Level::info},
        {"BIGBOY", util::Logger::Level::trace},

        // math
        {"TRANSFORM", util::Logger::Level::info},

        // util
        {"FILESYSTEM", util::Logger::Level::info},

        // quartz
        {"APPLICATION", util::Logger::Level::info},

        // rendering
        {"INPUTMAN", util::Logger::Level::info},
        {"PHYSICSMAN", util::Logger::Level::info},

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
        {"CAMERA", util::Logger::Level::info},
        {"DOODAD", util::Logger::Level::trace},
        {"SCENE", util::Logger::Level::info},
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
        validationLayersEnabled
    );

    try {
        application.run();
    } catch (const std::exception& e) {
        LOG_CRITICAL(GENERAL, "Caught exception");
        LOG_CRITICAL(GENERAL, "{}", e.what());
        return EXIT_FAILURE;
    }

    LOG_TRACE(GENERAL, "Terminating application");

#if false
    LOG_TRACE(GENERAL, "Performing physics simulation");
    reactphysics3d::PhysicsCommon physicsCommon;
    reactphysics3d::PhysicsWorld* p_physicsWorld = physicsCommon.createPhysicsWorld();

    reactphysics3d::Vector3 position(0, 20, 0);
    reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion::identity();
    reactphysics3d::Transform transform(position, orientation);
    reactphysics3d::RigidBody* p_rigidBody = p_physicsWorld->createRigidBody(transform);

    const uint32_t simulationTimeSec = 120;
    const reactphysics3d::decimal timeStep = 1.0f / 60.0f;
    for (uint32_t i = 0; i < simulationTimeSec; ++i) {
        p_physicsWorld->update(timeStep);
        const reactphysics3d::Transform& currTransform = p_rigidBody->getTransform();
        const reactphysics3d::Vector3& currPosition = currTransform.getPosition();

        LOG_TRACE(BIGBOY, "Rigid body position: {:10.6f}, {:10.6f}, {:10.6f}", currPosition.x, currPosition.y, currPosition.z);
    }

    // These will automatically get destroyed when the physics common goes out of scope but I think it'd be best practice to do them manually
    p_physicsWorld->destroyRigidBody(p_rigidBody);
    physicsCommon.destroyPhysicsWorld(p_physicsWorld);
#endif

    return EXIT_SUCCESS;
}
