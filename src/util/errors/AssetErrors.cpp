#include "util/errors/AssetErrors.hpp"

util::AssetLoadFailedError::AssetLoadFailedError(const std::string& message) :
    std::runtime_error(message)
{}

util::AssetInsufficientError::AssetInsufficientError(const std::string& message) :
    std::runtime_error(message)
{}