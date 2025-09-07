# Quartz

Quartz is an interactive realtime 3D engine. Quartz uses Vulkan.

![PBR Demo Gif](https://github.com/KingLineSoftworks/assets/blob/master/media/PBR%20Rendering%20Demo%20Shortened.gif)

## Building
This project is built with CMake. It is currently supporting MacOS and Linux. No efforts have been made to build and run on Windows. There is a known issue on Linux environments using Wayland to manage windows where Quartz segfaults at the end of the program, when it is trying to free a vulkan surface. This issue is not noticed on MacOS or on Linux environments using Xorg.

By default, the `CMakeLists.txt` is configured to build using clang as the compiler and ninja as the generator. This is configured in `PreLoad.cmake`.
The `CMakeLists.txt` also tells GLFW to use Wayland instead of Xorg if the detected platform is Linux. This can be changeed by modifying the `GLFW_BUILD_WAYLAND` and `GLFW_BUILD_X11` flags.

### Running Tests
If you desire to build and run the unit tests, ensure you are passing the `QUARTZ_COMPILE_UNIT_TESTS` to cmake.
You can do this via command line by adding the `-DQUARTZ_COMPILE_UNIT_TESTS`, or by setting this flag in your CMakeLists.txt like so:
```
set(QUARTZ_COMPILE_UNIT_TESTS ON CACHE BOOL "" FORCE)
```

Unit tests will be output to the build directory's `bin/ut` directory.
You can execute all unit tests with the `ctest` command.
See the `Unit Test Runner Usage` section of the [contribution guidelines](docs/contributing/contribution_guidelines.md) for information on running individual test cases.

## Scening Documentation
- [GLTF 2.0 models](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html)
- [PBR Materials via metallic-roughness model](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#materials)
- [Right handed coordinate system](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#coordinate-system-and-units)
    - Forward = `(1,0,0)`, Up = `(0,1,0)`, Right = `(0,0,1)`

## Physics Documentation
- Speeds are measured in meters / second
- Forces are applied in the unit of Newtons
- Torques are applied in the unit of Newton-Meters
- All forces and torques will be reset to 0 at the end of each fixed update. So torques and forces must be applied throughout all frames in which you wish the force or torque to take effect

## Contributing
Read the [contribution guidelines](docs/contributing/contribution_guidelines.md).

## Using Quartz
If you desire to use Quartz for your own application, the Quartz Demo App (in `demo_app/demo_app`) might provide a good starting point.

Additionally, the [Pole Position Simulator](https://github.com/KingLineSoftworks/PolePosition) provides a great example for how to include Quartz into your own project.

### Physics Properties

#### Triggers
All colliders are allowed to have collision callbacks which determine which logic they invoke upon collision start, stay, and exit.
If you set a collider to be a trigger, you will not be able to collide with it physically, though it's collision callbacks will still be invoked when a collision (overlap) occurs.
