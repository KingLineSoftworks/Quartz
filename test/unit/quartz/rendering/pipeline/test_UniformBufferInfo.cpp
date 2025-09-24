#include <optional>

#include "util/unit_test/UnitTest.hpp"

#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"

uint32_t manualStride(
    const uint32_t minAlignment,
    const uint32_t uboSizeBytes
) {
    if (minAlignment == 0) {
        return uboSizeBytes;
    }

    // TODO - what the fuck is our stride function actually doing? I don't get it ...
    return minAlignment + uboSizeBytes;
}

UT_FUNCTION(test_calculateDynamicUniformBufferByteStride) {
    struct TestInfo {
        uint32_t minAlignment;
        uint32_t uboSizeBytes;
    };

    std::vector<TestInfo> testInfos;
    const std::vector<uint32_t> alignments = { 0, 1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 13, 23, 37, 50, 199, 55555, 6666 };
    const double iterations = 10;
    for (const uint32_t alignment : alignments) {
        for (uint32_t uboSize = 1; uboSize < (alignment * iterations + 1); uboSize++) {
            testInfos.push_back({alignment, uboSize});
        }
    }

    for (const TestInfo& testInfo : testInfos) {
        UT_CHECK_NOT_EQUAL(
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

