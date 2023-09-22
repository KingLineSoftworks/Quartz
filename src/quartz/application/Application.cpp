#include <algorithm>
#include <functional>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "util/logger/Logger.hpp"

#include "quartz/application/Application.hpp"

namespace vk {

// Manually load the CreateDebugUtilsMessenger functions via proxy function
vk::ResultValueType<vk::DebugUtilsMessengerEXT>::type createDebugUtilsMessengerEXT(
    const vk::Instance& instance,
    const vk::DebugUtilsMessengerCreateInfoEXT& debugMessengerCreateInfo,
    vk::Optional<const vk::AllocationCallbacks> allocator
) {
    const char* desiredFunctionName = "vkCreateDebugUtilsMessengerEXT";

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, desiredFunctionName);

    if (vkCreateDebugUtilsMessengerEXT == nullptr) {
        LOG_CRITICAL(quartz::loggers::VALIDATION_LAYER, "Could not find extension {}", desiredFunctionName);
        // return VK_ERROR_EXTENSION_NOT_PRESENT; // if we were returning a VkResult, but we aren't because i'm a fucking god
        throw std::runtime_error("");
    }

    vk::DebugUtilsMessengerEXT debugMessenger;

    vk::Result result = static_cast<vk::Result>(
        vkCreateDebugUtilsMessengerEXT(
            instance,
            reinterpret_cast<const VkDebugUtilsMessengerCreateInfoEXT*>(&debugMessengerCreateInfo),
            reinterpret_cast<const VkAllocationCallbacks*>(
                static_cast<const vk::AllocationCallbacks*>(allocator)
            ),
            reinterpret_cast<VkDebugUtilsMessengerEXT*>(&debugMessenger)
        )
    );

    return vk::createResultValue(result, debugMessenger, "vk::createDebugUtilsMessengerEXT");
}

// Manually load the DestroyDebugUtilsMessenger functions via proxy function
void destroyDebugUtilsMessengerEXT(
    const vk::Instance& instance,
    vk::DebugUtilsMessengerEXT& debugMessenger,
    vk::Optional<const vk::AllocationCallbacks> allocator
) {
    const char* desiredFunctionName = "vkDestroyDebugUtilsMessengerEXT";

    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, desiredFunctionName);

    if (vkDestroyDebugUtilsMessengerEXT == nullptr) {
        LOG_CRITICAL(quartz::loggers::VALIDATION_LAYER, "Could not find extension {}", desiredFunctionName);
        throw std::runtime_error("");
    }

    vkDestroyDebugUtilsMessengerEXT(
        instance,
        reinterpret_cast<VkDebugUtilsMessengerEXT>(&debugMessenger),
        reinterpret_cast<const VkAllocationCallbacks*>(
            static_cast<const vk::AllocationCallbacks*>(allocator)
        )
    );
}

} // namespace vk

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
            LOG_TRACE(quartz::loggers::VALIDATION_LAYER, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(quartz::loggers::VALIDATION_LAYER, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARNING(quartz::loggers::VALIDATION_LAYER, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(quartz::loggers::VALIDATION_LAYER, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        default:
            LOG_CRITICAL(quartz::loggers::VALIDATION_LAYER, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
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

vk::UniqueInstance quartz::Application::createUniqueVulkanInstance(
    const std::string &applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "{} {}.{}.{}", applicationName, applicationMajorVersion, applicationMinorVersion, applicationPatchVersion);

    vk::ApplicationInfo applicationInfo(
        applicationName.c_str(),
        VK_MAKE_VERSION(applicationMajorVersion, applicationMinorVersion, applicationPatchVersion),
        QUARTZ_NAME,
        VK_MAKE_VERSION(QUARTZ_MAJOR_VERSION, QUARTZ_MINOR_VERSION, QUARTZ_PATCH_VERSION),
        VK_API_VERSION_1_2
    );

    vk::InstanceCreateFlags instanceCreateFlags;

    // ----- validation layers ----- //

    std::vector<const char*> enabledValidationLayerNames = quartz::Application::getEnabledValidationLayerNames(validationLayersEnabled);

    // ----- extensions ----- //

    std::vector<const char*> enabledExtensionNames = quartz::Application::getEnabledInstanceExtensionNames(validationLayersEnabled);

    // ----- creating the instance ----- //

    vk::InstanceCreateInfo instanceCreateInfo(
        instanceCreateFlags,
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

vk::DebugUtilsMessengerEXT quartz::Application::createVulkanDebugUtilsMessenger(
    const vk::UniqueInstance& uniqueInstance,
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "enable validation layers = {}", validationLayersEnabled);

    if (!validationLayersEnabled) {
        LOG_TRACE(quartz::loggers::APPLICATION, "Default constructing DebugUtilsMessengerEXT because validation layers aren't enabled");
        return {};
    }

    vk::DebugUtilsMessengerCreateFlagsEXT debugMessengerCreateFlags;

    vk::DebugUtilsMessageSeverityFlagsEXT debugMessengerSeverityFlags(
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );

    vk::DebugUtilsMessageTypeFlagsEXT debugMessengerTypeFlags(
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
    vk::DebugUtilsMessengerEXT debugMessenger = vk::createDebugUtilsMessengerEXT(
        *uniqueInstance,
        debugMessengerCreateInfo,
        nullptr
    );

    if (!debugMessenger) {
        LOG_CRITICAL(quartz::loggers::APPLICATION, "Failed to create the vk::DebugUtilsMessengerEXT");
        throw std::runtime_error("");
    }
    LOG_TRACE(quartz::loggers::APPLICATION, "Successfully created the vk::DebugUtilsMessengerEXT");

    return debugMessenger;
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
    mp_window(std::make_unique<quartz::rendering::Window>(m_applicationName, windowWidthPixels, windowHeightPixels)),
    m_uniqueVulkanInstance(quartz::Application::createUniqueVulkanInstance(
        m_applicationName,
        m_majorVersion,
        m_minorVersion,
        m_patchVersion,
        validationLayersEnabled
    )),
    m_vulkanDebugMessenger(quartz::Application::createVulkanDebugUtilsMessenger(m_uniqueVulkanInstance, validationLayersEnabled))
{
    LOG_FUNCTION_CALL_TRACEthis("{} version {}.{}.{}", m_applicationName, m_majorVersion, m_minorVersion, m_patchVersion);
}

quartz::Application::~Application() {
    LOG_FUNCTION_CALL_TRACEthis("");
}

void quartz::Application::run() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACE(quartz::loggers::APPLICATION, "Beginning main loop");
    while(!mp_window->shouldClose()) {
        glfwPollEvents();
    }

    // Destroy the debug messenger
    LOG_TRACE(quartz::loggers::APPLICATION, "Destroying debug messenger");
    vk::destroyDebugUtilsMessengerEXT(*m_uniqueVulkanInstance, m_vulkanDebugMessenger, nullptr);
}