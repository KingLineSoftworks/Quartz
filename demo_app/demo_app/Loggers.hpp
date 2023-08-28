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

} // namespace loggers
} // namespace quartz
