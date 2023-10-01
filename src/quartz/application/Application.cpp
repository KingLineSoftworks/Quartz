#include <algorithm>
#include <functional>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/file_system/FileSystem.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/application/Application.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL quartz::Application::vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
    UNUSED void* p_userData
) {

    std::string messageTypeString;
    switch (messageType) {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            messageTypeString = "GENERAL";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            messageTypeString = "VALIDATION";
            break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            messageTypeString = "PERFORMANCE";
            break;
        default:
            messageTypeString = "UNKNOWN";
            break;
    }

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(quartz::loggers::VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(quartz::loggers::VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARNING(quartz::loggers::VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(quartz::loggers::VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        default:
            LOG_CRITICAL(quartz::loggers::VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
    };

    return VK_FALSE;
}

std::vector<const char*> quartz::Application::getEnabledValidationLayerNames(
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "enable validation layers = {}", validationLayersEnabled);

    if (!validationLayersEnabled) {
        LOG_TRACE(quartz::loggers::APPLICATION, "Validation layer support not requested. Not getting any validation layers");
        return {};
    }

    // ----- determine which validation layers are available ----- //

    std::vector<vk::LayerProperties> supportedLayerProperties = vk::enumerateInstanceLayerProperties();
    LOG_TRACE(quartz::loggers::APPLICATION, "{} extensions available", supportedLayerProperties.size());
    for (const vk::LayerProperties& layerProperties : supportedLayerProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {} [ version {} ]", layerProperties.layerName, layerProperties.specVersion);
    }

    // ----- get the required validation layers ----- //

    const std::vector<const char*> requiredValidationLayerNames = {
        "VK_LAYER_KHRONOS_validation"
    };

    // ----- ensure all required validation layers are available ----- //


    LOG_TRACE(quartz::loggers::APPLICATION, "{} instance validation layers required", requiredValidationLayerNames.size());
    for (const std::string& requiredValidationLayerName : requiredValidationLayerNames) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {}", requiredValidationLayerName);

        bool found = false;
        for (const vk::LayerProperties& layerProperties : supportedLayerProperties) {
            if (layerProperties.layerName == std::string(requiredValidationLayerName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOG_CRITICAL(quartz::loggers::APPLICATION, "Required validation layer {} is not available", requiredValidationLayerName);
            throw std::runtime_error("");
        }
    }

    return requiredValidationLayerNames;
}

std::vector<const char*> quartz::Application::getEnabledInstanceExtensionNames(
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "enable validation layers = {}", validationLayersEnabled);

    // ----- determine what instance extensions are available ----- //

    std::vector<vk::ExtensionProperties> availableInstanceExtensionProperties = vk::enumerateInstanceExtensionProperties();
    LOG_TRACE(quartz::loggers::APPLICATION, "{} instance extensions available", availableInstanceExtensionProperties.size());
    for (const vk::ExtensionProperties& extensionProperties : availableInstanceExtensionProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {} [ version {} ]", extensionProperties.extensionName, extensionProperties.specVersion);
    }

    // ----- get the extensions required by glfw ----- //

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    LOG_TRACE(quartz::loggers::APPLICATION, "{} required instance extensions from glfw", glfwExtensionCount);
    std::vector<const char*> requiredInstanceExtensionNames(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // ----- get extensions required by validation layers ----- //

    if (validationLayersEnabled) {
        LOG_TRACE(quartz::loggers::APPLICATION, "Requiring validation layer instance extension");
        requiredInstanceExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // ----- ensure that all of our required extensions are available ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "{} instance extensions required", requiredInstanceExtensionNames.size());
    for (const char* requiredInstanceExtensionName : requiredInstanceExtensionNames) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {}", requiredInstanceExtensionName);

        bool found = false;
        for (const vk::ExtensionProperties& extensionProperties : availableInstanceExtensionProperties) {
            if (extensionProperties.extensionName == std::string(requiredInstanceExtensionName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOG_CRITICAL(quartz::loggers::APPLICATION, "Required instance extension {} is not available", requiredInstanceExtensionName);
            throw std::runtime_error("");
        }
    }

    return requiredInstanceExtensionNames;
}

vk::UniqueInstance quartz::Application::createVulkanUniqueInstance(
    const std::string &applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const std::vector<const char*>& enabledValidationLayerNames,
    const std::vector<const char*>& enabledExtensionNames
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "{} {}.{}.{}", applicationName, applicationMajorVersion, applicationMinorVersion, applicationPatchVersion);

    vk::ApplicationInfo applicationInfo(
        applicationName.c_str(),
        VK_MAKE_VERSION(applicationMajorVersion, applicationMinorVersion, applicationPatchVersion),
        QUARTZ_NAME,
        VK_MAKE_VERSION(QUARTZ_MAJOR_VERSION, QUARTZ_MINOR_VERSION, QUARTZ_PATCH_VERSION),
        VK_API_VERSION_1_2
    );

    // ----- creating the instance ----- //

    vk::InstanceCreateInfo instanceCreateInfo(
        {},
        &applicationInfo,
        enabledValidationLayerNames,
        enabledExtensionNames
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create the vk::Instance");
    vk::UniqueInstance uniqueInstance = vk::createInstanceUnique(instanceCreateInfo);

    if (!uniqueInstance) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create the vk::Instance");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created the vk::Instance");

    return uniqueInstance;
}

vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> quartz::Application::createVulkanUniqueDebugUtilsMessenger(
    const vk::UniqueInstance& uniqueInstance,
    const vk::DispatchLoaderDynamic& dispatchLoaderDynamic,
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "enable validation layers = {}", validationLayersEnabled);

    if (!validationLayersEnabled) {
        LOG_TRACE(quartz::loggers::APPLICATION, "Default constructing DebugUtilsMessengerEXT because validation layers aren't enabled");
        return {};
    }

    const vk::DebugUtilsMessengerCreateFlagsEXT debugMessengerCreateFlags;

    const vk::DebugUtilsMessageSeverityFlagsEXT debugMessengerSeverityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );

    const vk::DebugUtilsMessageTypeFlagsEXT debugMessengerTypeFlags(
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
        vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
        vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
    );

    const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo(
        debugMessengerCreateFlags,
        debugMessengerSeverityFlags,
        debugMessengerTypeFlags,
        quartz::Application::vulkanDebugCallback,
        nullptr
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create the vk::DebugUtilsMessengerEXT");
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> uniqueDebugUtilsMessengerExt = uniqueInstance->createDebugUtilsMessengerEXTUnique(
        debugMessengerCreateInfo,
        nullptr,
        dispatchLoaderDynamic
    );

    if (!uniqueDebugUtilsMessengerExt) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create the vk::DebugUtilsMessengerEXT");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created the vk::DebugUtilsMessengerEXT");

    return uniqueDebugUtilsMessengerExt;
}

vk::UniqueSurfaceKHR quartz::Application::createVulkanSurface(
    const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
    const vk::UniqueInstance& uniqueInstance
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

#ifndef ON_MAC
    LOG_CRITICAL(quartz::loggers::APPLICATION, "No support for non-mac platforms currently. Inable to create vk::SurfaceKHR");
    throw std::runtime_error("");
#endif

    VkSurfaceKHR rawVulkanSurface;
    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create VkSurfaceKHR");
    VkResult createResult = glfwCreateWindowSurface(
        *uniqueInstance,
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        nullptr,
        &rawVulkanSurface
    );
    if (createResult != VK_SUCCESS) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create VkSurfaceKHR ( {} )", static_cast<int64_t>(createResult));
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created VkSurfaceKHR");

    vk::UniqueSurfaceKHR uniqueSurface(
        rawVulkanSurface,
        *uniqueInstance
    );
    if (!uniqueSurface) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create vk::SurfaceKHR from VkSurfaceKHR");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created vk::SurfaceKHR from VkSurfaceKHR");

    return uniqueSurface;
}

std::pair<vk::PhysicalDevice, quartz::Application::QueueFamilyIndices> quartz::Application::getBestPhysicalDeviceAndQueueFamilyIndices(
    const vk::UniqueInstance& uniqueInstance,
    const vk::UniqueSurfaceKHR& uniqueSurface
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    // ----- get list of physical devices available to us ----- //

    std::vector<vk::PhysicalDevice> physicalDevices = uniqueInstance->enumeratePhysicalDevices();
    LOG_TRACE(quartz::loggers::APPLICATION, "{} physical devices available", physicalDevices.size());
    if (physicalDevices.size() == 0) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to find GPUs with vulkan support");
        throw std::runtime_error("");
    }

    // ----- choose the best (first) suitable physical device and get its best queue family index ----- //

    vk::PhysicalDevice bestPhysicalDevice;
    std::optional<uint32_t> graphicsFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;
    bool foundSuitableDevice = false;
    for (uint32_t i = 0; i < physicalDevices.size(); ++i) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - checking suitability score of physical device {}", i);

        vk::PhysicalDevice physicalDevice = physicalDevices[i];
        foundSuitableDevice = false;

        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        LOG_TRACE(quartz::loggers::APPLICATION, "    - {} queue families available", queueFamilyProperties.size());

        for (uint32_t j = 0; j < queueFamilyProperties.size(); ++j) {
            const vk::QueueFamilyProperties properties = queueFamilyProperties[j];

            if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                LOG_TRACE(quartz::loggers::APPLICATION, "        - queue family {} supports graphics queues", j);
                graphicsFamilyIndex = j;
            }

            if (physicalDevice.getSurfaceSupportKHR(j, *uniqueSurface)) {
                LOG_TRACE(quartz::loggers::APPLICATION, "        - queue family {} supports present queues", j);
                presentFamilyIndex = j;
            }

            foundSuitableDevice = graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value();
            if (foundSuitableDevice) {
                break;
            }
        }

        if (foundSuitableDevice) {
            LOG_TRACE(quartz::loggers::APPLICATION, "    - device is suitable");
            bestPhysicalDevice = physicalDevice;
            break;
        }

        LOG_TRACE(quartz::loggers::APPLICATION, "    - device not suitable");
    }

    if (!foundSuitableDevice) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "No suitable devices found");
        throw std::runtime_error("");
    }

    return {
        bestPhysicalDevice,
        {
            graphicsFamilyIndex.value(),
            presentFamilyIndex.value()
        }
    };
}

