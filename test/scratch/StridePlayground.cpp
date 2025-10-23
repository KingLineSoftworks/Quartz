#include <iomanip>
#include <iostream>
#include <vector>

#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"

void
inspectStride(
    const uint32_t minAlignmentBytes,
    const uint32_t uboSizeBytes 
) {
    const uint32_t resultBytes = quartz::rendering::UniformBufferInfo::calculateDynamicUniformBufferByteStride(
        minAlignmentBytes,
        uboSizeBytes
    );
    std::cout << std::setw(3) << uboSizeBytes << std::setw(0) << " -> ";
    std::cout << std::setw(4) << resultBytes << std::setw(0);
    std::cout << std::endl;
    return;
}

int
main() {
    // must be a multiple of 2 according to vulkan spec
    const std::vector<uint32_t> alignments = {
        1, 2, 4, 8, 16, 32
    };

    const double iterations = 2.5;
    for (const uint32_t alignment : alignments) {
        std::cout << std::endl;
        std::cout << "Alignment: " << alignment << std::endl;
        for (uint32_t uboSize = 1; uboSize < (alignment * iterations + 1); ++uboSize) {
            inspectStride(alignment, uboSize);
        }
    }

    return 0;
}
