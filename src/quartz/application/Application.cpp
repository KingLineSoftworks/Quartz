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
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "enable validation layers = {}", validationLayersEnabled);

    if (!validationLayersEnabled) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Validation layer support not requested. Not getting any validation layers");
        return {};
    }

    // ----- determine which validation layers are available ----- //

    std::vector<vk::LayerProperties> supportedLayerProperties = vk::enumerateInstanceLayerProperties();
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} extensions available", supportedLayerProperties.size());
    for (const vk::LayerProperties& layerProperties : supportedLayerProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - {} [ version {} ]", layerProperties.layerName, layerProperties.specVersion);
    }

    // ----- get the required validation layers ----- //

    const std::vector<const char*> requiredValidationLayerNames = {
        "VK_LAYER_KHRONOS_validation"
    };

    // ----- ensure all required validation layers are available ----- //


    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} instance validation layers required", requiredValidationLayerNames.size());
    for (const std::string& requiredValidationLayerName : requiredValidationLayerNames) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - {}", requiredValidationLayerName);

        bool found = false;
        for (const vk::LayerProperties& layerProperties : supportedLayerProperties) {
            if (layerProperties.layerName == std::string(requiredValidationLayerName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Required validation layer {} is not available", requiredValidationLayerName);
            throw std::runtime_error("");
        }
    }

    return requiredValidationLayerNames;
}

std::vector<const char*> quartz::Application::getEnabledInstanceExtensionNames(
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "enable validation layers = {}", validationLayersEnabled);

    // ----- determine what instance extensions are available ----- //

    std::vector<vk::ExtensionProperties> availableInstanceExtensionProperties = vk::enumerateInstanceExtensionProperties();
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} instance extensions available", availableInstanceExtensionProperties.size());
    for (const vk::ExtensionProperties& extensionProperties : availableInstanceExtensionProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - {} [ version {} ]", extensionProperties.extensionName, extensionProperties.specVersion);
    }

    // ----- get the extensions required by glfw ----- //

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} required instance extensions from glfw", glfwExtensionCount);
    std::vector<const char*> requiredInstanceExtensionNames(glfwExtensions, glfwExtensions + glfwExtensionCount);

    // ----- get extensions required by validation layers ----- //

    if (validationLayersEnabled) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Requiring validation layer instance extension");
        requiredInstanceExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // ----- ensure that all of our required extensions are available ----- //

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} instance extensions required", requiredInstanceExtensionNames.size());
    for (const char* requiredInstanceExtensionName : requiredInstanceExtensionNames) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - {}", requiredInstanceExtensionName);

        bool found = false;
        for (const vk::ExtensionProperties& extensionProperties : availableInstanceExtensionProperties) {
            if (extensionProperties.extensionName == std::string(requiredInstanceExtensionName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Required instance extension {} is not available", requiredInstanceExtensionName);
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
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} {}.{}.{}", applicationName, applicationMajorVersion, applicationMinorVersion, applicationPatchVersion);

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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create the vk::Instance");
    vk::UniqueInstance uniqueInstance = vk::createInstanceUnique(instanceCreateInfo);

    if (!uniqueInstance) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create the vk::Instance");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created the vk::Instance");

    return uniqueInstance;
}

vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> quartz::Application::createVulkanUniqueDebugUtilsMessenger(
    const vk::UniqueInstance& uniqueInstance,
    const vk::DispatchLoaderDynamic& dispatchLoaderDynamic,
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "enable validation layers = {}", validationLayersEnabled);

    if (!validationLayersEnabled) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Default constructing DebugUtilsMessengerEXT because validation layers aren't enabled");
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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create the vk::DebugUtilsMessengerEXT");
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> uniqueDebugUtilsMessengerExt = uniqueInstance->createDebugUtilsMessengerEXTUnique(
        debugMessengerCreateInfo,
        nullptr,
        dispatchLoaderDynamic
    );

    if (!uniqueDebugUtilsMessengerExt) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create the vk::DebugUtilsMessengerEXT");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created the vk::DebugUtilsMessengerEXT");

    return uniqueDebugUtilsMessengerExt;
}

vk::UniqueSurfaceKHR quartz::Application::createVulkanSurface(
    const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
    const vk::UniqueInstance& uniqueInstance
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

#ifndef ON_MAC
    LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "No support for non-mac platforms currently. Inable to create vk::SurfaceKHR");
    throw std::runtime_error("");
