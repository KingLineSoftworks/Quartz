#include <vulkan/vulkan.hpp>

#include "GLFW/glfw3.h"

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/rendering/context/Device.hpp"
#include "quartz/rendering/context/Instance.hpp"
#include "quartz/rendering/context/Window.hpp"

void
quartz::rendering::Window::glfwFramebufferSizeCallback(
    GLFWwindow* p_glfwWindow,
    int updatedWindowWidthPixels,
    int updatedWindowHeightPixels
) {
    // Get a pointer to the quartz::rendering::Window from the
    // GLFWwindow* so we may update its values
    quartz::rendering::Window* p_quartzWindow =
        reinterpret_cast<quartz::rendering::Window*>(
            glfwGetWindowUserPointer(p_glfwWindow)
        );

    if (!p_quartzWindow) {
        LOG_CRITICAL(
            WINDOW,
            "Retrieved invalid window pointer from glfw window user pointer"
        );
    }

    p_quartzWindow->m_widthPixels = updatedWindowWidthPixels;
    p_quartzWindow->m_heightPixels = updatedWindowHeightPixels;
    p_quartzWindow->m_wasResized = true;
}

std::shared_ptr<GLFWwindow>
quartz::rendering::Window::createGLFWwindowPtr(
    const std::string &name,
    const uint32_t widthPixels,
    const uint32_t heightPixels,
    const void* p_windowUser
) {
    LOG_FUNCTION_SCOPE_TRACE(
        WINDOW, "{} ({}x{}) , user = {}",
        name, widthPixels, heightPixels, p_windowUser
    );

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
#ifdef ON_MAC
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif

    std::shared_ptr<GLFWwindow> p_glfwWindow(
        glfwCreateWindow(
            widthPixels,
            heightPixels,
            name.c_str(),
            nullptr,
            nullptr
        ),
        [](GLFWwindow* p_window) { glfwDestroyWindow(p_window); }
    );
    LOG_TRACE(
        WINDOW, "Created GLFW window pointer at {}",
        static_cast<void*>(p_glfwWindow.get())
    );
    if (!p_glfwWindow) {
        LOG_CRITICAL(
            WINDOW, "Failed to create GLFW window pointer. Terminating"
        );
        glfwTerminate();
        throw std::runtime_error("");
    }

    LOG_TRACE(WINDOW, "Setting GLFW window user pointer to this");
    glfwSetWindowUserPointer(
        p_glfwWindow.get(),
        const_cast<void*>(p_windowUser)
    );

    LOG_TRACE(WINDOW, "Setting GLFW framebuffer size callback");
    glfwSetFramebufferSizeCallback(
        p_glfwWindow.get(),
        quartz::rendering::Window::glfwFramebufferSizeCallback
    );

    return p_glfwWindow;
}

vk::UniqueSurfaceKHR
quartz::rendering::Window::createVulkanSurfaceUniquePtr(
    const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
    const vk::UniqueInstance& uniqueInstance
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

#ifndef ON_MAC
    LOG_CRITICAL(WINDOW, "No support for non-mac platforms currently. Unable to create vk::SurfaceKHR");
    throw std::runtime_error("");
#endif

    VkSurfaceKHR rawVulkanSurface;
    LOG_TRACE(WINDOW, "Attempting to create VkSurfaceKHR");
    VkResult createResult = glfwCreateWindowSurface(
        *uniqueInstance,
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        nullptr,
        &rawVulkanSurface
    );
    if (createResult != VK_SUCCESS) {
        LOG_CRITICAL(
            WINDOW, "Failed to create VkSurfaceKHR ( {} )",
            static_cast<int64_t>(createResult)
        );
        throw std::runtime_error("");
    }
    LOG_TRACE(WINDOW, "Successfully created VkSurfaceKHR");

    vk::UniqueSurfaceKHR p_surface(
        rawVulkanSurface,
        *uniqueInstance
    );
    if (!p_surface) {
        LOG_CRITICAL(
            WINDOW, "Failed to create vk::SurfaceKHR from VkSurfaceKHR"
        );
        throw std::runtime_error("");
    }
    LOG_TRACE(WINDOW, "Successfully created vk::SurfaceKHR from VkSurfaceKHR");

    return p_surface;
}

