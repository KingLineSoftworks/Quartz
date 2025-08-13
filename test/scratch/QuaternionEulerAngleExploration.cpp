#include "math/Loggers.hpp"
#include "math/transform/Quaternion.hpp"
#include "util/logger/Logger.hpp"

int main() {
    REGISTER_LOGGER_GROUP(MATH);
    util::Logger::setLevel("TRANSFORM", util::Logger::Level::trace);

    LOG_INFO(TRANSFORM, "Testing logger");

    const math::Quaternion quat = math::Quaternion::fromEulerAngles(0, 1, 0);

    LOG_INFO(TRANSFORM, "Quat: {}", quat.toString());
}
