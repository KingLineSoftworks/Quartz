#include <cmath>
#include <optional>

#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"

uint32_t manualStride(
    const uint32_t minAlignment,
    const uint32_t uboSizeBytes
) {
    if (minAlignment == 0) {
        return uboSizeBytes;
    }

    // Take our uboSize and round up to the next multiple of our alignment size

    const double fitment = static_cast<double>(uboSizeBytes) / static_cast<double>(minAlignment);
    const uint32_t alignmentMultiples = std::ceil(fitment);
    return alignmentMultiples * minAlignment;
}

/**
 * @brief From the vulkan spec:
 *    """
 *    `minUniformBufferOffsetAlignment` is the minimum __required__ alignment, in bytes, for the
 *    `offset` member of the `VkDescriptorBufferInfo` structure for uniform buffers.
 *    When a descriptor of type `VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER` or `VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC`
 *    is updated, the `offset` __must__ be an integer multiple of this limit.
 *    Similarly, dynamic offsets for uniform buffers __must__ be multiples of this limit.
 *    The value __must__ be a power of two.
 *    """
 */
UT_FUNCTION(test_calculateDynamicUniformBufferByteStride) {
    struct TestInfo {
        uint32_t minAlignment;
        uint32_t uboSizeBytes;
    };

    // Create the list of testing information
    // We do this by creating a list of alignments (each of which is a power of 2),
    //    then creating a set of uboSizes to test each alignment
    std::vector<TestInfo> testInfos;
    std::vector<uint32_t> alignments;
    const uint32_t maxPower = 8;
    for (uint32_t power = 0; power <= maxPower; ++power) {
        alignments.push_back(std::pow(2, power));
    }
    const double iterations = 10;
    for (const uint32_t alignment : alignments) {
        for (uint32_t uboSize = 1; uboSize < (alignment * iterations + 1); uboSize++) {
            testInfos.push_back({alignment, uboSize});
        }
    }

    for (const TestInfo& testInfo : testInfos) {
        UT_CHECK_EQUAL(
            quartz::rendering::UniformBufferInfo::calculateDynamicUniformBufferByteStride(
                testInfo.minAlignment,
                testInfo.uboSizeBytes
            ),
            manualStride(
                testInfo.minAlignment,
                testInfo.uboSizeBytes
            )
        );
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_calculateDynamicUniformBufferByteStride);
    UT_RUN_TESTS();
}

