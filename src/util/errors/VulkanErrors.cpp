#include "util/errors/VulkanErrors.hpp"

util::VulkanFeatureNotSupportedError::VulkanFeatureNotSupportedError(const std::string& message) :
    std::runtime_error(message)
{}

util::VulkanCreationFailedError::VulkanCreationFailedError(const std::string& message) :
    std::runtime_error(message)
{}