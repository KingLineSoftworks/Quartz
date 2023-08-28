#pragma once

#include <array>

#include "util/logger/Logger.hpp"

namespace quartz {
namespace loggers {

constexpr quartz::util::Logger::RegistrationInfo SOMETHING = {"SOMETHING", quartz::util::Logger::Level::trace};
constexpr quartz::util::Logger::RegistrationInfo ANOTHER = {"ANOTHER", quartz::util::Logger::Level::info};

constexpr std::array<const quartz::util::Logger::RegistrationInfo, 2> QUARTZ_LOGGER_INFOS = {
    SOMETHING,
    ANOTHER
};

} // namespace loggers
} // namespace quartz
