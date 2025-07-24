#pragma once

#include <vector>
#include <string>

namespace util {
    class FileSystem;
}

class util::FileSystem {
public:
    /**
     * @todo 2025/07/23 We need to make Quartz specific variants of this which will still function as expected
     *    when quartz is a submodule and not the root level project
     *
     * @todo 2025/07/23 We need to make Quartz aware of the build directory from which it is operating within.
     *    Perhaps we can do this from within CMake by passing the current directory as the build directory
     *
     * If CMake gives us the build directory, and we know that we are a submodule, then we can get our way into
     * Quartz's root directory from wherever we may be
     */

    static std::string getAbsoluteFilepathInProjectDirectory(const std::string& filepathInProjectDirectory);
    static std::string getAbsoluteFilepathInBinaryDirectory(const std::string& filepathInBinaryDirectory);
    static std::string getCompiledShaderAbsoluteFilepath(const std::string& shaderSourceFilename);
    static std::vector<char> readBytesFromFile(const std::string& filepath);
    static std::string getFileName(const std::string& filepath);
    static std::string getFileExtension(const std::string& filepath);

public:
    FileSystem() = delete;
};
