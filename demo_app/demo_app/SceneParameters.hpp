#pragma once

#include <vector>

#include "quartz/scene/scene/Scene.hpp"

#include "demo_app/player/Player.hpp"

quartz::scene::Scene::Parameters createDemoLevelSceneParameters(
    Player& player
);