std::vector<const char*> quartz::Application::getEnabledPhysicalDeviceExtensionNames(
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    std::vector<const char*> requiredPhysicalDeviceExtensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    bool swapchainExtensionFound = false;

    std::vector<vk::ExtensionProperties> availablePhysicalDeviceExtensionProperties = physicalDevice.enumerateDeviceExtensionProperties();
    LOG_TRACE(quartz::loggers::APPLICATION, "{} physical device extensions available", availablePhysicalDeviceExtensionProperties.size());
    for (const vk::ExtensionProperties& extensionProperties : availablePhysicalDeviceExtensionProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {} [ version {} ]", extensionProperties.extensionName, extensionProperties.specVersion);

        if (extensionProperties.extensionName == std::string("VK_KHR_portability_subset")) {
            LOG_TRACE(quartz::loggers::APPLICATION, "    - portability subset extension found");
            requiredPhysicalDeviceExtensionNames.push_back("VK_KHR_portability_subset");
        }

        if (extensionProperties.extensionName == std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
            LOG_TRACE(quartz::loggers::APPLICATION, "    - swapchain extension found");
            swapchainExtensionFound = true;
        }
    }

    if (!swapchainExtensionFound) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "{} extension not found", VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        throw std::runtime_error("");
    }

    return requiredPhysicalDeviceExtensionNames;
}