#endif

    VkSurfaceKHR rawVulkanSurface;
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create VkSurfaceKHR");
    VkResult createResult = glfwCreateWindowSurface(
        *uniqueInstance,
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        nullptr,
        &rawVulkanSurface
    );
    if (createResult != VK_SUCCESS) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create VkSurfaceKHR ( {} )", static_cast<int64_t>(createResult));
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created VkSurfaceKHR");

    vk::UniqueSurfaceKHR uniqueSurface(
        rawVulkanSurface,
        *uniqueInstance
    );
    if (!uniqueSurface) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::SurfaceKHR from VkSurfaceKHR");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::SurfaceKHR from VkSurfaceKHR");

    return uniqueSurface;
}

std::pair<vk::PhysicalDevice, quartz::Application::QueueFamilyIndices> quartz::Application::getBestPhysicalDeviceAndQueueFamilyIndices(
    const vk::UniqueInstance& uniqueInstance,
    const vk::UniqueSurfaceKHR& uniqueSurface
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    // ----- get list of physical devices available to us ----- //

    std::vector<vk::PhysicalDevice> physicalDevices = uniqueInstance->enumeratePhysicalDevices();
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} physical devices available", physicalDevices.size());
    if (physicalDevices.size() == 0) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to find GPUs with vulkan support");
        throw std::runtime_error("");
    }

    // ----- choose the best (first) suitable physical device and get its best queue family index ----- //

    vk::PhysicalDevice bestPhysicalDevice;
    std::optional<uint32_t> graphicsFamilyIndex;
    std::optional<uint32_t> presentFamilyIndex;
    bool foundSuitableDevice = false;
    for (uint32_t i = 0; i < physicalDevices.size(); ++i) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - checking suitability score of physical device {}", i);

        vk::PhysicalDevice physicalDevice = physicalDevices[i];
        foundSuitableDevice = false;

        std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - {} queue families available", queueFamilyProperties.size());

        for (uint32_t j = 0; j < queueFamilyProperties.size(); ++j) {
            const vk::QueueFamilyProperties properties = queueFamilyProperties[j];

            if (properties.queueFlags & vk::QueueFlagBits::eGraphics) {
                LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "        - queue family {} supports graphics queues", j);
                graphicsFamilyIndex = j;
            }

            if (physicalDevice.getSurfaceSupportKHR(j, *uniqueSurface)) {
                LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "        - queue family {} supports present queues", j);
                presentFamilyIndex = j;
            }

            foundSuitableDevice = graphicsFamilyIndex.has_value() && presentFamilyIndex.has_value();
            if (foundSuitableDevice) {
                break;
            }
        }

        if (foundSuitableDevice) {
            LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - device is suitable");
            bestPhysicalDevice = physicalDevice;
            break;
        }

        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - device not suitable");
    }

    if (!foundSuitableDevice) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "No suitable devices found");
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
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    std::vector<const char*> requiredPhysicalDeviceExtensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    bool swapchainExtensionFound = false;

    std::vector<vk::ExtensionProperties> availablePhysicalDeviceExtensionProperties = physicalDevice.enumerateDeviceExtensionProperties();
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} physical device extensions available", availablePhysicalDeviceExtensionProperties.size());
    for (const vk::ExtensionProperties& extensionProperties : availablePhysicalDeviceExtensionProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - {} [ version {} ]", extensionProperties.extensionName, extensionProperties.specVersion);

        if (extensionProperties.extensionName == std::string("VK_KHR_portability_subset")) {
            LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - portability subset extension found");
            requiredPhysicalDeviceExtensionNames.push_back("VK_KHR_portability_subset");
        }

        if (extensionProperties.extensionName == std::string(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
            LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - swapchain extension found");
            swapchainExtensionFound = true;
        }
    }

    if (!swapchainExtensionFound) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "{} extension not found", VK_KHR_SWAPCHAIN_EXTENSION_NAME);
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
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "graphics queue family index = {} , present queue family index = {}", queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.presentFamilyIndex);

    std::set<uint32_t> uniqueQueueFamilyIndices = {queueFamilyIndices.graphicsFamilyIndex, queueFamilyIndices.presentFamilyIndex};
    std::vector<float> deviceQueuePriorities(uniqueQueueFamilyIndices.size(), 1.0f);

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::DeviceQueueCreateInfo for each of the unique queue family indices");
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;
    for (const uint32_t queueFamilyIndex : uniqueQueueFamilyIndices) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - Unique index {}", queueFamilyIndex);
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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create logical device");
    vk::UniqueDevice uniqueLogicalDevice = physicalDevice.createDeviceUnique(logicalDeviceCreateInfo);

    if (!uniqueLogicalDevice) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create logical device");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created logical device");

    return uniqueLogicalDevice;
}

