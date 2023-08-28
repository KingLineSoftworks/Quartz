#pragma once

#include <array>

#include "util/logger/Logger.hpp"

namespace quartz {
namespace loggers {

constexpr quartz::util::Logger::RegistrationInfo GENERAL = {"GENERAL", quartz::util::Logger::Level::trace};
constexpr quartz::util::Logger::RegistrationInfo BIGBOY = {"BIGBOY", quartz::util::Logger::Level::info};
constexpr quartz::util::Logger::RegistrationInfo ALAMANCY = {"ALAMANCY", quartz::util::Logger::Level::critical};
constexpr quartz::util::Logger::RegistrationInfo GENERAL2 = {"GENERAL2", quartz::util::Logger::Level::error};

constexpr std::array<const quartz::util::Logger::RegistrationInfo, 4> DEMO_APP_LOGGER_INFOS = {
    GENERAL,
    BIGBOY,
    ALAMANCY,
    GENERAL2
};

#define REGISTER_DEMO_APP_LOGGERS() quartz::util::Logger::registerLoggers(quartz::loggers::DEMO_APP_LOGGER_INFOS)

/**
 * @brief Macro requirements
 * 
 * Inputs:
 * - group = DEMO_APP
 * - reg info = GENERAL, trace
 * - reg info = BIGBOY, info
 * - reg info = ALLAMANCY, critical
 * - reg info = GENERAL2, error
 * - ...
 * 
 * Outputs:
 * - constexpr RegistrationInfo for each of the reg info given as input
 * - constexpr std::array with the group name containing each of the reg info given as input
 * - macro registering the constexpr std::array
 */

} // namespace loggers
} // namespace quartz
