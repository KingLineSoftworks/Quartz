#====================================================================
# Quartz Scene SkyBox Unit Tests
#====================================================================

set(
    SkyBox_libs
    QUARTZ_SCENE_SkyBox
    UTIL_FileSystem
)

create_unit_test(
    test_SkyBox
    test_SkyBox.cpp
    "${SkyBox_libs}"
)

