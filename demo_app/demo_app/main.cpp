#include <iostream>

#include "quartz/core.hpp"

#include "demo_app/core.hpp"

int main() {
    ASSERT_QUARTZ_VERSION();
    ASSERT_APPLICATION_VERSION();

    std::cout << "quartz version " << QUARTZ_MAJOR_VERSION << "." << QUARTZ_MINOR_VERSION << "." << QUARTZ_PATCH_VERSION << "\n";
    std::cout << "app version " << APPLICATION_MAJOR_VERSION << "." << APPLICATION_MINOR_VERSION << "." << APPLICATION_PATCH_VERSION << "\n";

    return 0;
}