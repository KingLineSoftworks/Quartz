#include <algorithm>
#include <stdexcept>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <GLFW/glfw3.h>

#include "util/logger/Logger.hpp"

#include "quartz/application/Application.hpp"

vk::Instance quartz::Application::createVulkanInstance(
    const std::string &applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion
) {
    LOG_FUNCTION_SCOPE_TRACE(quartz::loggers::APPLICATION, "{} {}.{}.{}", applicationName, applicationMajorVersion, applicationMinorVersion, applicationPatchVersion);

    vk::ApplicationInfo applicationInfo(
        applicationName.c_str(),
        (applicationMajorVersion * 1000000) + (applicationMinorVersion * 1000) + (applicationPatchVersion),
        QUARTZ_NAME,
        (QUARTZ_MAJOR_VERSION * 1000000) + (QUARTZ_MINOR_VERSION * 1000) + (QUARTZ_PATCH_VERSION),
        VK_API_VERSION_1_2
    );

    vk::InstanceCreateFlags instanceCreateFlags;

    std::vector<const char*> enabledLayerNames;
    std::vector<const char*> enabledExtensionNames;

    std::vector<vk::ExtensionProperties> instanceExtensionProperties = vk::enumerateInstanceExtensionProperties();
    LOG_TRACE(quartz::loggers::APPLICATION, "{} extensions available:", instanceExtensionProperties.size());
    for (const vk::ExtensionProperties& instanceExtensionProperty : instanceExtensionProperties) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {} {}", instanceExtensionProperty.extensionName, instanceExtensionProperty.specVersion);
        enabledExtensionNames.push_back(instanceExtensionProperty.extensionName);
    }

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    LOG_TRACE(quartz::loggers::APPLICATION, "{} glfw extensions required", glfwExtensionCount);
    for (uint32_t i = 0; i < glfwExtensionCount; ++i) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {}", glfwExtensions[i]);
        if (!std::count(enabledExtensionNames.begin(), enabledExtensionNames.end(), std::string(glfwExtensions[i]))) {
            LOG_CRITICAL(quartz::loggers::APPLICATION, "    - required extension {} is not in available extensions", glfwExtensions[i]);
            throw std::runtime_error("");
        } else {
            LOG_TRACE(quartz::loggers::APPLICATION, "    - already in enabled extensions, not adding");
        }
    }

    LOG_TRACE(quartz::loggers::APPLICATION, "{} extensions enabled", enabledExtensionNames.size());
    for (uint32_t i = 0; i < enabledExtensionNames.size(); ++i) {
        LOG_TRACE(quartz::loggers::APPLICATION, "  - {}", enabledExtensionNames[i]);
    }

    vk::InstanceCreateInfo instanceCreateInfo(
        instanceCreateFlags,
        &applicationInfo,
        enabledLayerNames,
        enabledExtensionNames
    );

    LOG_TRACE(quartz::loggers::APPLICATION, "Attempting to create the vk::Instance");
    vk::Instance instance = vk::createInstance(instanceCreateInfo);

    return instance;
}

quartz::Application::Application(
    const std::string& applicationName,
    const uint32_t applicationMajorVersion,
    const uint32_t applicationMinorVersion,
    const uint32_t applicationPatchVersion,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels
) :
    m_applicationName(applicationName),
    m_majorVersion(applicationMajorVersion),
    m_minorVersion(applicationMinorVersion),
    m_patchVersion(applicationPatchVersion),
    mp_window(std::make_unique<quartz::rendering::Window>(m_applicationName, windowWidthPixels, windowHeightPixels)),
    m_vulkanInstance(quartz::Application::createVulkanInstance(m_applicationName, m_majorVersion, m_minorVersion, m_patchVersion))
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

    // Destroy the instance
}