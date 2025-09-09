#include <filesystem>
#include <fstream>
#include <fstream>

#include "util/platform.hpp"
#include "util/unit_test/UnitTest.hpp"
#include "util/file_system/FileSystem.hpp"

UT_FUNCTION(test_getAbsoluteFilepathInQuartzDirectory) {
    {
#ifdef QUARTZ_IS_TOP_LEVEL
        const std::string relativePath = "./../src/math/transform/Quaternion.hpp";
#else
        const std::string relativePath = "./../Quartz/src/math/transform/Quaternion.hpp";
#endif
        const std::string absolutePath = util::FileSystem::getAbsoluteFilepathInQuartzDirectory("src/math/transform/Quaternion.hpp");

        const std::vector<char> relativeBytes = util::FileSystem::readBytesFromFile(relativePath);
        const std::vector<char> absoluteBytes = util::FileSystem::readBytesFromFile(absolutePath);
        UT_CHECK_EQUAL_CONTAINERS(absoluteBytes, relativeBytes);
    }
}

UT_FUNCTION(test_getAbsoluteFilePathInProjectDirectory) {
    {
#ifdef QUARTZ_IS_TOP_LEVEL
        const std::string relativePath = "./../src/quartz/application/Application.hpp";
        const std::string absolutePath = util::FileSystem::getAbsoluteFilepathInProjectDirectory("src/quartz/application/Application.hpp");
#else
        const std::string relativePath = "./../Quartz/src/quartz/application/Application.hpp";
        const std::string absolutePath = util::FileSystem::getAbsoluteFilepathInProjectDirectory("Quartz/src/quartz/application/Application.hpp");
#endif

        const std::vector<char> relativeBytes = util::FileSystem::readBytesFromFile(relativePath);
        const std::vector<char> absoluteBytes = util::FileSystem::readBytesFromFile(absolutePath);
        UT_CHECK_EQUAL_CONTAINERS(absoluteBytes, relativeBytes);
    }
}

UT_FUNCTION(test_getAbsoluteFilePathInBinaryDirectory) {
    {
        // Here we are using CMakeCache.txt because we know it will have to exist if Quartz is compiled
        const std::string relativePath = "./CMakeCache.txt";
        const std::string binaryPath = util::FileSystem::getAbsoluteFilepathInBinaryDirectory("CMakeCache.txt");

        const std::vector<char> relativeBytes = util::FileSystem::readBytesFromFile(relativePath);
        const std::vector<char> binaryBytes = util::FileSystem::readBytesFromFile(binaryPath);
        UT_CHECK_EQUAL_CONTAINERS(binaryBytes, relativeBytes);
    }
}

UT_FUNCTION(test_getCompiledShaderAbsoluteFilepath) {
    {
        const std::string relativePath = "./spv/skybox.frag.spv";
        const std::string absolutePath = util::FileSystem::getCompiledShaderAbsoluteFilepath("skybox.frag");

        const std::vector<char> relativeBytes = util::FileSystem::readBytesFromFile(relativePath);
        const std::vector<char> absoluteBytes = util::FileSystem::readBytesFromFile(absolutePath);
        UT_CHECK_EQUAL_CONTAINERS(absoluteBytes, relativeBytes);
    } 
}

UT_FUNCTION(test_readBytesFromFile) {
    {
#ifdef ON_LINUX
        const std::string tempFilepath = std::filesystem::temp_directory_path().string() + "/" + std::string("tempfile.txt");
#else
        const std::string tempFilepath = std::filesystem::temp_directory_path().string() + std::string("tempfile.txt");
#endif

        const std::string inputString = " Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
        const std::vector<char> inputData(inputString.begin(), inputString.end());

        std::ofstream tempFile(tempFilepath);
        tempFile << inputString;
        tempFile.close();

        const std::vector<char> outputData = util::FileSystem::readBytesFromFile(tempFilepath);

        UT_CHECK_EQUAL_CONTAINERS(outputData, inputData);
    } 
}

UT_FUNCTION(test_getFileName) {
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("something.txt"), "something.txt");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("./something.txt"), "something.txt");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("/from/the/beginning/aye"), "aye");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("booger"), "booger");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension(".booger"), ".booger");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("directory/"), "directory");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("here/is/another/directory/"), "directory");
}

UT_FUNCTION(test_getFileExtension) {
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("something.txt"), "txt");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("/hello/world/another.hpp"), "hpp");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("/hello/world/another.one.c"), "c");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension("./this/one/is/empty"), "");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension(".hidden"), "");
    UT_CHECK_EQUAL(util::FileSystem::getFileExtension(".hidden.sh"), "sh");
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_getAbsoluteFilePathInProjectDirectory);
    REGISTER_UT_FUNCTION(test_getAbsoluteFilePathInBinaryDirectory);
    REGISTER_UT_FUNCTION(test_getCompiledShaderAbsoluteFilepath);
    REGISTER_UT_FUNCTION(test_readBytesFromFile);
    REGISTER_UT_FUNCTION(test_getFileExtension);
    UT_RUN_TESTS();
}
