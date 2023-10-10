#include <stdexcept>
#include <cstdlib>

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/core.hpp"
#include "quartz/application/Application.hpp"
#include "quartz/application2/Application2.hpp"

#include "demo_app/core.hpp"
#include "demo_app/Loggers.hpp"

int main() {
    constexpr bool shouldLogPreamble = false;

    ASSERT_QUARTZ_VERSION();
    ASSERT_APPLICATION_VERSION();

    quartz::util::Logger::setShouldLogPreamble(shouldLogPreamble);

    REGISTER_LOGGER_GROUP(UTIL);
    REGISTER_LOGGER_GROUP(QUARTZ);
    REGISTER_LOGGER_GROUP(QUARTZ_RENDERING);
    REGISTER_LOGGER_GROUP(DEMO_APP);

    quartz::util::Logger::setLevels({
        {"GENERAL", quartz::util::Logger::Level::trace},
        {"FILESYSTEM", quartz::util::Logger::Level::trace},
        {"APPLICATION", quartz::util::Logger::Level::trace},
        {"APPLICATION_INITIALIZATION", quartz::util::Logger::Level::trace},
        {"WINDOW", quartz::util::Logger::Level::trace},
        {"VULKAN", quartz::util::Logger::Level::info}
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

#ifdef QUARTZ_RELEASE
    const bool validationLayersEnabled = false;
#else
    const bool validationLayersEnabled = true;
#endif

    if (!glfwInit()) {
        LOG_CRITICAL(quartz::loggers::GENERAL, "Failed to initialize GLFW");
        return EXIT_FAILURE;
    }
    LOG_INFO(quartz::loggers::GENERAL, "GLFW initialized");

//    quartz::Application application(
//        APPLICATION_NAME,
//        APPLICATION_MAJOR_VERSION,
//        APPLICATION_MINOR_VERSION,
//        APPLICATION_PATCH_VERSION,
//        800,
//        600,
//        validationLayersEnabled
//    );
//
//    try {
//        application.run();
//    } catch (const std::exception& e) {
//        LOG_CRITICAL(quartz::loggers::GENERAL, "{}", e.what());
//        return EXIT_FAILURE;
//    }
//
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");
//    LOG_TRACE(quartz::loggers::GENERAL, "");

    quartz::Application2 application2(
        APPLICATION_NAME,
        APPLICATION_MAJOR_VERSION,
        APPLICATION_MINOR_VERSION,
        APPLICATION_PATCH_VERSION,
        800,
        600,
        validationLayersEnabled
    );

    try {
        application2.run();
    } catch (const std::exception& e) {
        LOG_CRITICAL(quartz::loggers::GENERAL, "{}", e.what());
        return EXIT_FAILURE;
    }

    LOG_TRACE(quartz::loggers::GENERAL, "Terminating");
    return EXIT_SUCCESS;
}