vk::UniqueDevice quartz::Application::createVulkanUniqueLogicalDevice(
    const vk::PhysicalDevice& physicalDevice,
    const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
    const std::vector<const char*>& validationLayerNames,
    const std::vector<const char*>& physicalDeviceExtensionNames
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "graphics queue family index = {} , present queue family index = {}", queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.presentFamilyIndex);

    std::set<uint32_t> uniqueQueueFamilyIndices = {queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.presentFamilyIndex};
    std::vector<float> deviceQueuePriorities(uniqueQueueFamilyIndices.size(), 1.0f);

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::DeviceQueueCreateInfo for each of the unique queue family indices");
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    for (const uint32_t queueFamilyIndex : uniqueQueueFamilyIndices) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - Unique index {}", queueFamilyIndex);
        deviceQueueCreateInfos.push_back(
            vk::DeviceQueueCreateInfo(
                {},
                queueFamilyIndex,
                deviceQueuePriorities
            )
        );
    }

    vk::PhysicalDeviceFeatures physicalDeviceFeatures;

    vk::DeviceCreateInfo logicalDeviceCreateInfo(
        {},
        deviceQueueCreateInfos,
        validationLayerNames,
        physicalDeviceExtensionNames,
        &physicalDeviceFeatures
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create logical device");
    vk::UniqueDevice uniqueLogicalDevice = physicalDevice.createDeviceUnique(logicalDeviceCreateInfo);

    if (!uniqueLogicalDevice) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create logical device");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created logical device");

    return uniqueLogicalDevice;
}

