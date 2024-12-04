#====================================================================
# The Quartz Physics RigidBody unit tests
#====================================================================

list(APPEND current_linked_libraries
    QUARTZ_MANAGERS_PhysicsManager
    QUARTZ_PHYSICS_Collider
    QUARTZ_PHYSICS_Field
    QUARTZ_PHYSICS_RigidBody
)
create_unit_test(
    test_rigidbody_collider_pointers
    test_rigidbody_collider_pointers.cpp
    "${current_linked_libraries}"
)

