#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>

#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"

void
inspectStride(
    const uint32_t minAlignmentBytes,
    const uint32_t uboSizeBytes 
) {
#if true
    const uint32_t resultBytes = quartz::rendering::UniformBufferInfo::calculateDynamicUniformBufferByteStride(
        minAlignmentBytes,
        uboSizeBytes
    );
    std::cout << std::setw(3) << uboSizeBytes << std::setw(0) << " , ";
    std::cout << std::setw(3) << minAlignmentBytes << std::setw(0) << " -> ";
    std::cout << std::setw(4) << resultBytes << std::setw(0);
    std::cout << std::endl;
    return;
#else
    const uint32_t addedBytes = uboSizeBytes + minAlignmentBytes - 1;
    const uint32_t negatedMinAlignmentBytes = ~(minAlignmentBytes - 1);
    const uint32_t resultBytes = addedBytes & negatedMinAlignmentBytes;

    std::cout << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << "{" << std::endl;

    std::cout << "  UBO size bytes       : " << std::bitset<32>(uboSizeBytes) << " = " << uboSizeBytes << std::endl;
    std::cout << "  Min alignment        : " << std::bitset<32>(minAlignmentBytes) << " = " << minAlignmentBytes << std::endl;
    std::cout << std::endl;

    std::cout << "  added with offset    : " << std::bitset<32>(addedBytes) << " = " << addedBytes << std::endl;
    std::cout << std::endl;
    std::cout << "  negated min alignment: " << std::bitset<32>(negatedMinAlignmentBytes) << " = "  << negatedMinAlignmentBytes << std::endl;
    std::cout << std::endl;
    std::cout << "  result               : " << std::bitset<32>(resultBytes) << " = "  << resultBytes << std::endl;

    std::cout << "}" << std::endl;
    std::cout << "====================================================" << std::endl;
    std::cout << std::endl;
#endif
}

int
main() {
    const std::vector<uint32_t> alignments = {
        1, 2, 3, 4, 5, 7, 10, 11
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
