#pragma once

#include <vector>
#include <string>

namespace util {
    class FileSystem;
}

class util::FileSystem {
public:
    static std::string getAbsoluteFilepathInProjectDirectory(const std::string& filepathInProjectDirectory);
    static std::string getAbsoluteFilepathInBinaryDirectory(const std::string& filepathInBinaryDirectory);
    static std::string getCompiledShaderAbsoluteFilepath(const std::string& shaderSourceFilename);
    static std::vector<char> readBytesFromFile(const std::string& filepath);
    static std::string getFileExtension(const std::string& filepath);

public:
    FileSystem() = delete;
};
