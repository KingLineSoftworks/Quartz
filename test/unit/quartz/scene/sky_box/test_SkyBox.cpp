#include <cstring>

#include "util/file_system/FileSystem.hpp"
#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "quartz/rendering/buffer/StagedImageBuffer.hpp"
#include "quartz/rendering/cube_map/CubeMap.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/scene/sky_box/SkyBox.hpp"

UT_FUNCTION(test_construction_same_size) {
    const uint32_t imageRows = 2;
    const uint32_t imageCols = 2;
    const uint32_t bytesPerPixel = 4;
    const uint32_t totalImageDataSize = imageRows * imageCols * bytesPerPixel;
    const uint32_t numImages = 6;
    const uint32_t totalSize = numImages * totalImageDataSize;

    quartz::rendering::Instance renderingInstance(
        "SKYBOX_UT",
        9,
        9,
        9,
        true
    );
    quartz::rendering::Device renderingDevice(renderingInstance);

    quartz::scene::SkyBox skyBox(
        renderingDevice,
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posx-00FFFF-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negx-FF0000-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posy-FF00FF-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negy-00FF00-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posz-FFFF00-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negz-0000FF-2x2.jpg")
    );

    const quartz::rendering::CubeMap& cubeMap = skyBox.getCubeMap();
    const quartz::rendering::StagedImageBuffer& stagedImageBuffer = cubeMap.getStagedImageBuffer();
    const vk::UniqueDeviceMemory& p_vulkanDeviceStagingMemory = stagedImageBuffer.getVulkanPhysicalDeviceStagingMemory();
   
    /**
     * @todo 2025/08/05 Make utility within BufferUtil to read memory from a device
     */
    void* p_mappedStagingMemory = renderingDevice.getVulkanLogicalDevicePtr()->mapMemory(
        *p_vulkanDeviceStagingMemory,
        0,
        totalSize
    );
    UT_REQUIRE(p_mappedStagingMemory);
    renderingDevice.getVulkanLogicalDevicePtr()->unmapMemory(*p_vulkanDeviceStagingMemory);

    std::array<uint8_t, totalSize> actualImageBytes;
    memcpy(actualImageBytes.data(), p_mappedStagingMemory, totalSize);
    for (uint32_t imageIndex = 0; imageIndex < 6; ++imageIndex) {
        LOG_INFO(UT, "Displaying pixel data for image {}", imageIndex);
        UNUSED const uint32_t startIndex = totalImageDataSize * imageIndex;
        for (uint32_t byteIndex = 0; byteIndex < totalImageDataSize; byteIndex += 4) {
            LOG_INFO(
                UT,
                "  0x{:X} 0x{:X} 0x{:X} 0x{:X}",
                actualImageBytes[startIndex+byteIndex+0],
                actualImageBytes[startIndex+byteIndex+1],
                actualImageBytes[startIndex+byteIndex+2],
                actualImageBytes[startIndex+byteIndex+3]
            );
        }
    }

    // Some of these values are FE instead of FF or 01 instead of 00 due to
    // gimp not creating images correctly ...
    const std::array<uint8_t, totalSize> expectedImageBytes = {
        // Pos x - 00 FF FF
        0x00, 0xFF, 0xFF, 0xFF,     0x00, 0xFF, 0xFF, 0xFF,
        0x00, 0xFF, 0xFF, 0xFF,     0x00, 0xFF, 0xFF, 0xFF,
        // Neg x - FF 00 00
        0xFE, 0x00, 0x00, 0xFF,     0xFE, 0x00, 0x00, 0xFF,
        0xFE, 0x00, 0x00, 0xFF,     0xFE, 0x00, 0x00, 0xFF,

        // Pos y - FF 00 FF
        0xFF, 0x00, 0xFE, 0xFF,     0xFF, 0x00, 0xFE, 0xFF,
        0xFF, 0x00, 0xFE, 0xFF,     0xFF, 0x00, 0xFE, 0xFF,
        // Neg y - 00 FF 00
        0x00, 0xFF, 0x01, 0xFF,     0x00, 0xFF, 0x01, 0xFF,
        0x00, 0xFF, 0x01, 0xFF,     0x00, 0xFF, 0x01, 0xFF,

        // Pos z - FF FF 00
        0xFF, 0xFF, 0x00, 0xFF,     0xFF, 0xFF, 0x00, 0xFF,
        0xFF, 0xFF, 0x00, 0xFF,     0xFF, 0xFF, 0x00, 0xFF,
        // Neg z - 00 00 FF
        0x00, 0x00, 0xFE, 0xFF,     0x00, 0x00, 0xFE, 0xFF,
        0x00, 0x00, 0xFE, 0xFF,     0x00, 0x00, 0xFE, 0xFF
    };

    UT_CHECK_EQUAL_CONTAINERS(actualImageBytes, expectedImageBytes);
}

UT_FUNCTION(test_construction_differing_size) {
    // std::array<std::string, 6> skyBoxInformation = {
    //     util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posx-00FFFF-2x2.jpg"),
    //     util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negx-FF0000-2x2.jpg"),
    //     util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posy-FF00FF-2x2.jpg"),
    //     util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negy-00FF00-2x2.jpg"),
    //     util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posz-FFFF00-1x1.jpg"),
    //     util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negz-0000FF-2x2.jpg")
    // };
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction_same_size);
    REGISTER_UT_FUNCTION(test_construction_differing_size);
    UT_RUN_TESTS();
}
