# Quartz

Quartz is an interactive realtime 3D engine. Quartz uses Vulkan.

![PBR Demo Gif](https://github.com/KingLineSoftworks/assets/blob/master/media/PBR%20Rendering%20Demo%20Shortened.gif)

## Building
This project is built with CMake. It is currently supporting MacOS and Linux. No efforts have been made to build and run on Windows. There is a known issue on Linux environments using Wayland to manage windows where Quartz segfaults at the end of the program, when it is trying to free a vulkan surface. This issue is not noticed on MacOS or on Linux environments using Xorg.

By default, the `CMakeLists.txt` is configured to build using clang as the compiler and ninja as the generator. This is configured in `PreLoad.cmake`.
The `CMakeLists.txt` also tells GLFW to use Wayland instead of Xorg if the detected platform is Linux. This can be changeed by modifying the `GLFW_BUILD_WAYLAND` and `GLFW_BUILD_X11` flags.

#### Running Tests

@todo: 2025/07/09 Add instructions about building and running the tests

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
Pick up an issue, put on some [acid techno](https://soundcloud.com/intercell/kiki-at-intercell-acid-night-2020?si=ef2408dbdc3146369b2ea2264687329a&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing) or some [ambient dnb](https://soundcloud.com/deep-z-lost-in-heaven/lost-in-heaven-131-dnb-mix-april-2023-atmospheric-liquid-drum-and-bass?si=767ead2f66584ef2a8a62ac51ef785a2&utm_source=clipboard&utm_medium=text&utm_campaign=social_sharing), and do good work.

### Merging vs Rebasing
If your branch is out of date with master (or any other branch) please do a rebase instead of a merge. We want a linear commit history, for ease of auditing. If you're not familiar with rebasing, it is not hard. Google is your friend.
Do `git rebase master your_branch` to take all of the commits on `your_branch` and appy them on top of master.

### Tests

Make sure your additions are tested. There are examples of how to write unit tests and use the quartz testing framework.
The quartz unit testing framework is defined in the `src/util/unit_test` directory.
Examples on how to use this unit testing framework are in the `test/unit/example` directory.

#### Unit Test Runner Usage Via Command Line

This runner allows you to specify individual test cases from the command line, while still allowing the whole file to be executed via ctest.

To execute a subset of tests within a file via command line, provide the -c flag like so:
```
./bin/ut/test_file_something -c test_case_2
```
You can also specify multiple test cases to run
```
./bin/ut/test_file_something -c test_case_3 test_case_3 test_case_99
```

By default all of the loggers are turned off besides the UNIT_TEST and UT_RUNNER loggers which are set to critical and trace respectively.
If you wish for more verbose logs, you can specify the level of individual loggers via command line arguments with the -l flag like so:
```
./bin/ut/test_another_one -l RIGIDBODY:debug FIELD:trace COLLIDER:error
```

You can also specify test cases and loggers at the same time:
```
./bin/ut/test_hehehe -c test_case_yoyo test_case_lala -l BUFFER:trace IMAGE:critical MODEL:info
./bin/ut/test_hehehe -l DOODAD:trace INPUTMAN:critical -c test_case_yoyo_2 test_case_lala_the_sequel
```


## Using Quartz
If you desire to use Quartz for your own application, the Quartz Demo App (in `demo_app/demo_app`) might provide a good starting point.

Additionally, the [Pole Position Simulator](https://github.com/KingLineSoftworks/PolePosition) provides a great example for how to include Quartz into your own project.

### Physics Properties

#### Triggers
All colliders are allowed to have collision callbacks which determine which logic they invoke upon collision start, stay, and exit.
If you set a collider to be a trigger, you will not be able to collide with it physically, though it's collision callbacks will still be invoked when a collision (overlap) occurs.