vk::Extent2D quartz::Application::getBestSwapExtent(
    const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    LOG_TRACE(quartz::loggers::APPLICATION, "Choosing best swap extent");
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        LOG_TRACE(quartz::loggers::APPLICATION, "Using capabilities's current swap extent");
        return surfaceCapabilities.currentExtent;
    }

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating our own swap extent (capabilities's current extent has width of uint32_t max value, so we must handle manually)");
    int32_t widthPixels;
    int32_t heightPixels;
    glfwGetFramebufferSize(
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        &widthPixels,
        &heightPixels
    );
    LOG_TRACE(quartz::loggers::APPLICATION, "Got GLFW framebuffer size of W {} pix x H {} pix", widthPixels, heightPixels);

    uint32_t adjustedWidthPixels = std::clamp(
        static_cast<uint32_t>(widthPixels),
        surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width
    );
    LOG_TRACE(quartz::loggers::APPLICATION, "Adjusted width of {} pixels (after clamping between {} and {})", adjustedWidthPixels, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);

    uint32_t adjustedHeightPixels = std::clamp(
        static_cast<uint32_t>(heightPixels),
        surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height
    );
    LOG_TRACE(quartz::loggers::APPLICATION, "Adjusted height of {} pixels (after clamping between {} and {})", adjustedHeightPixels, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating custom extent of W {} pix x H {} pix", adjustedWidthPixels, adjustedHeightPixels);
    vk::Extent2D customExtent(
        static_cast<uint32_t>(adjustedWidthPixels),
        static_cast<uint32_t>(adjustedHeightPixels)
    );

    return customExtent;
}

vk::SurfaceFormatKHR quartz::Application::getBestSurfaceFormat(
    const vk::UniqueSurfaceKHR& uniqueSurface,
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    LOG_TRACE(quartz::loggers::APPLICATION, "Getting surface formats from physical device");
    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(*uniqueSurface);
    if (surfaceFormats.empty()) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "No surface formats available for chosen physical device");
        throw std::runtime_error("");
    }

    LOG_TRACE(quartz::loggers::APPLICATION, "Choosing suitable surface format");
    for (const vk::SurfaceFormatKHR& surfaceFormat : surfaceFormats) {
        if (
            surfaceFormat.format == vk::Format::eB8G8R8A8Srgb &&
            surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
        ) {
            LOG_TRACE(quartz::loggers::APPLICATION, "  - found suitable surface format");
            return surfaceFormat;
        }
    }

    LOG_CRITICAL(quartz::loggers::APPLICATION, "No suitable surface formats found");
    throw std::runtime_error("");
}

vk::PresentModeKHR quartz::Application::getBestPresentMode(
    const vk::UniqueSurfaceKHR& uniqueSurface,
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    LOG_TRACE(quartz::loggers::APPLICATION, "Getting present modes from physical device");
    std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(*uniqueSurface);
    if (presentModes.empty()) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "No present modes available for chosen physical device");
        throw std::runtime_error("");
    }

    vk::PresentModeKHR bestPresentMode = vk::PresentModeKHR::eFifo;
    LOG_TRACE(quartz::loggers::APPLICATION, "Choosing best present mode");
    for (const vk::PresentModeKHR& presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            bestPresentMode = presentMode;
            break;
        }
    }

    if (bestPresentMode == vk::PresentModeKHR::eMailbox) {
        LOG_TRACE(quartz::loggers::APPLICATION, "Using mailbox present mode");
    } else {
        LOG_TRACE(quartz::loggers::APPLICATION, "Using fifo present mode");
    }

    return bestPresentMode;
}

