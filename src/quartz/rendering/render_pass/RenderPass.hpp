#pragma once

#include <vulkan/vulkan.hpp>

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/device/Device.hpp"
#include "quartz/rendering/window/Window.hpp"

namespace quartz {
namespace rendering {
    class RenderPass;
}
}

class quartz::rendering::RenderPass {
public: // member functions
    RenderPass(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window& renderingWindow
    );
    ~RenderPass();

    void reset();
    void recreate(
        const quartz::rendering::Device& renderingDevice,
        const quartz::rendering::Window& renderingWindow
    );

    USE_LOGGER(RENDERPASS);

    const vk::UniqueRenderPass& getVulkanRenderPassPtr() const { return mp_vulkanRenderPass; }

private: // static functions
    static vk::UniqueRenderPass createVulkanRenderPassPtr(
        const vk::UniqueDevice& p_logicalDevice,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const vk::Format& depthFormat
    );

private: // member variables
    vk::UniqueRenderPass mp_vulkanRenderPass;
};