#include "util/unit_test/UnitTest.hpp"

#include "quartz/rendering/buffer/LocallyMappedBuffer.hpp"

UT_FUNCTION(test_mapVulkanPhysicalDeviceMemoryToLocalMemory) {
    // Create physical device
    quartz::rendering::Instance renderingInstance("DOODAD_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    // Create uniform buffer info
    // Create locally mapped buffer
    // Ensure that the locally mapped buffer contains the correct data (UBO info data)
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_mapVulkanPhysicalDeviceMemoryToLocalMemory);
    UT_RUN_TESTS();
}

