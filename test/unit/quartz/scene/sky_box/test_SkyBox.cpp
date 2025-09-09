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

    LOG_TRACE(UT, "Creating instance");
    quartz::rendering::Instance renderingInstance(
        "SKYBOX_UT",
        9,
        9,
        9,
        true
    );
    LOG_TRACE(UT, "Creating device");
    quartz::rendering::Device renderingDevice(renderingInstance);

    LOG_TRACE(UT, "Creating skybox");
    quartz::scene::SkyBox skyBox(
        renderingDevice,
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posx-00FFFF-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negx-FF0000-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posy-FF00FF-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negy-00FF00-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/posz-FFFF00-2x2.jpg"),
        util::FileSystem::getAbsoluteFilepathInQuartzDirectory("assets/sky_boxes/test/negz-0000FF-2x2.jpg")
    );

    LOG_TRACE(UT, "Getting cube map from sky box");
    const quartz::rendering::CubeMap& cubeMap = skyBox.getCubeMap();
    LOG_TRACE(UT, "Getting staged image buffer from cube map");
    const quartz::rendering::StagedImageBuffer& stagedImageBuffer = cubeMap.getStagedImageBuffer();
    LOG_TRACE(UT, "Getting staging memory from staged image buffer");
    const vk::UniqueDeviceMemory& p_vulkanDeviceStagingMemory = stagedImageBuffer.getVulkanPhysicalDeviceStagingMemory();
 
    /**
     * @todo 2025/08/05 Make utility within BufferUtil to read memory from a device
     */
    LOG_TRACE(UT, "Mapping staging memory onto the device");
    void* p_mappedStagingMemory = renderingDevice.getVulkanLogicalDevicePtr()->mapMemory(
        *p_vulkanDeviceStagingMemory,
        0,
        totalSize
    );
    LOG_TRACE(UT, "Mapped staged memory to local memory at: 0x{}", p_mappedStagingMemory);
    UT_REQUIRE(p_mappedStagingMemory);

    LOG_TRACE(UT, "Allocating memory to copy into a local variable");
    std::array<uint8_t, totalSize> actualImageBytes;
    LOG_TRACE(UT, "Copying into our local data from our mapped staged memory");
    memcpy(actualImageBytes.data(), p_mappedStagingMemory, totalSize);
    LOG_TRACE(UT, "Unmapping staging memory");
    renderingDevice.getVulkanLogicalDevicePtr()->unmapMemory(*p_vulkanDeviceStagingMemory);

    LOG_TRACE(UT, "Iterating over pixel data for each of the images");
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

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_construction_same_size);
    UT_RUN_TESTS();
}
