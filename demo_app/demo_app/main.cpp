#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_extension_inspection.hpp>

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/core.hpp"
#include "quartz/something/Something.hpp"

#include "demo_app/core.hpp"
#include "demo_app/Loggers.hpp"

int main() {
    constexpr bool shouldLogPreamble = false;

    ASSERT_QUARTZ_VERSION();
    ASSERT_APPLICATION_VERSION();

    quartz::util::Logger::setShouldLogPreamble(shouldLogPreamble);

    REGISTER_LOGGER_GROUP(QUARTZ);
    REGISTER_LOGGER_GROUP(DEMO_APP);

    quartz::util::Logger::setLevels({
        {"GENERAL", quartz::util::Logger::Level::trace},
        {"BIGBOY", quartz::util::Logger::Level::trace},
        {"ALAMANCY", quartz::util::Logger::Level::trace},
        {"GENERAL2", quartz::util::Logger::Level::trace},
        {"SOMETHING", quartz::util::Logger::Level::trace},
        {"ANOTHER", quartz::util::Logger::Level::trace}
    });

    if (shouldLogPreamble) {
        LOG_INFO(quartz::loggers::GENERAL, "Quartz version   : {}.{}.{}", QUARTZ_MAJOR_VERSION, QUARTZ_MINOR_VERSION, QUARTZ_PATCH_VERSION);
        LOG_INFO(quartz::loggers::GENERAL, "Demo app version : {}.{}.{}", APPLICATION_MAJOR_VERSION, APPLICATION_MINOR_VERSION, APPLICATION_PATCH_VERSION);

#ifdef QUARTZ_DEBUG
        LOG_INFO(quartz::loggers::GENERAL, "Quartz built in debug mode ( QUARTZ_DEBUG )");
#endif
#ifdef QUARTZ_TEST
        LOG_INFO(quartz::loggers::GENERAL, "Quartz built in test mode ( QUARTZ_TEST )");
#endif
#ifdef QUARTZ_RELEASE
        LOG_INFO(quartz::loggers::GENERAL, "Quartz built in release mode ( QUARTZ_RELEASE )");
#endif

#ifdef ON_MAC
        LOG_INFO(quartz::loggers::GENERAL, "On Mac ( ON_MAC )");
        LOG_INFO(quartz::loggers::GENERAL, "*Bad* Mac version defined as {} ( MAC_VERSION_BAD )", MAC_VERSION_BAD);
        LOG_INFO(quartz::loggers::GENERAL, "Mac version defined as {} ( MAC_VERSION )", MAC_VERSION);
#else
        LOG_INFO(quartz::loggers::GENERAL, "Not on Mac ( ON_MAC )");
#endif // ON_MAC
    }

//    quartz::Something something(69, 42.666);
//    something.doSomething();

    /////////////////////////////////////////////////////////////////
    // Setup
    /////////////////////////////////////////////////////////////////

    LOG_TRACE(quartz::loggers::GENERAL, "Initializing GLFW");
    glfwInit();

    LOG_TRACE(quartz::loggers::GENERAL, "Creating GLFW Window");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* p_window = glfwCreateWindow(800, 600, "Demo application window", nullptr, nullptr);

    /////////////////////////////////////////////////////////////////
    // Get instance extension properties
    /////////////////////////////////////////////////////////////////

    LOG_TRACE(quartz::loggers::GENERAL, "Enumerating extension properties");
    const std::set<std::string>& instanceExtensions = vk::getInstanceExtensions();
    LOG_TRACE(quartz::loggers::GENERAL, "{} extensions supported", instanceExtensions.size());
    for (const std::string& instanceExtension : instanceExtensions) {
        LOG_TRACE(quartz::loggers::GENERAL, "  - {}", instanceExtension);
    }

    /////////////////////////////////////////////////////////////////
    // The loop
    /////////////////////////////////////////////////////////////////

    while(!glfwWindowShouldClose(p_window)) {
        glfwPollEvents();
    }

    /////////////////////////////////////////////////////////////////
    // Cleanup
    /////////////////////////////////////////////////////////////////

    LOG_TRACE(quartz::loggers::GENERAL, "Destroying window");
    glfwDestroyWindow(p_window);

    LOG_TRACE(quartz::loggers::GENERAL, "GLFW Terminating");
    glfwTerminate();

    LOG_TRACE(quartz::loggers::GENERAL, "Testing GLM");
    glm::mat4 matrix;
    glm::vec4 vector;
    glm::vec4 result = matrix * vector;
    LOG_TRACE(quartz::loggers::GENERAL, "Resulting vector [ {} {} {} {} ]", result.x, result.y, result.z, result.w);

    LOG_TRACE(quartz::loggers::GENERAL, "Terminating");
    return 0;
}