vk::Extent2D quartz::Application::getBestSwapExtent(
    const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Choosing best swap extent");
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Using capabilities's current swap extent");
        return surfaceCapabilities.currentExtent;
    }

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating our own swap extent (capabilities's current extent has width of uint32_t max value, so we must handle manually)");
    int32_t widthPixels;
    int32_t heightPixels;
    glfwGetFramebufferSize(
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        &widthPixels,
        &heightPixels
    );
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Got GLFW framebuffer size of W {} pix x H {} pix", widthPixels, heightPixels);

    uint32_t adjustedWidthPixels = std::clamp(
        static_cast<uint32_t>(widthPixels),
        surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width
    );
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Adjusted width of {} pixels (after clamping between {} and {})", adjustedWidthPixels, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);

    uint32_t adjustedHeightPixels = std::clamp(
        static_cast<uint32_t>(heightPixels),
        surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height
    );
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Adjusted height of {} pixels (after clamping between {} and {})", adjustedHeightPixels, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating custom extent of W {} pix x H {} pix", adjustedWidthPixels, adjustedHeightPixels);
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
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Getting surface formats from physical device");
    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(*uniqueSurface);
    if (surfaceFormats.empty()) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "No surface formats available for chosen physical device");
        throw std::runtime_error("");
    }

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Choosing suitable surface format");
    for (const vk::SurfaceFormatKHR& surfaceFormat : surfaceFormats) {
        if (
            surfaceFormat.format == vk::Format::eB8G8R8A8Srgb &&
            surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
        ) {
            LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - found suitable surface format");
            return surfaceFormat;
        }
    }

    LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "No suitable surface formats found");
    throw std::runtime_error("");
}

vk::PresentModeKHR quartz::Application::getBestPresentMode(
    const vk::UniqueSurfaceKHR& uniqueSurface,
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Getting present modes from physical device");
    std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(*uniqueSurface);
    if (presentModes.empty()) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "No present modes available for chosen physical device");
        throw std::runtime_error("");
    }

    vk::PresentModeKHR bestPresentMode = vk::PresentModeKHR::eFifo;
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Choosing best present mode");
    for (const vk::PresentModeKHR& presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            bestPresentMode = presentMode;
            break;
        }
    }

    if (bestPresentMode == vk::PresentModeKHR::eMailbox) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Using mailbox present mode");
    } else {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Using fifo present mode");
    }

    return bestPresentMode;
}

vk::UniqueSwapchainKHR quartz::Application::createVulkanUniqueSwapchain(
    const vk::UniqueSurfaceKHR& uniqueSurface,
    const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
    const vk::Extent2D& swapchainExtent,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const vk::PresentModeKHR& presentMode
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

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
        swapchainExtent,
        1,
        vk::ImageUsageFlagBits::eColorAttachment,
        (uniqueQueueFamilyIndicesSet.size() > 1) ? vk::SharingMode::eConcurrent : vk::SharingMode::eExclusive,
        uniqueQueueFamilyIndicesVector,
        surfaceCapabilities.currentTransform,
        vk::CompositeAlphaFlagBitsKHR::eOpaque,
        presentMode,
        true
    );

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create the vk::SwapchainKHR");
    vk::UniqueSwapchainKHR uniqueSwapchain = uniqueLogicalDevice->createSwapchainKHRUnique(swapchainCreateInfo);

    if (!uniqueSwapchain) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create the vk::SwapchainKHR");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created the vk::SwapchainKHR");

    return uniqueSwapchain;
}

std::vector<vk::UniqueImageView> quartz::Application::createVulkanUniqueSwapchainImageViews(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::SurfaceFormatKHR& surfaceFormat,
    const std::vector<vk::Image>& swapchainImages
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

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
            LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::ImageView {}", i);
            throw std::runtime_error("");
        }

        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::ImageView {}", i);
        uniqueImageViews.push_back(std::move(uniqueImageView));
    }

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created all {} vk::ImageViews(s)", swapchainImages.size());
    return uniqueImageViews;
}

vk::UniqueShaderModule quartz::Application::createVulkanUniqueShaderModule(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const std::string& filepath
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{}", filepath);

    const std::vector<char> shaderBytes = quartz::util::FileSystem::readBytesFromFile(filepath);

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo(
        {},
        shaderBytes.size(),
        reinterpret_cast<const uint32_t*>(shaderBytes.data())
    );

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create vk::ShaderModule");
    vk::UniqueShaderModule uniqueShaderModule = uniqueLogicalDevice->createShaderModuleUnique(shaderModuleCreateInfo);

    if (!uniqueShaderModule) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::ShaderModule");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::ShaderModule");

    return uniqueShaderModule;
}

