#include "util/logger/Logger.hpp"

#include "math/Loggers.hpp"
#include "math/transform/Quaternion.hpp"

int main() {
    REGISTER_LOGGER_GROUP(MATH);
    util::Logger::setLevel("TRANSFORM", util::Logger::Level::trace);

    for (uint32_t caseIndex = 0; caseIndex < 3; ++caseIndex) {
        const std::string caseString = (caseIndex == 0) ? "yaw" : (caseIndex == 1) ? "pitch" : "roll";

        LOG_INFO(TRANSFORM, "Displaying {}", caseString);

        for (uint32_t i = 0; i < 12; ++i) {
            const double degrees = i * 30;

            math::Quaternion quat;
            if (caseIndex == 0) {
                quat = math::Quaternion::fromEulerAngles(degrees, 0, 0);
            } else if (caseIndex == 1) {
                quat = math::Quaternion::fromEulerAngles(0, degrees, 0);
            } else if (caseIndex == 2) {
                quat = math::Quaternion::fromEulerAngles(0, 0, degrees);
            }

            LOG_INFO(TRANSFORM, "  {} degrees {} direction = {}", degrees, caseString, quat.getDirectionVector().toString());
        }

        if (caseIndex < 2) {
            LOG_INFO(TRANSFORM, "");
        }
    }
}
