#include <chrono>
#include <thread>

#include <GLFW/glfw3.h>

#include "util/logger/Logger.hpp"

#include "quartz/rendering/Loggers.hpp"
#include "quartz/rendering/window/Window.hpp"

int main() {
    REGISTER_LOGGER_GROUP(QUARTZ_RENDERING);
    util::Logger::setLevel("WINDOW", util::Logger::Level::trace);
    util::Logger::setLevel("VULKAN", util::Logger::Level::info);

    const std::string testName = "SuccessiveWindowsExecutable";

    LOG_INFO(WINDOW, "Creating rendering instance");
    quartz::rendering::Instance renderingInstance(testName, 9, 9, 9, true);

    LOG_INFO(WINDOW, "Creating rendering instance");
    quartz::rendering::Device renderingDevice(renderingInstance);

    {
        LOG_INFO(WINDOW, "Creating first window");
        quartz::rendering::Window window1("First_Window", 4, 4, renderingInstance, renderingDevice);

        LOG_INFO(WINDOW, "Sleeping this thread for 2 seconds");
        std::this_thread::sleep_for(std::chrono::seconds(2));

        LOG_INFO(WINDOW, "Done with first window");
    }

    LOG_INFO(WINDOW, "Sleeping this thread for 2 seconds");
    std::this_thread::sleep_for(std::chrono::seconds(2));

    {
        LOG_INFO(WINDOW, "Creating second window");
        quartz::rendering::Window window2("Second_Window", 4, 4, renderingInstance, renderingDevice);

        LOG_INFO(WINDOW, "Sleeping this thread for 2 seconds");
        std::this_thread::sleep_for(std::chrono::seconds(2));

        LOG_INFO(WINDOW, "Done with second window");
    }
}

