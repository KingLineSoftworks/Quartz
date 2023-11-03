#include <set>

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"

vk::PhysicalDevice
quartz::rendering::Device::getBestPhysicalDevice(
    const vk::UniqueInstance& p_instance
) {
    LOG_FUNCTION_SCOPE_TRACE(DEVICE, "");

    // ----- get list of physical devices available to us ----- //

    std::vector<vk::PhysicalDevice> physicalDevices =
        p_instance->enumeratePhysicalDevices();
    LOG_TRACE(
        DEVICE, "{} physical devices available", physicalDevices.size()
    );
    if (physicalDevices.empty()) {
        LOG_CRITICAL(DEVICE, "Failed to find GPUs with vulkan support");
        throw std::runtime_error("");
    }

    // ----- choose the best (first) suitable physical device, ----- //
    //       get best queue family index                             //

    int64_t suitablePhysicalDeviceIndex = -1;
    for (uint32_t i = 0; i < physicalDevices.size(); ++i) {
        LOG_TRACE(DEVICE, "  - checking suitability of physical device {}", i);

        vk::PhysicalDevice physicalDevice = physicalDevices[i];

        vk::PhysicalDeviceFeatures supportedFeatures =
            physicalDevice.getFeatures();
        if (!supportedFeatures.samplerAnisotropy) {
            LOG_TRACE(DEVICE, "    - Sampler anisotropy not supported. Next");
            continue;
        }
        /// @todo check for supportedFeatures.depthBounds

        std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
            physicalDevice.getQueueFamilyProperties();
        LOG_TRACE(
            DEVICE, "    - {} queue families available",
            queueFamilyProperties.size()
        );

        int64_t suitableQueueFamilyIndex = -1;
        for (uint32_t j = 0; j < queueFamilyProperties.size(); ++j) {
            const vk::QueueFamilyProperties properties =
                queueFamilyProperties[j];

            if (!(properties.queueFlags & vk::QueueFlagBits::eGraphics)) {
                LOG_TRACE(
                    DEVICE, "      - Queue family {} doesn't support graphics",
                    j
                );
                continue;
            }

            LOG_TRACE(DEVICE, "      - queue family {} supports graphics", j);
            LOG_TRACE(DEVICE, "      - assuming implied support for present");

            suitableQueueFamilyIndex = j;
            break;
        }

        if (suitableQueueFamilyIndex == -1) {
            LOG_TRACE(DEVICE, "    - Graphics queue not supported. Next");
            continue;
        }

        LOG_INFO(DEVICE, "  - Physical device {} is suitable", i);
        suitablePhysicalDeviceIndex = i;
        break;
    }

    if (suitablePhysicalDeviceIndex == -1) {
        LOG_CRITICAL(DEVICE, "No suitable devices found");
        throw std::runtime_error("");
    }

    return physicalDevices[suitablePhysicalDeviceIndex];
}

uint32_t
quartz::rendering::Device::getGraphicsQueueFamilyIndex(
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(DEVICE, "");

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties =
        physicalDevice.getQueueFamilyProperties();
    LOG_TRACE(
        DEVICE, "{} queue families available for chosen physical device",
        queueFamilyProperties.size()
    );

    for (uint32_t j = 0; j < queueFamilyProperties.size(); ++j) {
        const vk::QueueFamilyProperties properties = queueFamilyProperties[j];

        if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
            LOG_TRACE(
                DEVICE,
                "  - queue family {} supports graphics queues", j
            );
            LOG_TRACE(
                DEVICE,
                "    - assuming implied support for present queues as well"
            );
            return j;
        }
    }

    LOG_CRITICAL(DEVICE, "Failed to find queue family index");
    throw std::runtime_error("");
}

