#include <iostream>

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/core.hpp"

#include "demo_app/core.hpp"

#define GENERAL_LOGGER_NAME "GENERAL"
const std::string LOGGER_NAME = GENERAL_LOGGER_NAME;

int main() {
    ASSERT_QUARTZ_VERSION();
    ASSERT_APPLICATION_VERSION();

#ifdef QUARTZ_DEBUG
    std::cout << "quartz debug defined" << std::endl;
#endif
#ifdef QUARTZ_TEST
    std::cout << "quartz test defined" << std::endl;
#endif
#ifdef QUARTZ_RELEASE
    std::cout << "quartz release defined" << std::endl;
#endif

    std::cout << "quartz version " << QUARTZ_MAJOR_VERSION << "." << QUARTZ_MINOR_VERSION << "." << QUARTZ_PATCH_VERSION << "\n";
    std::cout << "app version " << APPLICATION_MAJOR_VERSION << "." << APPLICATION_MINOR_VERSION << "." << APPLICATION_PATCH_VERSION << "\n";

    quartz::util::Logger::registerLoggers({
        {GENERAL_LOGGER_NAME, quartz::util::Logger::Level::trace}
    });

    LOG_TRACE("Attempting log - LOG_TRACE");
    LOG_DEBUG("Attempting log - LOG_DEBUG");
    LOG_INFO("Attempting log - LOG_INFO");
    LOG_WARNING("Attempting log - LOG_WARNING");
    LOG_ERROR("Attempting log - LOG_ERROR");
    LOG_CRITICAL("Attempting log - LOG_CRITICAL");

    return 0;
}