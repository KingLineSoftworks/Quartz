#pragma once

#include <array>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <vulkan/vulkan.hpp>

#include "quartz/Loggers.hpp"
#include "quartz/rendering/window/Window.hpp"

namespace quartz {
    struct UniformBufferObject;
    struct Vertex;
    class Application;
}

struct quartz::UniformBufferObject {
public: // member functions
    UniformBufferObject() = default;

    UniformBufferObject(
        glm::mat4 model_,
        glm::mat4 view_,
        glm::mat4 projection_
    );

public: // member variables
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 projection;
};

struct quartz::Vertex {
public: // member functions
    Vertex(
        const glm::vec3& worldPosition_,
        const glm::vec3& color_
    );

public: // static functions
    static vk::VertexInputBindingDescription getVulkanVertexInputBindingDescription();
    static std::array<vk::VertexInputAttributeDescription, 2> getVulkanVertexInputAttributeDescriptions();

public: // member variables
    glm::vec3 worldPosition;
    glm::vec3 color;
};

class quartz::Application {
public: // classes and enums
    struct QueueFamilyIndices {
        uint32_t graphicsFamilyIndex;
        uint32_t presentFamilyIndex;
    };

    struct SwapchainSupportDetails {
        vk::SurfaceCapabilitiesKHR surfaceCapabilities;
        std::vector<vk::SurfaceFormatKHR> surfaceFormats;
        std::vector<vk::PresentModeKHR> presentModes;
    };

    struct PipelineInformation {
        // The things the create infos store as references
        vk::VertexInputBindingDescription vertexInputBindingDescriptions;
        std::array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescriptions;
        std::vector<vk::Viewport> viewports;
        std::vector<vk::Rect2D> scissorRectangles;
        std::vector<vk::PipelineColorBlendAttachmentState> colorBlendAttachmentStates;
        std::vector<vk::DynamicState> dynamicStates;

        // The create infos
        std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos;
        vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
        vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
        vk::PipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo;
        vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
        vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
        vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
        vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo;
        vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
        vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
    };

public: // member functions
    Application(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const uint32_t windowWidthPixels,
        const uint32_t windowHeightPixels,
        const bool validationLayersEnabled
    );
    ~Application();

    USE_LOGGER(APPLICATION);

    void run();

private: // member functions

    void recreateSwapchain();
    void updateUniformBuffer(const uint32_t currentInFlightFrameIndex);
    void resetAndRecordDrawingCommandBuffer(const uint32_t currentInFlightFrameIndex, const uint32_t swapchainImageIndex);
    void drawFrameToWindow(const uint32_t currentInFlightFrameIndex);

public: // static functions
    // The callback the validation layer uses
    static VKAPI_ATTR VkBool32 VKAPI_CALL vulkanDebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* p_callbackData,
        void* p_userData
    );