std::vector<const char*>
quartz::rendering::Device::getEnabledPhysicalDeviceExtensionNames(
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(DEVICE, "");

    std::vector<const char*> requiredPhysicalDeviceExtensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    bool swapchainExtensionFound = false;

    std::vector<vk::ExtensionProperties> availablePhysicalDeviceExtensionProperties =
        physicalDevice.enumerateDeviceExtensionProperties();
    LOG_TRACE(
        DEVICE, "{} physical device extensions available",
        availablePhysicalDeviceExtensionProperties.size()
    );

    for (
        const vk::ExtensionProperties& extensionProperties :
        availablePhysicalDeviceExtensionProperties
    ) {
        LOG_TRACE(
            DEVICE, "  - {} [ version {} ]",
            extensionProperties.extensionName, extensionProperties.specVersion
        );

        if (
            extensionProperties.extensionName ==
            std::string("VK_KHR_portability_subset")
        ) {
            LOG_TRACE(DEVICE, "    - portability subset extension found");
            requiredPhysicalDeviceExtensionNames.push_back("VK_KHR_portability_subset");
        }

        if (
            extensionProperties.extensionName ==
            std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME)
        ) {
            LOG_TRACE(DEVICE, "    - swapchain extension found");
            swapchainExtensionFound = true;
        }
    }

    if (!swapchainExtensionFound) {
        LOG_CRITICAL(DEVICE, "{} extension not found", VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        throw std::runtime_error("");
    }

    return requiredPhysicalDeviceExtensionNames;
}

vk::UniqueDevice
quartz::rendering::Device::createVulkanLogicalDeviceUniquePtr(
    const vk::PhysicalDevice& physicalDevice,
    const uint32_t graphicsQueueFamilyIndex,
    const std::vector<const char*>& validationLayerNames,
    const std::vector<const char*>& physicalDeviceExtensionNames
) {
    LOG_FUNCTION_SCOPE_TRACE(
        DEVICE, "graphics queue family index = {}", graphicsQueueFamilyIndex
    );

    std::set<uint32_t> uniqueQueueFamilyIndices = { graphicsQueueFamilyIndex };
    std::vector<float> deviceQueuePriorities(
        uniqueQueueFamilyIndices.size(),
        1.0f
    );

    LOG_TRACE(
        DEVICE,
        "Creating vk::DeviceQueueCreateInfo for each of the unique queue "
        "family indices"
    );

    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    for (const uint32_t queueFamilyIndex : uniqueQueueFamilyIndices) {
        LOG_TRACE(DEVICE, "  - Unique index {}", queueFamilyIndex);

        deviceQueueCreateInfos.push_back(
            vk::DeviceQueueCreateInfo(
                {},
                queueFamilyIndex,
                deviceQueuePriorities
            )
        );
    }

    vk::PhysicalDeviceFeatures requestedPhysicalDeviceFeatures;
    requestedPhysicalDeviceFeatures.samplerAnisotropy = true;
    /// @todo enable requestedPhysicalDeviceFeatures.depthBounds

    vk::DeviceCreateInfo logicalDeviceCreateInfo(
        {},
        deviceQueueCreateInfos,
        validationLayerNames,
        physicalDeviceExtensionNames,
        &requestedPhysicalDeviceFeatures
    );

    vk::UniqueDevice uniqueLogicalDevice =
        physicalDevice.createDeviceUnique(logicalDeviceCreateInfo);

    if (!uniqueLogicalDevice) {
        LOG_CRITICAL(DEVICE, "Failed to create logical device");
        throw std::runtime_error("");
    }

    return uniqueLogicalDevice;
}

quartz::rendering::Device::Device(
    const quartz::rendering::Instance& renderingInstance
) :
    m_vulkanPhysicalDevice(
        quartz::rendering::Device::getBestPhysicalDevice(
            renderingInstance.getVulkanInstanceUniquePtr()
        )
    ),
    m_graphicsQueueFamilyIndex(
        quartz::rendering::Device::getGraphicsQueueFamilyIndex(
            m_vulkanPhysicalDevice
        )
    ),
    m_physicalDeviceExtensionNames(
        quartz::rendering::Device::getEnabledPhysicalDeviceExtensionNames(
            m_vulkanPhysicalDevice
        )
    ),
    mp_vulkanLogicalDevice(
        quartz::rendering::Device::createVulkanLogicalDeviceUniquePtr(
            m_vulkanPhysicalDevice,
            m_graphicsQueueFamilyIndex,
            renderingInstance.getValidationLayerNames(),
            m_physicalDeviceExtensionNames
        )
    ),
    m_vulkanGraphicsQueue(mp_vulkanLogicalDevice->getQueue(
        m_graphicsQueueFamilyIndex,
        0
    )),
    m_vulkanPresentQueue(mp_vulkanLogicalDevice->getQueue(
        m_graphicsQueueFamilyIndex,
        0
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Device::~Device() {
    LOG_FUNCTION_CALL_TRACEthis("");
}