vk::UniqueSwapchainKHR quartz::Application::createVulkanUniqueSwapchain(
    const vk::UniqueSurfaceKHR& uniqueSurface,
    const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
    const vk::Extent2D& swapExtent,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const vk::PresentModeKHR& presentMode
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    uint32_t imageCount = (surfaceCapabilities.maxImageCount != 0) ?
        surfaceCapabilities.maxImageCount :
        surfaceCapabilities.minImageCount + 1
    ;

    std::set uniqueQueueFamilyIndicesSet = {queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.presentFamilyIndex};
    std::vector uniqueQueueFamilyIndicesVector(uniqueQueueFamilyIndicesSet.begin(), uniqueQueueFamilyIndicesSet.end());

    vk::SwapchainCreateInfoKHR swapchainCreateInfo(
        {},
        *uniqueSurface,
        imageCount,
        surfaceFormat.format,
        surfaceFormat.colorSpace,
        swapExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        (uniqueQueueFamilyIndicesSet.size() > 1) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
        uniqueQueueFamilyIndicesVector,
        surfaceCapabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        presentMode,
        true
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create the vk::SwapchainKHR");
    vk::UniqueSwapchainKHR uniqueSwapchain = uniqueLogicalDevice->createSwapchainKHRUnique(swapchainCreateInfo);

    if (!uniqueSwapchain) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create the vk::SwapchainKHR");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created the vk::SwapchainKHR");

    return uniqueSwapchain;
}

std::vector<vk::UniqueImageView> quartz::Application::createVulkanUniqueImageViews(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const std::vector<vk::Image>& swapchainImages
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    std::vector<vk::UniqueImageView> uniqueImageViews;
    uniqueImageViews.reserve(swapchainImages.size());

    vk::ComponentMapping components(
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity,
        vk::ComponentSwizzle::eIdentity
    );

    vk::ImageSubresourceRange imageSubresourceRange(
        vk::ImageAspectFlagBits::eColor,
        0,
        1,
        0,
        1
    );

    for (uint32_t i = 0; i < swapchainImages.size(); ++i) {
        vk::ImageViewCreateInfo imageViewCreateInfo(
            {},
            swapchainImages[i],
            vk::ImageViewType::e2D,
            surfaceFormat.format,
            components,
            imageSubresourceRange
        );

        vk::UniqueImageView uniqueImageView = uniqueLogicalDevice->createImageViewUnique(imageViewCreateInfo);

        if (!uniqueImageView) {
            LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create vk::ImageView {}", i);
            throw std::runtime_error("");
        }

        LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created vk::ImageView {}", i);
        uniqueImageViews.push_back(std::move(uniqueImageView));
    }

    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created all {} vk::ImageViews(s)", swapchainImages.size());
    return uniqueImageViews;
}

vk::UniqueShaderModule quartz::Application::createVulkanUniqueShaderModule(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "{}", filepath);

    const std::vector<char> shaderBytes = quartz::util::FileSystem::readBytesFromFile(filepath);

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo(
        {},
        shaderBytes.size(),
        reinterpret_cast<const uint32_t*>(shaderBytes.data())
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create vk::ShaderModule");
    vk::UniqueShaderModule uniqueShaderModule = uniqueLogicalDevice->createShaderModuleUnique(shaderModuleCreateInfo);

    if (!uniqueShaderModule) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create vk::ShaderModule");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created vk::ShaderModule");

    return uniqueShaderModule;
}