quartz::Application::PipelineInformation quartz::Application::getPipelineInformation(
    const vk::Extent2D& swapchainExtent,
    const vk::UniqueShaderModule& uniqueVertexShaderModule,
    const vk::UniqueShaderModule& uniqueFragmentShaderModule
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    quartz::Application::PipelineInformation pipelineInformation;

    // ----- shader stage tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineShaderStageCreateInfo(s)");
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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineVertexInputStateCreateInfo");
    pipelineInformation.pipelineVertexInputStateCreateInfo = vk::PipelineVertexInputStateCreateInfo(
        {},
        pipelineInformation.vertexInputBindingDescriptions,
        pipelineInformation.vertexInputAttributeDescriptions
    );

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineInputAssemblyStateCreateInfo");
    pipelineInformation.pipelineInputAssemblyStateCreateInfo = vk::PipelineInputAssemblyStateCreateInfo(
        {},
        vk::PrimitiveTopology::eTriangleList,
        false
    );

    // ----- tessellation tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineTessellationStateCreateInfo");
    pipelineInformation.pipelineTessellationStateCreateInfo = vk::PipelineTessellationStateCreateInfo();

    // ----- viewport and scissor tings ----- //

    pipelineInformation.viewports = {
        vk::Viewport(
            0.0f,
            0.0f,
            static_cast<float>(swapchainExtent.width),
            static_cast<float>(swapchainExtent.height),
            0.0f,
            1.0f
        )
    };

    pipelineInformation.scissorRectangles = {
        vk::Rect2D(
            vk::Offset2D(0.0f, 0.0f),
            swapchainExtent
        )
    };

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineViewportStateCreateInfo");
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - using {} viewports", pipelineInformation.viewports.size());
    for (const vk::Viewport& viewport : pipelineInformation.viewports) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - {} x {}", viewport.width, viewport.height);
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - using {} scissor rectangles", pipelineInformation.scissorRectangles.size());
    for (const vk::Rect2D& scissorRectangle : pipelineInformation.scissorRectangles) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - {} , {}", scissorRectangle.offset.x, scissorRectangle.offset.y);
    }
    pipelineInformation.pipelineViewportStateCreateInfo = vk::PipelineViewportStateCreateInfo(
        {},
        pipelineInformation.viewports,
        pipelineInformation.scissorRectangles
    );

    // ----- rasterizer tings ----- //

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineRasterizationStateCreateInfo");
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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineMultisampleStateCreateInfo");
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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineDepthStencilStateCreateInfo");
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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineColorBlendStateCreateInfo");
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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Creating vk::PipelineDynamicStateCreateInfo", pipelineInformation.dynamicStates.size());
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - using {} dynamic states", pipelineInformation.dynamicStates.size());
    for (const vk::DynamicState& dynamicState : pipelineInformation.dynamicStates) {
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "    - {}", static_cast<uint32_t>(dynamicState));
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
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo(
        {},
        0,
        nullptr,
        0,
        nullptr
    );

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create vk::PipelineLayout");
    vk::UniquePipelineLayout uniquePipelineLayout = uniqueLogicalDevice->createPipelineLayoutUnique(pipelineLayoutCreateInfo);

    if (!uniquePipelineLayout) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::PipelineLayout");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::PipelineLayout");

    return uniquePipelineLayout;
}

vk::UniqueRenderPass quartz::Application::createVulkanUniqueRenderPass(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::Format& surfaceFormatFormat
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

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

    vk::SubpassDependency subpassDependency(
        VK_SUBPASS_EXTERNAL,
        0,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {},
        vk::AccessFlagBits::eColorAttachmentWrite,
        {}
    );

    vk::RenderPassCreateInfo renderPassCreateInfo(
        {},
        colorAttachment,
        subpassDescription,
        subpassDependency
    );

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create vk::RenderPass");
    vk::UniqueRenderPass uniqueRenderPass = uniqueLogicalDevice->createRenderPassUnique(renderPassCreateInfo);

    if (!uniqueRenderPass) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::RenderPass");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::RenderPass");

    return uniqueRenderPass;
}

vk::UniquePipeline quartz::Application::createVulkanUniqueGraphicsPipeline(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const quartz::Application::PipelineInformation& pipelineInformation,
    const vk::UniquePipelineLayout& uniquePipelineLayout,
    const vk::UniqueRenderPass& uniqueRenderPass
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

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

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create vk::Pipeline");
    vk::ResultValue<vk::UniquePipeline> uniqueGraphicsPipelineResult = uniqueLogicalDevice->createGraphicsPipelineUnique(
        VK_NULL_HANDLE,
        graphicsPipelineCreateInfo
    );

    if (uniqueGraphicsPipelineResult.result != vk::Result::eSuccess) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::Pipeline");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::Pipeline");

    return std::move(uniqueGraphicsPipelineResult.value);
}

