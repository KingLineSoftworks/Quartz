# Quartz

Quartz is an interactive realtime 3D engine. Quartz uses Vulkan.

![PBR Demo Gif](https://github.com/KingLineSoftworks/assets/blob/master/media/PBR%20Rendering%20Demo%20Shortened.gif)

## Building

This project is built with CMake.
It is currently supporting MacOS and Linux.
No efforts have been made to build and run on Windows.
There is a known issue on Linux environments using Wayland to manage windows where Quartz segfaults at the end of the program, when it is trying to free a vulkan surface. This issue is not noticed on MacOS or on Linux environments using Xorg.

Quartz leverages C++23 for `std::basic_stacktrace`, so ensure that your toolchain allows for compilation of the C++23 standard.

By default, the `CMakeLists.txt` is configured to build using clang as the compiler and ninja as the generator.
This is configured in `PreLoad.cmake`.
The `CMakeLists.txt` also tells GLFW to use Wayland instead of Xorg if the detected platform is Linux.
This can be changeed by modifying the `GLFW_BUILD_WAYLAND` and `GLFW_BUILD_X11` flags.

### Running Tests

If you desire to build and run the unit tests, ensure you are passing the `QUARTZ_COMPILE_UNIT_TESTS` to cmake.
You can do this via command line by adding the `-DQUARTZ_COMPILE_UNIT_TESTS`, or by setting this flag in your CMakeLists.txt like so:
```
set(QUARTZ_COMPILE_UNIT_TESTS ON CACHE BOOL "" FORCE)
```

Unit tests will be output to the build directory's `bin/ut` directory.
You can execute all unit tests with the `ctest` command.
See the [Unit Test Runner Usage section](docs/contributing/readme.md#Unit-Test-Runner-Usage-Via-Command-Line) of the contribution guidelines for information on running individual test cases.

## Scening Documentation

Read the [scening documentation](docs/scening).

## Physics Documentation

Read the [physics documentation](docs/physics).

## Contributing

Read the [contribution guidelines](docs/contributing).

## Using Quartz

The [Pole Position Simulator](https://github.com/KingLineSoftworks/PolePosition) provides a great example for how to include Quartz into your own project.

