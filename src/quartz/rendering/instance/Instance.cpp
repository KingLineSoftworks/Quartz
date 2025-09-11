#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "util/errors/RichException.hpp"

#include "quartz/rendering/instance/Instance.hpp"

VKAPI_ATTR VkBool32 VKAPI_CALL
quartz::rendering::Instance::vulkanDebugCallback(
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
#if defined QUARTZ_DEBUG
            // don't want to be reporting a bunch of performance issues when we are iterating and debugging,
            // only when we are doing testing or releasing so we can see where the profiling problems are
            return VK_FALSE;
#endif
            messageTypeString = "PERFORMANCE";
            break;
        default:
            messageTypeString = "UNKNOWN";
            break;
    }

    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            LOG_TRACE(VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            LOG_INFO(VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            LOG_WARNING(VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            LOG_ERROR(VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
        default:
            LOG_CRITICAL(VULKAN, "( {} ) {}", messageTypeString, p_callbackData->pMessage);
            break;
    }

    return VK_FALSE;
}

bool
quartz::rendering::Instance::initializeGLFW() {
    LOG_FUNCTION_SCOPE_TRACE(INSTANCE, "");

    if (!glfwInit()) {
        LOG_THROW(INSTANCE, util::IntException, 0, "Failed to initialize GLFW");
    }

    LOG_INFO(INSTANCE, "GLFW initialized");
    return true;
}

std::vector<const char*>
quartz::rendering::Instance::getEnabledValidationLayerNames(
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(INSTANCE, "enable validation layers = {}", validationLayersEnabled);

    if (!validationLayersEnabled) {
        LOG_TRACE(INSTANCE, "Validation layer support not requested. Not getting any validation layers");
        return {};
    }

    // ----- determine which validation layers are available ----- //

    std::vector<vk::LayerProperties> supportedLayerProperties = vk::enumerateInstanceLayerProperties();

    LOG_TRACE(INSTANCE, "{} extensions available", supportedLayerProperties.size());

    for (UNUSED const vk::LayerProperties& layerProperties : supportedLayerProperties) {
        LOG_TRACE(INSTANCE, "  - {} [ version {} ]", std::string(layerProperties.layerName), layerProperties.specVersion);
    }

    // ----- get the required validation layers ----- //

    const std::vector<const char*> requiredValidationLayerNames = {
        "VK_LAYER_KHRONOS_validation"
    };

    // ----- ensure all required validation layers are available ----- //

    LOG_TRACE(INSTANCE, "{} instance validation layers required", requiredValidationLayerNames.size());

    for (const std::string& requiredValidationLayerName : requiredValidationLayerNames) {
        LOG_TRACE(INSTANCE, "  - {}", requiredValidationLayerName);

        bool found = false;
        for (const vk::LayerProperties& layerProperties : supportedLayerProperties) {
            if (std::string(layerProperties.layerName) == std::string(requiredValidationLayerName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOG_THROW(INSTANCE, util::RichException<std::vector<const char*>>, requiredValidationLayerNames, "Required validation layer {} is not available", requiredValidationLayerName);
        }
    }

    return requiredValidationLayerNames;
}

std::vector<const char*>
quartz::rendering::Instance::getEnabledInstanceExtensionNames(
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(INSTANCE, "enable validation layers = {}", validationLayersEnabled);

    // ----- determine what instance extensions are available ----- //

    std::vector<vk::ExtensionProperties> availableInstanceExtensionProperties = vk::enumerateInstanceExtensionProperties();

    LOG_TRACE(INSTANCE, "{} instance extensions available", availableInstanceExtensionProperties.size());

    for (UNUSED const vk::ExtensionProperties& extensionProperties : availableInstanceExtensionProperties) {
        LOG_TRACE(INSTANCE, "  - {} [ version {} ]", std::string(extensionProperties.extensionName), extensionProperties.specVersion);
    }

    // ----- get the extensions required by glfw ----- //

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    LOG_TRACE(INSTANCE, "{} required instance extensions from glfw", glfwExtensionCount);

    std::vector<const char*> requiredInstanceExtensionNames(
        glfwExtensions,
        glfwExtensions + glfwExtensionCount
    );

    // ----- get extensions required by validation layers ----- //

    if (validationLayersEnabled) {
        LOG_TRACE(INSTANCE, "Requiring validation layer instance extension");
        requiredInstanceExtensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // ----- ensure that all of our required extensions are available ----- //

    LOG_TRACE(INSTANCE, "{} instance extensions required", requiredInstanceExtensionNames.size());

    for (const char* requiredInstanceExtensionName : requiredInstanceExtensionNames) {
        LOG_TRACE(INSTANCE, "  - {}", requiredInstanceExtensionName);

        bool found = false;
        for (const vk::ExtensionProperties& extensionProperties : availableInstanceExtensionProperties) {
            if (std::string(extensionProperties.extensionName) == std::string(requiredInstanceExtensionName)) {
                found = true;
                break;
            }
        }

        if (!found) {
            LOG_THROW(INSTANCE, util::RichException<std::vector<const char*>>, requiredInstanceExtensionNames, "Required instance extension {} is not available", requiredInstanceExtensionName);
        }
    }

    return requiredInstanceExtensionNames;
}

vk::UniqueInstance
quartz::rendering::Instance::createVulkanInstancePtr(
    const std::string &applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const std::vector<const char*>& enabledValidationLayerNames,
    const std::vector<const char*>& enabledExtensionNames
) {
    LOG_FUNCTION_SCOPE_TRACE(INSTANCE, "{} {}.{}.{}", applicationName, applicationMajorVersion, applicationMinorVersion, applicationPatchVersion);

    vk::ApplicationInfo applicationInfo(
        applicationName.c_str(),
        VK_MAKE_VERSION(
            applicationMajorVersion,
            applicationMinorVersion,
            applicationPatchVersion
        ),
        QUARTZ_NAME,
        VK_MAKE_VERSION(
            QUARTZ_MAJOR_VERSION,
            QUARTZ_MINOR_VERSION,
            QUARTZ_PATCH_VERSION
        ),
        VK_API_VERSION_1_2
    );

    // ----- creating the instance ----- //

    vk::InstanceCreateInfo instanceCreateInfo(
        {},
        &applicationInfo,
        enabledValidationLayerNames,
        enabledExtensionNames
    );

    vk::UniqueInstance p_instance = vk::createInstanceUnique(instanceCreateInfo);

    if (!p_instance) {
        LOG_THROW(INSTANCE, util::RichException<vk::InstanceCreateInfo>, instanceCreateInfo, "Failed to create the vk::Instance");
    }

    return p_instance;
}

vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic>
quartz::rendering::Instance::createVulkanDebugUtilsMessengerPtr(
    const vk::UniqueInstance& p_instance,
    const vk::DispatchLoaderDynamic& dispatchLoaderDynamic,
    const bool validationLayersEnabled
) {
    LOG_FUNCTION_SCOPE_TRACE(INSTANCE, "enable validation layers = {}", validationLayersEnabled);

    if (!validationLayersEnabled) {
        LOG_TRACE(INSTANCE, "Default constructing DebugUtilsMessengerEXT because validation layers aren't enabled");
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
        quartz::rendering::Instance::vulkanDebugCallback,
        nullptr
    );

    vk::UniqueHandle<
        vk::DebugUtilsMessengerEXT,
        vk::DispatchLoaderDynamic
    > p_debugUtilsMessenger = p_instance->createDebugUtilsMessengerEXTUnique(
        debugMessengerCreateInfo,
        nullptr,
        dispatchLoaderDynamic
    );

    if (!p_debugUtilsMessenger) {
        LOG_THROW(INSTANCE, util::RichException<vk::DebugUtilsMessengerCreateInfoEXT>, debugMessengerCreateInfo, "Failed to create vk::DebugUtilsMessengerEXT");
    }

    return p_debugUtilsMessenger;
}

quartz::rendering::Instance::Instance(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const bool validationLayersEnabled
) :
    m_glfwInitialized(quartz::rendering::Instance::initializeGLFW()),
    m_validationLayerNames(
        quartz::rendering::Instance::getEnabledValidationLayerNames(
            validationLayersEnabled
        )
    ),
    m_instanceExtensionNames(
        quartz::rendering::Instance::getEnabledInstanceExtensionNames(
            validationLayersEnabled
        )
    ),
    mp_vulkanInstance(
        quartz::rendering::Instance::createVulkanInstancePtr(
        applicationName,
        applicationMajorVersion,
        applicationMinorVersion,
        applicationPatchVersion,
        m_validationLayerNames,
        m_instanceExtensionNames
    )),
    m_vulkanDispatchLoaderDynamic(
        *mp_vulkanInstance,
        vkGetInstanceProcAddr
    ),
    mp_vulkanDebugMessenger(
        quartz::rendering::Instance::createVulkanDebugUtilsMessengerPtr(
            mp_vulkanInstance,
            m_vulkanDispatchLoaderDynamic,
            validationLayersEnabled
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::rendering::Instance::~Instance() {
    LOG_FUNCTION_CALL_TRACEthis("");

    LOG_TRACEthis("Terminating GLFW");
    glfwTerminate();
    LOG_INFOthis("Successfully terminated GLFW");
}