vk::SurfaceFormatKHR
quartz::rendering::Window::getBestSurfaceFormat(
    const vk::UniqueSurfaceKHR& p_surface,
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

    LOG_TRACE(WINDOW, "Getting surface formats from physical device");
    std::vector<vk::SurfaceFormatKHR> surfaceFormats =
        physicalDevice.getSurfaceFormatsKHR(*p_surface);
    if (surfaceFormats.empty()) {
        LOG_CRITICAL(
            WINDOW, "No surface formats available for chosen physical device"
        );
        throw std::runtime_error("");
    }

    LOG_TRACE(WINDOW, "Choosing suitable surface format");
    for (const vk::SurfaceFormatKHR& surfaceFormat : surfaceFormats) {
        if (
            surfaceFormat.format == vk::Format::eB8G8R8A8Srgb &&
            surfaceFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear
        ) {
            LOG_TRACE(WINDOW, "  - found suitable surface format");
            return surfaceFormat;
        }
    }

    LOG_CRITICAL(WINDOW, "No suitable surface formats found");
    throw std::runtime_error("");
}

vk::PresentModeKHR
quartz::rendering::Window::getBestPresentMode(
    const vk::UniqueSurfaceKHR& p_surface,
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

    LOG_TRACE(WINDOW, "Getting present modes from physical device");
    std::vector<vk::PresentModeKHR> presentModes =
        physicalDevice.getSurfacePresentModesKHR(*p_surface);
    if (presentModes.empty()) {
        LOG_CRITICAL(
            WINDOW, "No present modes available for chosen physical device"
        );
        throw std::runtime_error("");
    }

    vk::PresentModeKHR bestPresentMode = vk::PresentModeKHR::eFifo;
    LOG_TRACE(WINDOW, "Choosing best present mode");
    for (const vk::PresentModeKHR& presentMode : presentModes) {
        if (presentMode == vk::PresentModeKHR::eMailbox) {
            bestPresentMode = presentMode;
            break;
        }
    }

    if (bestPresentMode == vk::PresentModeKHR::eMailbox) {
        LOG_TRACE(WINDOW, "Using mailbox present mode");
    } else {
        LOG_TRACE(WINDOW, "Using fifo present mode");
    }

    return bestPresentMode;
}

vk::Extent2D
quartz::rendering::Window::getBestVulkanExtent(
    const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
    const vk::SurfaceCapabilitiesKHR& surfaceCapabilities
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

    LOG_TRACE(WINDOW, "Choosing best swap extent");
    if (
        surfaceCapabilities.currentExtent.width !=
        std::numeric_limits<uint32_t>::max()
    ) {
        LOG_TRACE(
            WINDOW, "Using capabilities's current swap extent of size {} x {}",
            surfaceCapabilities.currentExtent.width,
            surfaceCapabilities.currentExtent.height
        );
        return surfaceCapabilities.currentExtent;
    }

    LOG_TRACE(
        WINDOW,
        "Creating our own swap extent (current extent of surface capabilities "
        "has width of uint32_t max value, so we must handle manually)"
    );
    int32_t widthPixels;
    int32_t heightPixels;
    glfwGetFramebufferSize(
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        &widthPixels,
        &heightPixels
    );
    LOG_TRACE(
        WINDOW, "Got GLFW framebuffer size of W {} pix x H {} pix",
        widthPixels, heightPixels
    );

    uint32_t adjustedWidthPixels = std::clamp(
        static_cast<uint32_t>(widthPixels),
        surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width
    );
    LOG_TRACE(
        WINDOW, "Adjusted width of {} pixels (after clamp between {} and {})",
        adjustedWidthPixels,
        surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width
    );

    uint32_t adjustedHeightPixels = std::clamp(
        static_cast<uint32_t>(heightPixels),
        surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height
    );
    LOG_TRACE(
        WINDOW, "Adjusted height of {} pixels (after clamp between {} and {})",
        adjustedHeightPixels,
        surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height
    );

    LOG_TRACE(
        WINDOW, "Creating custom extent of W {} pix x H {} pix",
        adjustedWidthPixels, adjustedHeightPixels
    );
    vk::Extent2D customExtent(
        static_cast<uint32_t>(adjustedWidthPixels),
        static_cast<uint32_t>(adjustedHeightPixels)
    );

    return customExtent;
}

