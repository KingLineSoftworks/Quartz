#include "unit_test/Util.hpp"

#include "quartz/managers/input_manager/InputManager.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"

quartz::managers::InputManager&
quartz::unit_test::UnitTestClient::getInputManagerDummyInstance() {
    return quartz::managers::InputManager::Client::getInstance(nullptr);
}

quartz::managers::PhysicsManager&
quartz::unit_test::UnitTestClient::getPhysicsManagerInstance() {
    return quartz::managers::PhysicsManager::Client::getInstance();
}
