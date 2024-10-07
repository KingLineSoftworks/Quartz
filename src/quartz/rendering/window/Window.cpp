#include <vulkan/vulkan.hpp> // need vulkan first for glfwCreateWindowSurface

#include <GLFW/glfw3.h>

#include "util/macros.hpp"
#include "util/platform.hpp"
#include "util/logger/Logger.hpp"

#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/instance/Instance.hpp"
#include "quartz/rendering/window/Window.hpp"

void
quartz::rendering::Window::glfwFramebufferSizeCallback(
    GLFWwindow* p_glfwWindow,
    int updatedWindowWidthPixels,
    int updatedWindowHeightPixels
) {
    // Get a pointer to the quartz::rendering::Window from the
    // GLFWwindow* so we may update its values
    quartz::rendering::Window* p_quartzWindow = reinterpret_cast<quartz::rendering::Window*>(
        glfwGetWindowUserPointer(p_glfwWindow)
    );

    if (!p_quartzWindow) {
        LOG_CRITICAL(WINDOW, "Retrieved invalid window pointer from glfw window user pointer");
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
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "{} ({}x{}) , user = {}", name, widthPixels, heightPixels, p_windowUser);

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
        [] (GLFWwindow* p_window) {
            glfwDestroyWindow(p_window);
        }
    );
    LOG_TRACE(WINDOW, "Created GLFW window pointer at {}", static_cast<void*>(p_glfwWindow.get()));
    if (!p_glfwWindow) {
        LOG_CRITICAL(WINDOW, "Terminating GLFW");
        glfwTerminate();
        LOG_THROW(WINDOW, util::VulkanCreationFailedError, "Failed to create GLFW window pointer");
    }

    LOG_TRACE(WINDOW, "Setting GLFW window user pointer");
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
quartz::rendering::Window::createVulkanSurfacePtr(
    const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
    const vk::UniqueInstance& uniqueInstance
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

#if !defined ON_MAC && !defined ON_LINUX
    LOG_THROW(WINDOW, util::VulkanFeatureNotSupportedError, "No support for non mac or linux platforms currently. Unable to create vk::SurfaceKHR");
#endif

    VkSurfaceKHR rawVulkanSurface;
    VkResult createResult = glfwCreateWindowSurface(
        *uniqueInstance,
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        nullptr,
        &rawVulkanSurface
    );
    if (createResult != VK_SUCCESS) {
        LOG_THROW(WINDOW, util::VulkanCreationFailedError, "Failed to create VkSurfaceKHR ( {} )", static_cast<int64_t>(createResult));
    }

    vk::UniqueSurfaceKHR p_surface(
        rawVulkanSurface,
        *uniqueInstance
    );
    if (!p_surface) {
        LOG_THROW(WINDOW, util::VulkanCreationFailedError, "Failed to create vk::SurfaceKHR from VkSurfaceKHR");
    }

    return p_surface;
}

