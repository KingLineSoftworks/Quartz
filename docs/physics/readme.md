# Physics

## General Information

- Speeds are measured in meters / second
- Forces are applied in the unit of Newtons
- Torques are applied in the unit of Newton-Meters
- All forces and torques will be reset to 0 at the end of each fixed update. So torques and forces must be applied throughout all frames in which you wish the force or torque to take effect

## Physics Properties

### Triggers

All colliders are allowed to have collision callbacks which determine which logic they invoke upon collision start, stay, and exit.
If you set a collider to be a trigger, you will not be able to collide with it physically, though it's collision callbacks will still be invoked when a collision (overlap) occurs.