std::vector<vk::UniqueFramebuffer> quartz::Application::createVulkanUniqueFramebuffers(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::Extent2D& swapchainExtent,
    const std::vector<vk::UniqueImageView>& uniqueSwapchainImageViews,
    const vk::UniqueRenderPass& uniqueRenderPass
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} swapchain image views", uniqueSwapchainImageViews.size());

    std::vector<vk::UniqueFramebuffer> uniqueFramebuffers(uniqueSwapchainImageViews.size());

    for (uint32_t i = 0; i < uniqueSwapchainImageViews.size(); ++i) {
        vk::FramebufferCreateInfo framebufferCreateInfo(
            {},
            *uniqueRenderPass,
            *uniqueSwapchainImageViews[i],
            swapchainExtent.width,
            swapchainExtent.height,
            1
        );

        uniqueFramebuffers[i] = uniqueLogicalDevice->createFramebufferUnique(framebufferCreateInfo);

        if (!uniqueFramebuffers[i]) {
            LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::Framebuffer {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::Framebuffer {}", i);
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created {} vk::Framebuffer(s)", uniqueFramebuffers.size());

    return uniqueFramebuffers;
}

vk::UniqueCommandPool quartz::Application::createVulkanUniqueCommandPool(
    const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
    const vk::UniqueDevice& uniqueLogicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "");

    vk::CommandPoolCreateInfo commandPoolCreateInfo(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        queueFamilyIndices.graphicsFamilyIndex
    );

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create vk::CommandPool");
    vk::UniqueCommandPool uniqueCommandPool = uniqueLogicalDevice->createCommandPoolUnique(commandPoolCreateInfo);

    if (!uniqueCommandPool) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::CommandPool");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::CommandPool");

    return uniqueCommandPool;
};

std::vector<vk::UniqueCommandBuffer> quartz::Application::createVulkanUniqueCommandBuffers(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const vk::Extent2D& swapchainExtent,
    const std::vector<vk::Image>& swapchainImages,
    const vk::UniqueRenderPass& uniqueRenderPass,
    const vk::UniquePipeline& uniqueGraphicsPipeline,
    const std::vector<vk::UniqueFramebuffer>& uniqueFramebuffers,
    const vk::UniqueCommandPool& uniqueCommandPool
) {
    const uint32_t desiredCommandBufferCount = swapchainImages.size();
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} max frames in flight", desiredCommandBufferCount);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo(
        *uniqueCommandPool,
        vk::CommandBufferLevel::ePrimary,
        desiredCommandBufferCount
    );

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to allocate {} vk::CommandBuffer(s)", desiredCommandBufferCount);
    std::vector<vk::UniqueCommandBuffer> uniqueCommandBuffers = uniqueLogicalDevice->allocateCommandBuffersUnique(commandBufferAllocateInfo);

    if (uniqueCommandBuffers.size() != desiredCommandBufferCount) {
        LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Allocated {} vk::CommandBuffer(s) instead of {}", uniqueCommandBuffers.size(), desiredCommandBufferCount);
        throw std::runtime_error("");
    }

    for (uint32_t i = 0; i < uniqueCommandBuffers.size(); ++i) {
        if (!uniqueCommandBuffers[i]) {
            LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to allocate vk::CommandBuffer {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully allocated vk::CommandBuffer {}", i);
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - Recording commands");

        // ----- record things into a command buffer ? ----- //

        vk::CommandBufferBeginInfo commandBufferBeginInfo(
            {},
            {}
        );

        uniqueCommandBuffers[i]->begin(commandBufferBeginInfo);

        // ----- start a render pass ----- //

        vk::ClearValue clearScreenColor(
            vk::ClearColorValue(
                std::array<float, 4>{ 0.0f, 0.0f, 0.0f, 0.0f }
            )
        );

        vk::Rect2D renderPassRenderArea(
            vk::Offset2D(0.0f, 0.0f),
            swapchainExtent
        );

        vk::RenderPassBeginInfo renderPassBeginInfo(
            *uniqueRenderPass,
            *(uniqueFramebuffers[i]),
            renderPassRenderArea,
            clearScreenColor
        );

        uniqueCommandBuffers[i]->beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

        // ----- draw (but first bind graphics pipeline and set up viewport and scissor) ----- //

        uniqueCommandBuffers[i]->bindPipeline(vk::PipelineBindPoint::eGraphics, *uniqueGraphicsPipeline);

        vk::Viewport viewport(
            0.0f,
            0.0f,
            static_cast<float>(swapchainExtent.width),
            static_cast<float>(swapchainExtent.height),
            0.0f,
            1.0f
        );
        uniqueCommandBuffers[i]->setViewport(
            0,
            viewport
        );

        vk::Rect2D scissor(
            vk::Offset2D(0.0f, 0.0f),
            swapchainExtent
        );
        uniqueCommandBuffers[i]->setScissor(
            0,
            scissor
        );

        uniqueCommandBuffers[i]->draw(
            3,
            1,
            0,
            0
        );

        // ----- finish up ----- //

        uniqueCommandBuffers[i]->endRenderPass();

        uniqueCommandBuffers[i]->end();

        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "  - Commands recorded successfully");
    }

    return uniqueCommandBuffers;
}

