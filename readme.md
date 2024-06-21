# Quartz

## Building
This project is built with CMake. It is currently supporting MacOS and Linux. No efforts have been made to build and run on Windows. There is a known issue on Linux environments using Wayland to manage windows where Quartz segfaults at the end of the program, when it is trying to free a vulkan surface. This issue is not noticed on MacOS or on Linux environments using Xorg.

By default, the `CMakeLists.txt` is configured to build using clang as the compiler and ninja as the generator. This is configured in `PreLoad.cmake`.
The `CMakeLists.txt` also tells GLFW to use Wayland instead of Xorg if the detected platform is Linux. This can be changeed by modifying the `GLFW_BUILD_WAYLAND` and `GLFW_BUILD_X11` flags.

## Scening Documentation
- [GLTF 2.0 models](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html)
- [PBR Materials via metallic-roughness model](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#materials)
- [Right handed coordinate system](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#coordinate-system-and-units)

## Physics Documentation
- Speeds are measured in meters / second
- Forces are applied in the unit of Newtons
- Torques are applied in the unit of Newton-Meters
- All forces and torques will be reset to 0 at the end of each fixed update. So torques and forces must be applied throughout all frames in which you wish the force or torque to take effect

## Contributing
Pick up an issue, put on some [acid techno](https://soundcloud.com/intercell/kiki-at-intercell-acid-night-2020?si=ef2408dbdc3146369b2ea2264687329a&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing) or some [ambient dnb](https://soundcloud.com/deep-z-lost-in-heaven/lost-in-heaven-131-dnb-mix-april-2023-atmospheric-liquid-drum-and-bass?si=767ead2f66584ef2a8a62ac51ef785a2&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing), and do good work.
