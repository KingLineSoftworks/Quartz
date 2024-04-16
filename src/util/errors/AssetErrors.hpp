#include <stdexcept>
#include <string>

namespace util {
    class AssetLoadFailedError;
    class AssetInsufficientError;
}

class util::AssetLoadFailedError : public std::runtime_error {
public:
    AssetLoadFailedError(const std::string& message);
};

class util::AssetInsufficientError : public std::runtime_error {
public:
    AssetInsufficientError(const std::string& message);
};