std::vector<vk::UniqueSemaphore> quartz::Application::createVulkanUniqueSemaphores(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const uint32_t maxNumFramesInFlight
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} max frames in flight", maxNumFramesInFlight);

    std::vector<vk::UniqueSemaphore> uniqueSemaphores(maxNumFramesInFlight);

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create {} vk::Semaphore(s)", maxNumFramesInFlight);
    for (uint32_t i = 0; i < maxNumFramesInFlight; ++i) {
        vk::SemaphoreCreateInfo semaphoreCreateInfo;

        uniqueSemaphores[i] = uniqueLogicalDevice->createSemaphoreUnique(semaphoreCreateInfo);

        if (!uniqueSemaphores[i]) {
            LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::Semaphore {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::Semaphore {}", i);
    }

    return uniqueSemaphores;
}

std::vector<vk::UniqueFence> quartz::Application::createVulkanUniqueFences(
    const vk::UniqueDevice& uniqueLogicalDevice,
    const uint32_t maxNumFramesInFlight
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "{} max frames in flight", maxNumFramesInFlight);

    std::vector<vk::UniqueFence> uniqueFences(maxNumFramesInFlight);

    LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Attempting to create {} vk::Fence(s)", maxNumFramesInFlight);
    for (uint32_t i = 0; i < maxNumFramesInFlight; ++i) {
        vk::FenceCreateInfo fenceCreateInfo(
            vk::FenceCreateFlagBits::eSignaled
        );

        uniqueFences[i] = uniqueLogicalDevice->createFenceUnique(fenceCreateInfo);

        if (!uniqueFences[i]) {
            LOG_CRITICAL(quartz::loggers::APPLICATION_INITIALIZATION, "Failed to create vk::Fence {}", i);
            throw std::runtime_error("");
        }
        LOG_TRACE(quartz::loggers::APPLICATION_INITIALIZATION, "Successfully created vk::Fence {}", i);
    }

    return uniqueFences;
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
    m_vulkanSwapchainExtent(quartz::Application::getBestSwapExtent(
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
        m_vulkanSwapchainExtent,
        m_vulkanSurfaceFormat,
        m_vulkanPresentMode
    )),
    m_vulkanSwapchainImages(m_vulkanUniqueLogicalDevice->getSwapchainImagesKHR(*m_vulkanUniqueSwapchain)),
    m_vulkanUniqueSwapchainImageViews(quartz::Application::createVulkanUniqueSwapchainImageViews(
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
        m_vulkanSwapchainExtent,
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
    )),
    m_vulkanUniqueFramebuffers(quartz::Application::createVulkanUniqueFramebuffers(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSwapchainExtent,
        m_vulkanUniqueSwapchainImageViews,
        m_vulkanUniqueRenderPass
    )),
    m_vulkanUniqueCommandPool(quartz::Application::createVulkanUniqueCommandPool(
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second,
        m_vulkanUniqueLogicalDevice
    )),
    m_maxNumFramesInFlight(2),
    m_vulkanUniqueCommandBuffers(quartz::Application::createVulkanUniqueCommandBuffers(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSwapchainExtent,
        m_vulkanSwapchainImages,
        m_vulkanUniqueRenderPass,
        m_vulkanUniqueGraphicsPipeline,
        m_vulkanUniqueFramebuffers,
        m_vulkanUniqueCommandPool
    )),
    m_vulkanUniqueImageAvailableSemaphores(quartz::Application::createVulkanUniqueSemaphores(
        m_vulkanUniqueLogicalDevice,
        m_maxNumFramesInFlight
    )),
    m_vulkanUniqueRenderFinishedSemaphores(quartz::Application::createVulkanUniqueSemaphores(
        m_vulkanUniqueLogicalDevice,
        m_maxNumFramesInFlight
    )),
    m_vulkanUniqueInFlightFences(quartz::Application::createVulkanUniqueFences(
        m_vulkanUniqueLogicalDevice,
        m_maxNumFramesInFlight
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

    uint32_t currentInFlightFrameIndex = 0;
    LOG_TRACEthis("Beginning main loop");
    while(!mp_window->shouldClose()) {
        glfwPollEvents();
        this->drawFrameToWindow(currentInFlightFrameIndex);
        currentInFlightFrameIndex = (currentInFlightFrameIndex + 1) % m_maxNumFramesInFlight;
    }

    m_vulkanUniqueLogicalDevice->waitIdle();
}

void quartz::Application::recreateSwapchain() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
    mp_window->clearWasResized();
    m_vulkanUniqueLogicalDevice->waitIdle();

    // ----- clean up ----- //

    for (vk::UniqueSemaphore& uniqueImageAvailableSemaphore : m_vulkanUniqueImageAvailableSemaphores) { uniqueImageAvailableSemaphore.reset(); }
    for (vk::UniqueSemaphore& uniqueRenderFinishedSemaphore : m_vulkanUniqueRenderFinishedSemaphores) { uniqueRenderFinishedSemaphore.reset(); }
    for (vk::UniqueFence& uniqueInFlightFence : m_vulkanUniqueInFlightFences) { uniqueInFlightFence.reset(); }
    for (vk::UniqueCommandBuffer& uniqueCommandBuffer : m_vulkanUniqueCommandBuffers) { uniqueCommandBuffer.reset(); }
    m_vulkanUniqueCommandPool.reset();
    for (vk::UniqueFramebuffer& uniqueFramebuffer : m_vulkanUniqueFramebuffers) { uniqueFramebuffer.reset(); }
    for (vk::UniqueImageView& uniqueImageView : m_vulkanUniqueSwapchainImageViews) { uniqueImageView.reset(); }
    m_vulkanUniqueGraphicsPipeline.reset();
    m_vulkanUniqueRenderPass.reset();
    m_vulkanUniquePipelineLayout.reset();
    m_vulkanUniqueSwapchain.reset();
    m_vulkanUniqueSurface.reset();

    // ----- recreate things ----- //

    m_vulkanUniqueSurface = quartz::Application::createVulkanSurface(
        mp_window->getGLFWwindowPtr(),
        m_vulkanUniqueInstance
    );
    m_vulkanSurfaceCapabilities = m_vulkanPhysicalDeviceAndQueueFamilyIndex.first.getSurfaceCapabilitiesKHR(*m_vulkanUniqueSurface);
    m_vulkanSwapchainExtent = quartz::Application::getBestSwapExtent(
        mp_window->getGLFWwindowPtr(),
        m_vulkanSurfaceCapabilities
    );
    m_vulkanSurfaceFormat = quartz::Application::getBestSurfaceFormat(
        m_vulkanUniqueSurface,
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.first
    );
    m_vulkanPresentMode = quartz::Application::getBestPresentMode(
        m_vulkanUniqueSurface,
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.first
    );
    m_vulkanUniqueSwapchain = quartz::Application::createVulkanUniqueSwapchain(
        m_vulkanUniqueSurface,
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second,
        m_vulkanUniqueLogicalDevice,
        m_vulkanSurfaceCapabilities,
        m_vulkanSwapchainExtent,
        m_vulkanSurfaceFormat,
        m_vulkanPresentMode
    );
    m_vulkanSwapchainImages = m_vulkanUniqueLogicalDevice->getSwapchainImagesKHR(*m_vulkanUniqueSwapchain);
    m_vulkanUniqueSwapchainImageViews = quartz::Application::createVulkanUniqueSwapchainImageViews(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSurfaceFormat,
        m_vulkanSwapchainImages
    );
    m_pipelineInformation = quartz::Application::getPipelineInformation(
        m_vulkanSwapchainExtent,
        m_vulkanUniqueVertexShaderModule,
        m_vulkanUniqueFragmentShaderModule
    );
    m_vulkanUniquePipelineLayout = quartz::Application::createVulkanUniquePipelineLayout(
        m_vulkanUniqueLogicalDevice
    );
    m_vulkanUniqueRenderPass = quartz::Application::createVulkanUniqueRenderPass(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSurfaceFormat.format
    );
    m_vulkanUniqueGraphicsPipeline = quartz::Application::createVulkanUniqueGraphicsPipeline(
        m_vulkanUniqueLogicalDevice,
        m_pipelineInformation,
        m_vulkanUniquePipelineLayout,
        m_vulkanUniqueRenderPass
    );
    m_vulkanUniqueFramebuffers = quartz::Application::createVulkanUniqueFramebuffers(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSwapchainExtent,
        m_vulkanUniqueSwapchainImageViews,
        m_vulkanUniqueRenderPass
    );
    m_vulkanUniqueCommandPool = quartz::Application::createVulkanUniqueCommandPool(
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second,
        m_vulkanUniqueLogicalDevice
    );
    m_vulkanUniqueCommandBuffers = quartz::Application::createVulkanUniqueCommandBuffers(
        m_vulkanUniqueLogicalDevice,
        m_vulkanSwapchainExtent,
        m_vulkanSwapchainImages,
        m_vulkanUniqueRenderPass,
        m_vulkanUniqueGraphicsPipeline,
        m_vulkanUniqueFramebuffers,
        m_vulkanUniqueCommandPool
    );
     m_vulkanUniqueImageAvailableSemaphores = quartz::Application::createVulkanUniqueSemaphores(
        m_vulkanUniqueLogicalDevice,
        m_maxNumFramesInFlight
    );
    m_vulkanUniqueRenderFinishedSemaphores = quartz::Application::createVulkanUniqueSemaphores(
        m_vulkanUniqueLogicalDevice,
        m_maxNumFramesInFlight
    );
    m_vulkanUniqueInFlightFences = quartz::Application::createVulkanUniqueFences(
        m_vulkanUniqueLogicalDevice,
        m_maxNumFramesInFlight
    );
}

void quartz::Application::drawFrameToWindow(const uint32_t currentInFlightFrameIndex) {
    // ----- 1. wait for previous frame to finish ----- //

    vk::Result waitForInFlightFenceResult = m_vulkanUniqueLogicalDevice->waitForFences(
        *(m_vulkanUniqueInFlightFences[currentInFlightFrameIndex]),
        true,
        std::numeric_limits<uint64_t>::max()
    );

    if (waitForInFlightFenceResult != vk::Result::eSuccess) {
        LOG_ERRORthis("Failed to wait for previous frame to finish: {}", static_cast<uint32_t>(waitForInFlightFenceResult));
    }

    // ----- 2. acquire an image from the swapchain ----- //

    uint32_t availableImageIndex;
    vk::Result acquireAvailableImageIndexResult = m_vulkanUniqueLogicalDevice->acquireNextImageKHR(
        *m_vulkanUniqueSwapchain,
        std::numeric_limits<uint64_t>::max(),
        *(m_vulkanUniqueImageAvailableSemaphores[currentInFlightFrameIndex]),
        {},
        &availableImageIndex
    );

    if (
        acquireAvailableImageIndexResult == vk::Result::eErrorOutOfDateKHR ||
        acquireAvailableImageIndexResult == vk::Result::eSuboptimalKHR ||
        mp_window->getWasResized()
    ) {
        LOG_INFOthis("Updating swapchain, image views, framebuffer due to image acquisition result ({}) or window resizing ({})", static_cast<uint32_t>(acquireAvailableImageIndexResult), mp_window->getWasResized());
        this->recreateSwapchain();
        return; // so we try again
    } else if (acquireAvailableImageIndexResult != vk::Result::eSuccess) {
        LOG_ERRORthis("Failed to retrieve available image index: {}", static_cast<uint32_t>(acquireAvailableImageIndexResult));
    }

    m_vulkanUniqueLogicalDevice->resetFences(*(m_vulkanUniqueInFlightFences[currentInFlightFrameIndex]));

    // ----- 3. record a command buffer which draws the scene onto the acquired image ----- //

    // m_vulkanUniqueCommandBuffers[availableImageIndex]->reset(); // if we are going to reset and re record the command buffer information every time

    // ----- 4. submit the recorded command buffer ----- //

    vk::PipelineStageFlags waitStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

    vk::SubmitInfo commandBufferSubmitInfo(
        *(m_vulkanUniqueImageAvailableSemaphores[currentInFlightFrameIndex]),
        waitStageMask,
        *(m_vulkanUniqueCommandBuffers[availableImageIndex]),
        *(m_vulkanUniqueRenderFinishedSemaphores[currentInFlightFrameIndex])
    );

    m_vulkanGraphicsQueue.submit(
        commandBufferSubmitInfo,
        *(m_vulkanUniqueInFlightFences[currentInFlightFrameIndex])
    );

    // ----- 5. present the swapchain image ----- //

    vk::PresentInfoKHR presentInfo(
        *(m_vulkanUniqueRenderFinishedSemaphores[currentInFlightFrameIndex]),
        *m_vulkanUniqueSwapchain,
        availableImageIndex,
        {}
    );

    vk::Result presentResult = m_vulkanPresentQueue.presentKHR(presentInfo);

    if (
        presentResult == vk::Result::eErrorOutOfDateKHR ||
        presentResult == vk::Result::eSuboptimalKHR ||
        mp_window->getWasResized()
    ) {
        LOG_INFOthis("Updating swapchain, image views, framebuffer due to presentation result ({}) or window resizing ({})", static_cast<uint32_t>(acquireAvailableImageIndexResult), mp_window->getWasResized());
        this->recreateSwapchain();
        return; // so we can try again
    } else if (presentResult != vk::Result::eSuccess) {
        LOG_ERRORthis("Failed to present to screen: {}", static_cast<uint32_t>(presentResult));
    }
}