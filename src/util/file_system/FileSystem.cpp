#include <algorithm>
#include <cstddef>
#include <fstream>
#include <stdexcept>
#include <string>

#include "util/platform.hpp"
#include "util/Loggers.hpp"
#include "util/logger/Logger.hpp"

#include "util/file_system/FileSystem.hpp"

std::string
util::FileSystem::getAbsoluteFilepathInQuartzDirectory(const std::string& filepathInQuartzDirectory) {
    return std::string(QUARTZ_ROOT_DIR) + std::string("/") + filepathInQuartzDirectory;
}

std::string
util::FileSystem::getAbsoluteFilepathInProjectDirectory(const std::string& filepathInProjectDirectory) {
    return std::string(PROJECT_ROOT_DIR) + std::string("/") + filepathInProjectDirectory;
}

std::string
util::FileSystem::getAbsoluteFilepathInBinaryDirectory(const std::string& filepathInBinaryDirectory) {
    return std::string(PROJECT_BINARY_DIR) + std::string("/") + filepathInBinaryDirectory;
}

std::string
util::FileSystem::getCompiledShaderAbsoluteFilepath(const std::string& shaderSourceFilename) {
    return std::string(SHADER_BINARY_DIR) + std::string("/") + shaderSourceFilename + std::string(".spv");
}

std::vector<char>
util::FileSystem::readBytesFromFile(const std::string& filepath) {
    LOG_FUNCTION_SCOPE_TRACE(FILESYSTEM, "{}", filepath);
    std::ifstream infile(filepath, std::ios::ate | std::ios::binary);

    if (!infile.is_open()) {
        LOG_THROW(FILESYSTEM, std::runtime_error, "Failed to open {} for binary reading", filepath);
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

std::string
util::FileSystem::getFileName(const std::string& filepath) {
    const size_t slashPosition = filepath.find_last_of('/');

    if (slashPosition == std::string::npos) {
        return filepath;
    }

    if (slashPosition != filepath.size()) {
        return filepath.substr(slashPosition + 1);
    }

    const std::string subFilepath = filepath.substr(0, filepath.size() - 1);
    const size_t secondSlashPosition = std::max<size_t>(0, subFilepath.rfind('/'));
    return subFilepath.substr(secondSlashPosition);
}

std::string
util::FileSystem::getFileExtension(const std::string& filepath) {
    const std::string filename = util::FileSystem::getFileName(filepath);
    const size_t dotPosition = filename.find_last_of('.');

    if (dotPosition == std::string::npos || dotPosition == 0) {
        return "";
    }

    return filename.substr(dotPosition + 1);
}
