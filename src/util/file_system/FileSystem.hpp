#pragma once

#include <vector>
#include <string>

namespace quartz {
namespace util {
    class FileSystem;
}
}

class quartz::util::FileSystem {
public:
    static std::string getAbsoluteFilepathInProject(const std::string& filepathInProject);
    static std::vector<char> readBytesFromFile(const std::string& filepath);

public:
    FileSystem() = delete;
};