vk::SurfaceFormatKHR
quartz::rendering::Window::getBestSurfaceFormat(
    const vk::UniqueSurfaceKHR& p_surface,
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

    std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(*p_surface);

    if (surfaceFormats.empty()) {
        LOG_THROW(WINDOW, util::VulkanFeatureNotSupportedError, "No surface formats available for chosen physical device");
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

    LOG_THROW(WINDOW, util::VulkanFeatureNotSupportedError, "No suitable surface formats found");
}

vk::PresentModeKHR
quartz::rendering::Window::getBestPresentMode(
    const vk::UniqueSurfaceKHR& p_surface,
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

    std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(*p_surface);

    if (presentModes.empty()) {
        LOG_THROW(WINDOW, util::VulkanFeatureNotSupportedError, "No present modes available for chosen physical device");
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
    if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        LOG_TRACE(WINDOW, "Using capabilities's current swap extent of size {} x {}", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);
        return surfaceCapabilities.currentExtent;
    }

    LOG_TRACE(WINDOW, "Creating our own swap extent (current extent of surface capabilities has width of uint32_t max value, so we must handle manually)");

    int32_t widthPixels;
    int32_t heightPixels;
    glfwGetFramebufferSize(
        const_cast<GLFWwindow*>(p_GLFWwindow.get()),
        &widthPixels,
        &heightPixels
    );
    LOG_TRACE(WINDOW, "Got GLFW framebuffer size of W {} pix x H {} pix", widthPixels, heightPixels);

    uint32_t adjustedWidthPixels = std::clamp(
        static_cast<uint32_t>(widthPixels),
        surfaceCapabilities.minImageExtent.width,
        surfaceCapabilities.maxImageExtent.width
    );
    LOG_TRACE(WINDOW, "Adjusted width of {} pixels (after clamp between {} and {})", adjustedWidthPixels, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width);

    uint32_t adjustedHeightPixels = std::clamp(
        static_cast<uint32_t>(heightPixels),
        surfaceCapabilities.minImageExtent.height,
        surfaceCapabilities.maxImageExtent.height
    );
    LOG_TRACE(WINDOW, "Adjusted height of {} pixels (after clamp between {} and {})", adjustedHeightPixels, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

    LOG_TRACE(WINDOW, "Creating custom extent of W {} pix x H {} pix", adjustedWidthPixels, adjustedHeightPixels);
    vk::Extent2D customExtent(
        static_cast<uint32_t>(adjustedWidthPixels),
        static_cast<uint32_t>(adjustedHeightPixels)
    );

    return customExtent;
}

vk::Format
quartz::rendering::Window::getBestVulkanDepthBufferFormat(
    const vk::PhysicalDevice& physicalDevice
) {
    LOG_FUNCTION_SCOPE_TRACE(WINDOW, "");

    std::vector<vk::Format> formatCandidates = {
        vk::Format::eD32Sfloat,
        vk::Format::eD32SfloatS8Uint,
        vk::Format::eD24UnormS8Uint
    };

    vk::ImageTiling tiling = vk::ImageTiling::eOptimal;

    vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

    for (const vk::Format& format : formatCandidates) {
        vk::FormatProperties properties = physicalDevice.getFormatProperties(format);

        if (
            tiling == vk::ImageTiling::eLinear &&
            (properties.linearTilingFeatures & features) == features
        ) {
            LOG_TRACE(WINDOW, "Using linear image tiling");
            return format;
        } else if (
            tiling == vk::ImageTiling::eOptimal &&
            (properties.optimalTilingFeatures & features) == features
        ) {
            LOG_TRACE(WINDOW, "Using optimal image tiling");
            return format;
        }
    }

    LOG_THROW(WINDOW, util::VulkanFeatureNotSupportedError, "Failed to find supported depth buffer image format");
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
        quartz::rendering::Window::createVulkanSurfacePtr(
            mp_glfwWindow,
            renderingInstance.getVulkanInstancePtr()
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
    ),
    m_vulkanDepthBufferFormat(
        quartz::rendering::Window::getBestVulkanDepthBufferFormat(
            renderingDevice.getVulkanPhysicalDevice()
        )
    )
{
    LOG_FUNCTION_CALL_TRACEthis("{} ( {} x {} )", m_name, m_widthPixels, m_heightPixels);
}

quartz::rendering::Window::~Window() {
    LOG_FUNCTION_SCOPE_TRACEthis("");

    LOG_TRACEthis("Destroying GLFW window at {}", static_cast<void*>(mp_glfwWindow.get()));
    glfwDestroyWindow(mp_glfwWindow.get());

    LOG_TRACEthis("Terminating GLFW");
    glfwTerminate();
    LOG_INFOthis("Successfully terminated GLFW");
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

    mp_vulkanSurface = quartz::rendering::Window::createVulkanSurfacePtr(
        mp_glfwWindow,
        renderingInstance.getVulkanInstancePtr()
    );

    m_vulkanSurfaceCapabilities = renderingDevice.getVulkanPhysicalDevice().getSurfaceCapabilitiesKHR(
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

bool
quartz::rendering::Window::shouldClose() const {
    bool shouldClose = static_cast<bool>(
        glfwWindowShouldClose(mp_glfwWindow.get())
    );

    if (shouldClose) {
        LOG_INFOthis("Window should close");
    }

    return shouldClose;
}

void
quartz::rendering::Window::setShouldDisplayCursor(
    const bool shouldDisplayCursor
) {
    if (shouldDisplayCursor) {
        LOG_TRACEthis("Displaying cursor");
        glfwSetInputMode(mp_glfwWindow.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    } else {
        LOG_TRACEthis("Hiding cursor");
        glfwSetInputMode(mp_glfwWindow.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}