private: // static functions

    static std::vector<const char*> getEnabledValidationLayerNames(
        const bool validationLayersEnabled
    );

    static std::vector<const char*> getEnabledInstanceExtensionNames(
        const bool validationLayersEnabled
    );

    static vk::UniqueInstance createVulkanUniqueInstance(
        const std::string& applicationName,
        const uint32_t applicationMajorVersion,
        const uint32_t applicationMinorVersion,
        const uint32_t applicationPatchVersion,
        const std::vector<const char*>& enabledValidationLayerNames,
        const std::vector<const char*>& enabledExtensionNames
    );

    static vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> createVulkanUniqueDebugUtilsMessenger(
        const vk::UniqueInstance& uniqueInstance,
        const vk::DispatchLoaderDynamic& dispatchLoaderDynamic,
        const bool validationLayersEnabled
    );

    static vk::UniqueSurfaceKHR createVulkanSurface(
        const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
        const vk::UniqueInstance& uniqueInstance
    );

    static std::pair<vk::PhysicalDevice, quartz::Application::QueueFamilyIndices> getBestPhysicalDeviceAndQueueFamilyIndices(
        const vk::UniqueInstance& uniqueInstance
    );

    static std::vector<const char*> getEnabledPhysicalDeviceExtensionNames(
        const vk::PhysicalDevice& physicalDevice
    );

    static vk::UniqueDevice createVulkanUniqueLogicalDevice(
        const vk::PhysicalDevice& physicalDevice,
        const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
        const std::vector<const char*>& validationLayerNames,
        const std::vector<const char*>& physicalDeviceExtensionNames
    );

    static vk::Extent2D getBestSwapExtent(
        const std::shared_ptr<const GLFWwindow>& p_GLFWwindow,
        const vk::SurfaceCapabilitiesKHR& surfaceCapabilities
    );

    static vk::SurfaceFormatKHR getBestSurfaceFormat(
        const vk::UniqueSurfaceKHR& uniqueSurface,
        const vk::PhysicalDevice& physicalDevice
    );

    static vk::PresentModeKHR getBestPresentMode(
        const vk::UniqueSurfaceKHR& uniqueSurface,
        const vk::PhysicalDevice& physicalDevice
    );

    static vk::UniqueSwapchainKHR createVulkanUniqueSwapchain(
        const vk::UniqueSurfaceKHR& uniqueSurface,
        const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::SurfaceCapabilitiesKHR& surfaceCapabilities,
        const vk::Extent2D& swapchainExtent,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const vk::PresentModeKHR& presentMode
    );

    static std::vector<vk::UniqueImageView> createVulkanUniqueSwapchainImageViews(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::SurfaceFormatKHR& surfaceFormat,
        const std::vector<vk::Image>& swapchainImages
    );

    static vk::UniqueShaderModule createVulkanUniqueShaderModule(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const std::string& filepath
    );

    static quartz::Application::PipelineInformation getPipelineInformation(
        const vk::Extent2D& swapchainExtent,
        const vk::UniqueShaderModule& uniqueVertexShaderModule,
        const vk::UniqueShaderModule& uniqueFragmentShaderModule
    );

    static vk::UniqueRenderPass createVulkanUniqueRenderPass(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::Format& surfaceFormatFormat
    );

    static vk::UniqueDescriptorSetLayout createVulkanUniqueDescriptorSetLayout(
        const vk::UniqueDevice& uniqueLogicalDevice
    );

    static vk::UniquePipelineLayout createVulkanUniquePipelineLayout(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::UniqueDescriptorSetLayout& uniqueDescriptorSetLayout
    );

    static vk::UniquePipeline createVulkanUniqueGraphicsPipeline(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const quartz::Application::PipelineInformation& pipelineInformation,
        const vk::UniquePipelineLayout& uniquePipelineLayout,
        const vk::UniqueRenderPass& uniqueRenderPass
    );

    static std::vector<vk::UniqueFramebuffer> createVulkanUniqueFramebuffers(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::Extent2D& swapchainExtent,
        const std::vector<vk::UniqueImageView>& uniqueSwapchainImageViews,
        const vk::UniqueRenderPass& uniqueRenderPass
    );

    static vk::UniqueCommandPool createVulkanUniqueCommandPool(
        const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::CommandPoolCreateFlags commandPoolCreateFlags
    );

    static std::vector<vk::UniqueCommandBuffer> createVulkanUniqueDrawingCommandBuffers(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::UniqueCommandPool& uniqueCommandPool,
        const uint32_t desiredCommandBufferCount
    );

    static std::vector<vk::UniqueSemaphore> createVulkanUniqueSemaphores(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t maxNumFramesInFlight
    );

    static std::vector<vk::UniqueFence> createVulkanUniqueFences(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t maxNumFramesInFlight
    );

    static std::vector<quartz::Vertex> loadSceneVertices();

    static std::vector<uint32_t> loadSceneIndices();

    static vk::UniqueBuffer createVulkanUniqueBuffer(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t bufferSizeBytes,
        const vk::BufferUsageFlags bufferUsageFlags
    );

    static vk::UniqueDeviceMemory allocateVulkanUniqueBufferMemory(
        const vk::PhysicalDevice& physicalDevice,
        const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t bufferSizeBytes,
        const void* p_bufferData,
        const vk::UniqueBuffer& uniqueDestinationBuffer,
        const vk::MemoryPropertyFlags requiredMemoryProperties,
        const vk::UniqueBuffer* p_uniqueSourceBuffer,
        const vk::Queue& graphicsQueue,
        const bool dontMap
    );

    static std::vector<vk::UniqueBuffer> createVulkanUniqueBuffers(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t maxNumFramesInFlight,
        const uint32_t bufferSizeBytes,
        const vk::BufferUsageFlags bufferUsageFlags
    );

    static std::vector<vk::UniqueDeviceMemory> allocateVulkanUniqueBufferMemories(
        const vk::PhysicalDevice& physicalDevice,
        const quartz::Application::QueueFamilyIndices& queueFamilyIndices,
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t bufferSizeBytes,
        const std::vector<void*> bufferDataPtrs,
        const std::vector<vk::UniqueBuffer>& uniqueDestinationBuffers,
        const vk::MemoryPropertyFlags requiredMemoryProperties,
        const std::vector<vk::UniqueBuffer*> uniqueSourceBufferPtrs,
        const vk::Queue& graphicsQueue
    );

    static std::vector<void*> mapVulkanUniqueBufferMemories(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t bufferSizeBytes,
        const std::vector<vk::UniqueDeviceMemory>& uniqueDeviceMemories
    );

    static vk::UniqueDescriptorPool createVulkanUniqueDescriptorPool(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const uint32_t maxNumFramesInFlight
    );

    static std::vector<vk::DescriptorSet> allocateVulkanUniqueDescriptorSets(
        const vk::UniqueDevice& uniqueLogicalDevice,
        const vk::UniqueDescriptorSetLayout& uniqueDescriptorSetLayout,
        const uint32_t maxNumFramesInFlight,
        const std::vector<vk::UniqueBuffer>& uniqueUniformBuffers,
        const vk::UniqueDescriptorPool& uniqueDescriptorPool
    );