quartz::Application::PipelineInformation quartz::Application::getPipelineInformation(
    const vk::Extent2D& swapExtent,
    const vk::UniqueShaderModule& uniqueVertexShaderModule,
    const vk::UniqueShaderModule& uniqueFragmentShaderModule
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    quartz::Application::PipelineInformation pipelineInformation;

    // ----- shader stage tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineShaderStageCreateInfo(s)");
    pipelineInformation.pipelineShaderStageCreateInfos = {
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eVertex,
            *uniqueVertexShaderModule,
            "main"
        ),
        vk::PipelineShaderStageCreateInfo(
            {},
            vk::ShaderStageFlagBits::eFragment,
            *uniqueFragmentShaderModule,
            "main"
        )
    };

    // ----- vertex input tings ----- //

    pipelineInformation.vertexInputBindingDescriptions = {};
    pipelineInformation.vertexInputAttributeDescriptions = {};

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineVertexInputStateCreateInfo");
    pipelineInformation.pipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo(
        {},
        pipelineInformation.vertexInputBindingDescriptions,
        pipelineInformation.vertexInputAttributeDescriptions
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineInputAssemblyStateCreateInfo");
    pipelineInformation.pipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
        {},
        vk::PrimitiveTopology::eTriangleList,
        false
    );

    // ----- tessellation tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineTessellationStateCreateInfo");
    pipelineInformation.pipelineTessellationStateCreateInfo = vk::PipelineTessellationStateCreateInfo();

    // ----- viewport and scissor tings ----- //

    pipelineInformation.viewports = {
        vk::Viewport(
            0.0f,
            0.0f,
            static_cast<float>(swapExtent.width),
            static_cast<float>(swapExtent.height),
            0.0f,
            1.0f
        )
    };

    pipelineInformation.scissorRectangles = {
        vk::Rect2D(
            vk::Offset2D(0.0f, 0.0f),
            swapExtent
        )
    };

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineViewportStateCreateInfo");
    LOG_TRACE(quartz::loggers::APPLICATION, "  - using {} viewports", pipelineInformation.viewports.size());
    for (const vk::Viewport& viewport : pipelineInformation.viewports) {
        LOG_TRACE(quartz::loggers::APPLICATION, "    - {} x {}", viewport.width, viewport.height);
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "  - using {} scissor rectangles", pipelineInformation.scissorRectangles.size());
    for (const vk::Rect2D& scissorRectangle : pipelineInformation.scissorRectangles) {
        LOG_TRACE(quartz::loggers::APPLICATION, "    - {} , {}", scissorRectangle.offset.x, scissorRectangle.offset.y);
    }
    pipelineInformation.pipelineViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo(
        {},
        pipelineInformation.viewports,
        pipelineInformation.scissorRectangles
    );

    // ----- rasterizer tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineRasterizationStateCreateInfo");
    pipelineInformation.pipelineRasterizationStateCreateInfo = vk::PipelineRasterizationStateCreateInfo(
        {},
        false,
        false,
        vk::PolygonMode::eFill,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eClockwise,
        false,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );

    // ----- multisample tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineMultisampleStateCreateInfo");
    pipelineInformation.pipelineMultisampleStateCreateInfo = vk::PipelineMultisampleStateCreateInfo(
        {},
        vk::SampleCountFlagBits::e1,
        false,
        1.0f,
        nullptr,
        false,
        false
    );

    // ----- depth stencil tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineDepthStencilStateCreateInfo");
    pipelineInformation.pipelineDepthStencilStateCreateInfo = vk::PipelineDepthStencilStateCreateInfo();

    // ----- color blend tings ----- //

    vk::ColorComponentFlags colorComponentFlags(
        vk::ColorComponentFlagBits::eR |
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA
    );

    pipelineInformation.colorBlendAttachmentStates = {
        vk::PipelineColorBlendAttachmentState(
            true,
            vk::BlendFactor::eSrcAlpha,
            vk::BlendFactor::eOneMinusSrcAlpha,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOne,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            colorComponentFlags
        )
    };

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineColorBlendStateCreateInfo");
    pipelineInformation.pipelineColorBlendStateCreateInfo = vk::PipelineColorBlendStateCreateInfo(
        {},
        false,
        vk::LogicOp::eCopy,
        pipelineInformation.colorBlendAttachmentStates,
        { 0.0f, 0.0f, 0.0f, 0.0f}
    );

    // ----- dynamic state tings ----- //

    pipelineInformation.dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    LOG_TRACE(quartz::loggers::APPLICATION, "Creating vk::PipelineDynamicStateCreateInfo", pipelineInformation.dynamicStates.size());
    LOG_TRACE(quartz::loggers::APPLICATION, "  - using {} dynamic states", pipelineInformation.dynamicStates.size());
    for (const vk::DynamicState& dynamicState : pipelineInformation.dynamicStates) {
        LOG_TRACE(quartz::loggers::APPLICATION, "    - {}", static_cast<uint32_t>(dynamicState));
    }
    pipelineInformation.pipelineDynamicStateCreateInfo = vk::PipelineDynamicStateCreateInfo(
        {},
        pipelineInformation.dynamicStates
    );

    // ----- returning ----- //

    return pipelineInformation;
}

vk::UniquePipelineLayout quartz::Application::createVulkanUniquePipelineLayout(
    const vk::UniqueDevice& uniqueLogicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        {},
        0,
        nullptr,
        0,
        nullptr
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create vk::PipelineLayout");
    vk::UniquePipelineLayout uniquePipelineLayout = uniqueLogicalDevice->createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    if (!uniquePipelineLayout) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create vk::PipelineLayout");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created vk::PipelineLayout");

    return uniquePipelineLayout;
}

