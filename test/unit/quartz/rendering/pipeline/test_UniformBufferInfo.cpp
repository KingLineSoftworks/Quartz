#include <cmath>
#include <optional>

#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "quartz/rendering/vulkan_util/VulkanUtil.hpp"
#include "quartz/rendering/pipeline/UniformBufferInfo.hpp"

#include <vulkan/vulkan_enums.hpp> // so linting stops complaining - and to prevent this from being auto included at the beginning

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

UT_FUNCTION(test_construction) {
    struct UBOInfoParameters {
        uint32_t locallyMappedBufferSizeBytes;
        vk::MemoryPropertyFlags locallyMappedBufferPropertyFlags;
        uint32_t bindingLocation;
        uint32_t descriptorCount;
        uint32_t objectStrideBytes;
        bool isDynamic;
        vk::ShaderStageFlags shaderStageFlags;
    };

    std::vector<UBOInfoParameters> allParameters = {
        UBOInfoParameters(
            99,
            vk::MemoryPropertyFlagBits::eHostCoherent,
            42,
            102,
            88,
            true,
            vk::ShaderStageFlagBits::eMeshNV
        ),

        UBOInfoParameters(
            233,
            vk::MemoryPropertyFlagBits::eDeviceCoherentAMD | vk::MemoryPropertyFlagBits::eHostVisible,
            4,
            12,
            8,
            false,
            vk::ShaderStageFlagBits::eClosestHitNV | vk::ShaderStageFlagBits::eAnyHitKHR
        )
    };

    for (uint32_t iParameter = 0; iParameter < allParameters.size(); ++iParameter) {
        LOG_SCOPE_CHANGE_TRACE(UT);
        LOG_TRACE(UT, "UBO Info {}", iParameter);

        const UBOInfoParameters& parameters = allParameters[iParameter];

        const quartz::rendering::UniformBufferInfo uboInfo(
            parameters.locallyMappedBufferSizeBytes,
            parameters.locallyMappedBufferPropertyFlags,
            parameters.bindingLocation,
            parameters.descriptorCount,
            parameters.objectStrideBytes,
            parameters.isDynamic,
            parameters.shaderStageFlags
        );

        UT_CHECK_EQUAL(uboInfo.getLocallyMappedBufferSize(), parameters.locallyMappedBufferSizeBytes);
        UT_CHECK_EQUAL(uboInfo.getLocallyMappedBufferVulkanUsageFlags(), vk::BufferUsageFlagBits::eUniformBuffer);
        UT_CHECK_EQUAL(uboInfo.getLocallyMappedBufferVulkanPropertyFlags(), parameters.locallyMappedBufferPropertyFlags);

        UT_CHECK_EQUAL(uboInfo.getBindingLocation(), parameters.bindingLocation);
        UT_CHECK_EQUAL(uboInfo.getDescriptorCount(), parameters.descriptorCount);
        UT_CHECK_EQUAL(uboInfo.getObjectStrideBytes(), parameters.objectStrideBytes);

        if (parameters.isDynamic) {
            UT_CHECK_EQUAL(uboInfo.getVulkanDescriptorType(), vk::DescriptorType::eUniformBufferDynamic);
        } else {
            UT_CHECK_EQUAL(uboInfo.getVulkanDescriptorType(), vk::DescriptorType::eUniformBuffer);
        }

        UT_CHECK_EQUAL(uboInfo.getVulkanShaderStageFlags(), parameters.shaderStageFlags);
    }
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_calculateDynamicUniformBufferByteStride);
    REGISTER_UT_FUNCTION(test_construction);
    UT_RUN_TESTS();
}

