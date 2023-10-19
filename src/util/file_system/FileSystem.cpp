#include <fstream>

#include "util/platform.hpp"
#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"

#include "util/file_system/FileSystem.hpp"

std::string util::FileSystem::getAbsoluteFilepathInProject(const std::string& filepathInProject) {
    return std::string(PROJECT_ROOT_DIR) + std::string("/") + filepathInProject;
}

std::vector<char> util::FileSystem::readBytesFromFile(const std::string& filepath) {
    LOG_FUNCTION_SCOPE_TRACE(FILESYSTEM, "{}", filepath);
    std::ifstream infile(filepath, std::ios::ate | std::ios::binary);

    if (!infile.is_open()) {
        LOG_CRITICAL(FILESYSTEM, "Failed to open {} for binary reading", filepath);
        throw std::runtime_error("");
    }

    uint32_t fileSizeBytes = infile.tellg(); // because we started at the end with std::ios::ate
    std::vector<char> bytes(fileSizeBytes);
    LOG_TRACE(FILESYSTEM, "File {} contains {} bytes", filepath, fileSizeBytes);

    infile.seekg(0); // go back to the beginning
    infile.read(bytes.data(), fileSizeBytes);

    infile.close();

    LOG_TRACE(FILESYSTEM, "Successfully read {} bytes", fileSizeBytes);

    return bytes;
}