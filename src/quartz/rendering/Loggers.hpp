#pragma once

#include "util/logger/Logger.hpp"

DECLARE_LOGGER(BUFFER, trace);
DECLARE_LOGGER(BUFFER_MAPPED, trace);
DECLARE_LOGGER(BUFFER_STAGED, trace);
DECLARE_LOGGER(BUFFER_IMAGE, trace);
DECLARE_LOGGER(CONTEXT, trace);
DECLARE_LOGGER(CUBEMAP, trace);
DECLARE_LOGGER(DEPTHBUFFER, trace);
DECLARE_LOGGER(DEVICE, trace);
DECLARE_LOGGER(IMAGE, trace);
DECLARE_LOGGER(INSTANCE, trace);
DECLARE_LOGGER(MATERIAL, trace);
DECLARE_LOGGER(MODEL, trace);
DECLARE_LOGGER(MODEL_MESH, trace);
DECLARE_LOGGER(MODEL_NODE, trace);
DECLARE_LOGGER(MODEL_PRIMITIVE, trace);
DECLARE_LOGGER(MODEL_SCENE, trace);
DECLARE_LOGGER(PIPELINE, trace);
DECLARE_LOGGER(RENDERPASS, trace);
DECLARE_LOGGER(SWAPCHAIN, trace);
DECLARE_LOGGER(TEXTURE, trace);
DECLARE_LOGGER(VULKAN, trace);
DECLARE_LOGGER(VULKANUTIL, trace);
DECLARE_LOGGER(WINDOW, trace);

DECLARE_LOGGER_GROUP(
        QUARTZ_RENDERING,
        23,
        BUFFER,
        BUFFER_MAPPED,
        BUFFER_STAGED,
        BUFFER_IMAGE,
        CONTEXT,
        CUBEMAP,
        DEPTHBUFFER,
        DEVICE,
        IMAGE,
        INSTANCE,
        MATERIAL,
        MODEL,
        MODEL_MESH,
        MODEL_PRIMITIVE,
        MODEL_NODE,
        MODEL_SCENE,
        PIPELINE,
        RENDERPASS,
        SWAPCHAIN,
        TEXTURE,
        VULKAN,
        VULKANUTIL,
        WINDOW
);
