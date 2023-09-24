#include <functional>
#include <optional>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "util/platform.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/application/Application.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL quartz::Application::vulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
    void* p_userData
) {
    UNUSED(p_userData);

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

    std::vector<const char*> requiredPhysicalDeviceExtensionNames;
    std::vector<vk::ExtensionProperties> availablePhysicalDeviceExtensionProperties = physicalDevice.enumerateDeviceExtensionProperties();
    LOG_TRACE(quartz::loggers::APPLICATION, "{} physical device extensions available", availablePhysicalDeviceExtensionProperties.size());
    for (const vk::ExtensionProperties& extensionProperties : availablePhysicalDeviceExtensionProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {} [ version {} ]", extensionProperties.extensionName, extensionProperties.specVersion);

        if (extensionProperties.extensionName == std::string("VK_KHR_portability_subset")) {
            LOG_TRACE(quartz::loggers::APPLICATION, "    - portability subset found");
            requiredPhysicalDeviceExtensionNames.push_back("VK_KHR_portability_subset");
        }
    }

    return requiredPhysicalDeviceExtensionNames;
}

vk::UniqueDevice quartz::Application::createVulkanUniqueLogicalDevice(
    const vk::PhysicalDevice& physicalDevice,
    const uint32_t queueFamilyIndex,
    const std::vector<const char*>& validationLayerNames,
    const std::vector<const char*>& physicalDeviceExtensionNames
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "");

    std::vector<float> deviceQueuePriorities = {1.0f};
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos = {
        vk::DeviceQueueCreateInfo(
            {},
            queueFamilyIndex,
            deviceQueuePriorities
        )
    };

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
    if (glfwCreateWindowSurface(
        *uniqueInstance,
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        nullptr,
        &rawVulkanSurface
    ) != VK_SUCCESS) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create VkSurfaceKHR");
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
    mp_window(std::make_shared<quartz::rendering::Window>(
        m_applicationName,
        windowWidthPixels,
        windowHeightPixels
    )),
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
        m_vulkanPhysicalDeviceAndQueueFamilyIndex.second.graphicsFamilyIndex,
        m_validationLayerNames,
        m_physicalDeviceExtensionNames
    ))
{
    LOG_FUNCTION_CALL_TRACEthis("{} version {}.{}.{}", m_applicationName, m_majorVersion, m_minorVersion, m_patchVersion);
}

quartz::Application::~Application() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application::run() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    // ----- get the queue ----- //

    vk::Queue graphicsQueue = m_vulkanUniqueLogicalDevice->getQueue(m_vulkanPhysicalDeviceAndQueueFamilyIndex.second.graphicsFamilyIndex, 0);
    UNUSED(graphicsQueue);

    // ----- drop that ass at me from an egregarious angle ----- //

    LOG_TRACE(quartz::loggers::APPLICATION, "Beginning main loop");
    while(!mp_window->shouldClose()) {
        glfwPollEvents();
    }
}