vk::UniqueRenderPass quartz::Application::createVulkanUniqueRenderPass(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::Format& surfaceFormatFormat
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    vk::AttachmentDescription colorAttachment(
        {},
        surfaceFormatFormat,
        vk::SampleCountFlagBits::e1,
        vk::AttachmentLoadOp::eClear,
        vk::AttachmentStoreOp::eStore,
        vk::AttachmentLoadOp::eDontCare,
        vk::AttachmentStoreOp::eDontCare,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::ePresentSrcKHR
    );

    vk::AttachmentReference colorAttachmentRef(
        0,
        vk::ImageLayout::eColorAttachmentOptimal
    );

    vk::SubpassDescription subpassDescription(
        {},
        vk::PipelineBindPoint::eGraphics,
        {},
        colorAttachmentRef,
        {},
        {},
        {}
    );

    vk::RenderPassCreateInfo renderPassCreateInfo(
        {},
        colorAttachment,
        subpassDescription,
        {}
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create vk::RenderPass");
    vk::UniqueRenderPass uniqueRenderPass = uniqueLogicalDevice->createRenderPassUnique(renderPassCreateInfo);

    if (!uniqueRenderPass) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create vk::RenderPass");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created vk::RenderPass");

    return uniqueRenderPass;
}

vk::UniquePipeline quartz::Application::createVulkanUniqueGraphicsPipeline(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const quartz::Application::PipelineInformation& pipelineInformation,
    const vk::UniquePipelineLayout& uniquePipelineLayout,
    const vk::UniqueRenderPass& uniqueRenderPass
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo(
        {},
        pipelineInformation.pipelineShaderStageCreateInfos,
        &pipelineInformation.pipelineVertexInputStateCreateInfo,
        &pipelineInformation.pipelineInputAssemblyStateCreateInfo,
        &pipelineInformation.pipelineTessellationStateCreateInfo,
        &pipelineInformation.pipelineViewportStateCreateInfo,
        &pipelineInformation.pipelineRasterizationStateCreateInfo,
        &pipelineInformation.pipelineMultisampleStateCreateInfo,
        &pipelineInformation.pipelineDepthStencilStateCreateInfo,
        &pipelineInformation.pipelineColorBlendStateCreateInfo,
        &pipelineInformation.pipelineDynamicStateCreateInfo,
        *uniquePipelineLayout,
        *uniqueRenderPass,
        0,
        VK_NULL_HANDLE,
        -1
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create vk::Pipeline");
    vk::ResultValue<vk::UniquePipeline> uniqueGraphicsPipelineResult = uniqueLogicalDevice->createGraphicsPipelineUnique(
        VK_NULL_HANDLE,
        graphicsPipelineCreateInfo
    );

    if (uniqueGraphicsPipelineResult.result != vk::Result::eSuccess) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create vk::Pipeline");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created vk::Pipeline");

    return std::move(uniqueGraphicsPipelineResult.value);
}

