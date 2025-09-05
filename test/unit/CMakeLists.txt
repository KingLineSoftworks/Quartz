#====================================================================
# Example unit tests
#====================================================================

# add_subdirectory("example")

#====================================================================
# Math unit tests
#====================================================================

add_subdirectory("math/algorithms")
add_subdirectory("math/transform")

#====================================================================
# Util unit tests
#====================================================================

add_subdirectory("util/file_system")
add_subdirectory("util/logger")

#====================================================================
# Quartz unit tests
#====================================================================

add_subdirectory("quartz/physics/collider")
add_subdirectory("quartz/physics/field")
add_subdirectory("quartz/physics/rigid_body")

add_subdirectory("quartz/rendering/material")

add_subdirectory("quartz/scene/camera")
add_subdirectory("quartz/scene/doodad")
add_subdirectory("quartz/scene/light")
add_subdirectory("quartz/scene/scene")
add_subdirectory("quartz/scene/sky_box")

