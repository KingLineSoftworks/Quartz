#include <iostream>

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/core.hpp"
#include "quartz/something/Something.hpp"

#include "demo_app/core.hpp"
#include "demo_app/Loggers.hpp"

int main() {
    ASSERT_QUARTZ_VERSION();
    ASSERT_APPLICATION_VERSION();

    /**
     * @todo This should be done automatically by declaring the loggers in Loggers.hpp files.
     * The default logging level should automatically be set upon declaration of the loggers so we don't
     * have to risk accidentally setting the default level incorrectly with the levels we wish to pass to
     * setLevels
     * @todo When declaring the loggers via macro, we should define a macro that gives us REGISTER_QUARTZ_LOGGERS()
     * and REGISTER_DEMO_APP_LOGGERS() so we don't have to risk this.
     */
    // REGISTER_QUARTZ_LOGGERS();
    // REGISTER_DEMO_APP_LOGGERS();

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

    LOG_TRACE(quartz::loggers::GENERAL, "Attempting log - LOG_TRACE");
    LOG_DEBUG(quartz::loggers::GENERAL, "Attempting log - LOG_DEBUG");
    LOG_INFO(quartz::loggers::GENERAL, "Attempting log - LOG_INFO");
    LOG_WARNING(quartz::loggers::GENERAL, "Attempting log - LOG_WARNING");
    LOG_ERROR(quartz::loggers::GENERAL, "Attempting log - LOG_ERROR");
    LOG_CRITICAL(quartz::loggers::GENERAL, "Attempting log - LOG_CRITICAL");

    LOG_TRACE(quartz::loggers::BIGBOY, "Attempting log - LOG_TRACE");
    LOG_DEBUG(quartz::loggers::BIGBOY, "Attempting log - LOG_DEBUG");
    LOG_INFO(quartz::loggers::BIGBOY, "Attempting log - LOG_INFO");
    LOG_WARNING(quartz::loggers::BIGBOY, "Attempting log - LOG_WARNING");
    LOG_ERROR(quartz::loggers::BIGBOY, "Attempting log - LOG_ERROR");
    LOG_CRITICAL(quartz::loggers::BIGBOY, "Attempting log - LOG_CRITICAL");

    LOG_TRACE(quartz::loggers::ALAMANCY, "Attempting log - LOG_TRACE");
    LOG_DEBUG(quartz::loggers::ALAMANCY, "Attempting log - LOG_DEBUG");
    LOG_INFO(quartz::loggers::ALAMANCY, "Attempting log - LOG_INFO");
    LOG_WARNING(quartz::loggers::ALAMANCY, "Attempting log - LOG_WARNING");
    LOG_ERROR(quartz::loggers::ALAMANCY, "Attempting log - LOG_ERROR");
    LOG_CRITICAL(quartz::loggers::ALAMANCY, "Attempting log - LOG_CRITICAL");

    quartz::Something something(69, 42.666);
    something.doSomething();

    return 0;
}