quartz::rendering::Window::Window(
    const std::string& name,
    const uint32_t windowWidthPixels,
    const uint32_t windowHeightPixels,
    const quartz::rendering::Instance& renderingInstance,
    const quartz::rendering::Device& renderingDevice
) :
    m_name(name),
    m_widthPixels(windowWidthPixels),
    m_heightPixels(windowHeightPixels),
    m_wasResized(false),
    mp_glfwWindow(
        quartz::rendering::Window::createGLFWwindowPtr(
            name,
            windowWidthPixels,
            windowHeightPixels,
            this
        )
    ),
    mp_vulkanSurface(
        quartz::rendering::Window::createVulkanSurfaceUniquePtr(
            mp_glfwWindow,
            renderingInstance.getVulkanInstanceUniquePtr()
        )
    ),
    m_vulkanSurfaceCapabilities(
        renderingDevice.getVulkanPhysicalDevice().getSurfaceCapabilitiesKHR(
            *mp_vulkanSurface
        )
    ),
    m_vulkanSurfaceFormat(
        quartz::rendering::Window::getBestSurfaceFormat(
            mp_vulkanSurface,
            renderingDevice.getVulkanPhysicalDevice()
        )
    ),
    m_vulkanPresentMode(
        quartz::rendering::Window::getBestPresentMode(
            mp_vulkanSurface,
            renderingDevice.getVulkanPhysicalDevice()
        )
    ),
    m_vulkanExtent(
        quartz::rendering::Window::getBestVulkanExtent(
            mp_glfwWindow,
            m_vulkanSurfaceCapabilities
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis(
        "{} ( {} x {} )", m_name, m_widthPixels, m_heightPixels
    );
}

quartz::rendering::Window::~Window() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis(
        "Destroying GLFW window at {}", static_cast<void*>(mp_glfwWindow.get())
    );
    glfwDestroyWindow(mp_glfwWindow.get());

    LOG_TRACEthis("Terminating GLFW");
    glfwTerminate();
}

void
quartz::rendering::Window::reset() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanSurface.reset();
}

void
quartz::rendering::Window::recreate(
    const quartz::rendering::Instance& renderingInstance,
    const quartz::rendering::Device& renderingDevice
) {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    mp_vulkanSurface = quartz::rendering::Window::createVulkanSurfaceUniquePtr(
        mp_glfwWindow,
        renderingInstance.getVulkanInstanceUniquePtr()
    );

    m_vulkanSurfaceCapabilities =
        renderingDevice.getVulkanPhysicalDevice().getSurfaceCapabilitiesKHR(
            *mp_vulkanSurface
        );

    m_vulkanSurfaceFormat = quartz::rendering::Window::getBestSurfaceFormat(
        mp_vulkanSurface,
        renderingDevice.getVulkanPhysicalDevice()
    );

    m_vulkanPresentMode = quartz::rendering::Window::getBestPresentMode(
        mp_vulkanSurface,
        renderingDevice.getVulkanPhysicalDevice()
    );

    m_vulkanExtent = quartz::rendering::Window::getBestVulkanExtent(
        mp_glfwWindow,
        m_vulkanSurfaceCapabilities
    );

    LOG_TRACEthis("Clearing the \"was resized\" flag");
    m_wasResized = false;
}

bool quartz::rendering::Window::shouldClose() const {
    return static_cast<bool>(
        glfwWindowShouldClose(mp_glfwWindow.get())
    );
}