quartz::Application::Application(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels,
    const bool validationLayersEnabled
) :
    m_applicationName(applicationName),
    m_majorVersion(applicationMajorVersion),
    m_minorVersion(applicationMinorVersion),
    m_patchVersion(applicationPatchVersion),
    m_validationLayerNames(quartz::Application::getEnabledValidationLayerNames(validationLayersEnabled)),
    m_instanceExtensionNames(quartz::Application::getEnabledInstanceExtensionNames(validationLayersEnabled)),
    m_vulkanUniqueInstance(quartz::Application::createVulkanUniqueInstance(
        m_applicationName,
        m_majorVersion,
        m_minorVersion,
        m_patchVersion,
        m_validationLayerNames,
        m_instanceExtensionNames
    )),
    m_vulkanDispatchLoaderDynamic(*m_vulkanUniqueInstance, vkGetInstanceProcAddr),
    m_vulkanUniqueDebugMessenger(quartz::Application::createVulkanUniqueDebugUtilsMessenger(
        m_vulkanUniqueInstance,
        m_vulkanDispatchLoaderDynamic,
        validationLayersEnabled
    )),
    mp_window(std::make_shared<quartz::rendering::Window>(
        m_applicationName,
        windowWidthPixels,
        windowHeightPixels
    )),
    m_vulkanUniqueSurface(quartz::Application::createVulkanSurface(
        mp_window->getGLFWwindowPtr(),
        m_vulkanUniqueInstance
    )),
    m_vulkanPhysicalDeviceAndQueueFamilyIndex(quartz::Application::getBestPhysicalDeviceAndQueueFamilyIndices(
        m_vulkanUniqueInstance,
        m_vulkanUniqueSurface
    )),
    m_physicalDeviceExtensionNames(quartz::Application::getEnabledPhysicalDeviceExtensionNames(m_vulkanPhysicalDeviceAndQueueFamilyIndex.first)),
    m_vulkanUniqueLogicalDevice(quartz::Application::createVulkanUniqueLogicalDevice(
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.first,
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second,
        m_validationLayerNames,
        m_physicalDeviceExtensionNames
    )),
    m_vulkanGraphicsQueue(m_vulkanUniqueLogicalDevice->getQueue(
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second.graphicsFamilyIndex,
        0
    )),
    m_vulkanPresentQueue(m_vulkanUniqueLogicalDevice->getQueue(
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second.presentFamilyIndex,
        0
    )),
    m_vulkanSurfaceCapabilities(m_vulkanPhysicalDeviceAndQueueFamilyIndex.first.getSurfaceCapabilitiesKHR(*m_vulkanUniqueSurface)),
    m_vulkanSwapExtent(quartz::Application::getBestSwapExtent(
        mp_window->getGLFWwindowPtr(),
        m_vulkanSurfaceCapabilities
    )),
    m_vulkanSurfaceFormat(quartz::Application::getBestSurfaceFormat(
        m_vulkanUniqueSurface,
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.first
    )),
    m_vulkanPresentMode(quartz::Application::getBestPresentMode(
        m_vulkanUniqueSurface,
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.first
    )),
    m_vulkanUniqueSwapchain(quartz::Application::createVulkanUniqueSwapchain(
        m_vulkanUniqueSurface,
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second,
        m_vulkanUniqueLogicalDevice,
        m_vulkanSurfaceCapabilities,
        m_vulkanSwapExtent,
        m_vulkanSurfaceFormat,
        m_vulkanPresentMode
    )),
    m_vulkanSwapchainImages(m_vulkanUniqueLogicalDevice->getSwapchainImagesKHR(*m_vulkanUniqueSwapchain)),
    m_vulkanUniqueImageViews(quartz::Application::createVulkanUniqueImageViews(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSurfaceFormat,
        m_vulkanSwapchainImages
    )),
    m_vulkanUniqueVertexShaderModule(quartz::Application::createVulkanUniqueShaderModule(
        m_vulkanUniqueLogicalDevice,
        quartz::util::FileSystem::getAbsoluteFilepathInProject("shader.vert.spv")
    )),
    m_vulkanUniqueFragmentShaderModule(quartz::Application::createVulkanUniqueShaderModule(
        m_vulkanUniqueLogicalDevice,
        quartz::util::FileSystem::getAbsoluteFilepathInProject("shader.frag.spv")
    )),
    m_pipelineInformation(quartz::Application::getPipelineInformation(
        m_vulkanSwapExtent,
        m_vulkanUniqueVertexShaderModule,
        m_vulkanUniqueFragmentShaderModule
    )),
    m_vulkanUniquePipelineLayout(quartz::Application::createVulkanUniquePipelineLayout(
        m_vulkanUniqueLogicalDevice
    )),
    m_vulkanUniqueRenderPass(quartz::Application::createVulkanUniqueRenderPass(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSurfaceFormat.format
    )),
    m_vulkanUniqueGraphicsPipeline(quartz::Application::createVulkanUniqueGraphicsPipeline(
        m_vulkanUniqueLogicalDevice,
        m_pipelineInformation,
        m_vulkanUniquePipelineLayout,
        m_vulkanUniqueRenderPass
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("{} version {}.{}.{}", m_applicationName, m_majorVersion, m_minorVersion, m_patchVersion);
}

quartz::Application::~Application() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application::run() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    // ----- drop that ass at me from an egregarious angle ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "Beginning main loop");
    while(!mp_window->shouldClose()) {
        glfwPollEvents();
    }
}