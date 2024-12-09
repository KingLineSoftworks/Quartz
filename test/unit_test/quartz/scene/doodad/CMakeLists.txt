#====================================================================
# The Quartz Scene Doodad unit tests
#====================================================================

list(APPEND current_linked_libraries
    QUARTZ_PHYSICS_Collider
    QUARTZ_PHYSICS_RigidBody
    QUARTZ_SCENE_Doodad
)
create_unit_test(
    test_doodad_rigidbody_collider_transforms
    test_doodad_rigidbody_collider_transforms.cpp
    "${current_linked_libraries}"
)

