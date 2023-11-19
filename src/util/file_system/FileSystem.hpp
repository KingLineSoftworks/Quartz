#pragma once

#include <vector>
#include <string>

namespace util {
    class FileSystem;
}

class util::FileSystem {
public:
    static std::string getAbsoluteFilepathInProject(const std::string& filepathInProject);
    static std::vector<char> readBytesFromFile(const std::string& filepath);
    static std::string getFileExtension(const std::string& filepath);

public:
    FileSystem() = delete;
};