private: // member variables
    const std::string m_applicationName;
    const uint32_t m_majorVersion;
    const uint32_t m_minorVersion;
    const uint32_t m_patchVersion;

    // ----- Context tings (tings lishted in the oahdah of which dey ahh creah'id mang) ---- //

    // instance
    std::vector<const char*> m_validationLayerNames;
    std::vector<const char*> m_instanceExtensionNames;
    vk::UniqueInstance m_vulkanUniqueInstance;
    vk::DispatchLoaderDynamic m_vulkanDispatchLoaderDynamic;
    vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_vulkanUniqueDebugMessenger; // we need to use the dynamic loader instead of the static loader (not sure why we can't statically link 😔)

    // device (and its queues)
    std::pair<vk::PhysicalDevice, quartz::Application::QueueFamilyIndices> m_vulkanPhysicalDeviceAndQueueFamilyIndex; // Because these are both (physical device && queue family indices) determined at the "same" time and truly are coupled
    std::vector<const char*> m_physicalDeviceExtensionNames;
    vk::UniqueDevice  m_vulkanUniqueLogicalDevice;
    vk::Queue m_vulkanGraphicsQueue;
    vk::Queue m_vulkanPresentQueue;

    // window
    std::shared_ptr<quartz::rendering::Window> mp_window;
    vk::UniqueSurfaceKHR m_vulkanUniqueSurface;
    vk::SurfaceCapabilitiesKHR m_vulkanSurfaceCapabilities;
    vk::SurfaceFormatKHR m_vulkanSurfaceFormat;
    vk::PresentModeKHR m_vulkanPresentMode;
    vk::Extent2D m_vulkanSwapchainExtent;

    // graphics pipeline
    const uint32_t m_maxNumFramesInFlight;
    // shaders
    vk::UniqueShaderModule m_vulkanUniqueVertexShaderModule;
    vk::UniqueShaderModule m_vulkanUniqueFragmentShaderModule;
    // uniform buffers for shader uniforms and their descriptor sets
    std::vector<vk::UniqueBuffer> m_vulkanUniqueUniformBuffers;
    std::vector<vk::UniqueDeviceMemory> m_vulkanUniqueUniformBufferMemories;
    std::vector<void*> m_mappedUniformBufferMemories;
    vk::UniqueDescriptorSetLayout m_vulkanUniqueDescriptorSetLayout;
    vk::UniqueDescriptorPool m_vulkanUniqueDescriptorPool;
    std::vector<vk::DescriptorSet> m_vulkanUniqueDescriptorSets;
    // render pass
    vk::UniqueRenderPass m_vulkanUniqueRenderPass;
    // graphics pipeline
    vk::UniquePipelineLayout m_vulkanUniquePipelineLayout;
    quartz::Application::PipelineInformation m_pipelineInformation;
    vk::UniquePipeline m_vulkanUniqueGraphicsPipeline;

    // swapchain
    vk::UniqueSwapchainKHR m_vulkanUniqueSwapchain;
    std::vector<vk::Image> m_vulkanSwapchainImages;
    std::vector<vk::UniqueImageView> m_vulkanUniqueSwapchainImageViews;
    // Framebuffer tings
    std::vector<vk::UniqueFramebuffer> m_vulkanUniqueFramebuffers;
    // command pools/buffers and synchronization objects
    vk::UniqueCommandPool m_vulkanUniqueDrawingCommandPool;
    std::vector<vk::UniqueCommandBuffer> m_vulkanUniqueDrawingCommandBuffers;
    std::vector<vk::UniqueSemaphore> m_vulkanUniqueImageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> m_vulkanUniqueRenderFinishedSemaphores;
    std::vector<vk::UniqueFence> m_vulkanUniqueInFlightFences;

    // Scene drawing information (should go with graphics pipeline?)
    std::vector<quartz::Vertex> m_vertices;
    vk::UniqueBuffer m_vulkanUniqueVertexStagingBuffer;
    vk::UniqueDeviceMemory m_vulkanUniqueVertexStagingBufferMemory;
    vk::UniqueBuffer m_vulkanUniqueVertexBuffer;
    vk::UniqueDeviceMemory m_vulkanUniqueVertexBufferMemory;
    std::vector<uint32_t> m_indices;
    vk::UniqueBuffer m_vulkanUniqueIndexStagingBuffer;
    vk::UniqueDeviceMemory m_vulkanUniqueIndexStagingBufferMemory;
    vk::UniqueBuffer m_vulkanUniqueIndexBuffer;
    vk::UniqueDeviceMemory m_vulkanUniqueIndexBufferMemory;
};