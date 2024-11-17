#include "unit_test/Util.hpp"

#include "quartz/managers/physics_manager/PhysicsManager.hpp"

quartz::managers::PhysicsManager&
quartz::unit_test::UnitTestClient::getPhysicsManagerInstance() {
    return quartz::managers::PhysicsManager::Client::getInstance();
}
