#include <stdexcept>
#include <string>

namespace util {
    class VulkanFeatureNotSupportedError;
    class VulkanCreationFailedError;
}

class util::VulkanFeatureNotSupportedError : public std::runtime_error {
public:
    VulkanFeatureNotSupportedError(const std::string& message);
};

class util::VulkanCreationFailedError : public std::runtime_error {
public:
    VulkanCreationFailedError(const std::string& message);
};