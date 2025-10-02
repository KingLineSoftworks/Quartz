#include <string>

#include "util/logger/Logger.hpp"
#include "util/unit_test/UnitTest.hpp"

#include "quartz/rendering/buffer/LocallyMappedBuffer.hpp"

std::string
catsOfSkyrim() {
    return
        "I have been sent to this frigid wasteland to catalogue and study any of its indigenous cats, which has so far been uneventful. After months of wandering I have so far only encountered some variations of the same basic species."
        "In my travels I have encountered several khajiit outcast from their clans that have taken up residence in Skyrim. They have been most unhelpful, probably for fear I’d expose their locations. I can’t say I’m surprised that there are few khajiit here, it’s cold and unwelcoming."
        "Sabrecats are basic giant cats that have evolved two dangerously sharp front teeth."
        "The average sabrecat has a reddish brown fur which it uses to blend into grassy regions, but I have observed them skulking and sleeping on rocks so I don’t believe the fur is for camouflage."
        "The primary attacks of the sabrecat are its biting attack, but it can also briefly rear up to attack with its front claws. I have also seen it pounce forward on its prey in a particularly powerful attack."
        "The snowy version of the sabrecat has spotted white fur which I believe it uses more for stalking more than its cousin in the plains."
        "The tooth of the cat is rumored to be useful in potions that restore the imbibers stamina as well as a potion that will temporarily give a more keen eye for smithing."
        "An skillful hunter can usually salvage the pelts and teeth of their kill, but report that the meat is tasteless and not fit to eat."
    ;
}


UT_FUNCTION(test_mapVulkanPhysicalDeviceMemoryToLocalMemory) {
    // Create physical device
    quartz::rendering::Instance renderingInstance("LMB_UT", 9, 9, 9, true);
    quartz::rendering::Device renderingDevice(renderingInstance);

    // Create the data which we are going to copy in
    const std::string sourceData = catsOfSkyrim();

    // Create locally mapped buffer
    quartz::rendering::LocallyMappedBuffer locallyMappedBuffer(
        renderingDevice,
        sourceData.size(),
        vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst,
        vk::MemoryPropertyFlagBits::eHostVisible
    );

    // Ensure that the locally mapped buffer contains the correct data (UBO info data)

    // The vk::UniqueBuffer is a unique handle around a VkBuffer.
    // A VkBuffer is used to "represent linear arrays of data which are used for various
    // purposes by binding them to a graphics or compute pipeline via descriptor sets or
    // certain commands, or by directly specifying them as parameters to certain commands."
    
    // We can use memcpy to get data into our buffer by utilizing the mapped local memory pointer
    // but we need to figure out how to get data out of the vk::UniqueBuffer without having to
    // unmap the buffer then remap it

    // Copy the data to the locally mapped buffer
    LOG_TRACE(UT, "Copying {} bytes into mapped buffer at 0x{}", sourceData.size(), locallyMappedBuffer.getMappedLocalMemoryPtr());
    memcpy(
        locallyMappedBuffer.getMappedLocalMemoryPtr(),
        sourceData.data(),
        sourceData.size()
    );

    LOG_TRACE(UT, "Comparing data at mapped memory location to source data");
    LOG_TRACE(UT, "  0x{}", locallyMappedBuffer.getMappedLocalMemoryPtr());
    uint32_t numMismatches = 0;
    for (uint32_t iByte = 0; iByte < sourceData.size(); ++iByte) {
        const char a = reinterpret_cast<char*>(locallyMappedBuffer.getMappedLocalMemoryPtr())[iByte];
        const char b = sourceData[iByte];
        if (a == b) {
            continue;
        }

        LOG_TRACE(UT, "Mismatch at index {} - {} != {}", iByte, a, b);
        ++numMismatches;
    }
    LOG_TRACE(UT, "Total mismatches: {}",  numMismatches);

    UT_CHECK_EQUAL(numMismatches, 0);
}

UT_MAIN() {
    REGISTER_UT_FUNCTION(test_mapVulkanPhysicalDeviceMemoryToLocalMemory);
    UT_RUN_TESTS();
}

