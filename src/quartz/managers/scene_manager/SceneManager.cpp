#include "quartz/managers/scene_manager/SceneManager.hpp"
#include "util/logger/Logger.hpp"
#include <_strings.h>

quartz::managers::SceneManager::SceneManager() :
    m_sceneParameters(),
    m_scenes(),
    m_currentlyLoadedSceneIndex(0)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::managers::SceneManager::~SceneManager() {
    LOG_FUNCTION_SCOPE_TRACEthis("");
}

quartz::managers::SceneManager&
quartz::managers::SceneManager::getInstance() {
    LOG_FUNCTION_SCOPE_TRACE(SCENEMAN, "");

    static quartz::managers::SceneManager sceneManagerInstance;

    return sceneManagerInstance;
}

quartz::managers::SceneManager&
quartz::managers::SceneManager::getInstance(
    const quartz::scene::Scene::Parameters& sceneParameters
) {
    LOG_FUNCTION_SCOPE_TRACE(SCENEMAN, "");

    quartz::managers::SceneManager& sceneManagerInstance = quartz::managers::SceneManager::getInstance();

    sceneManagerInstance.m_sceneParameters.push_back(sceneParameters);
    sceneManagerInstance.m_scenes.emplace_back();

    return sceneManagerInstance;
}

quartz::managers::SceneManager&
quartz::managers::SceneManager::getInstance(
    const std::vector<quartz::scene::Scene::Parameters>& sceneParameters
) {
    LOG_FUNCTION_SCOPE_TRACE(SCENEMAN, "");

    quartz::managers::SceneManager& sceneManagerInstance = quartz::managers::SceneManager::getInstance();

    for (uint32_t i = 0; i < sceneParameters.size(); ++i) {
        LOG_TRACE(SCENEMAN, "Adding scene {}: {}", i, sceneParameters[i].name);
        sceneManagerInstance.m_sceneParameters.push_back(sceneParameters[i]);
        sceneManagerInstance.m_scenes.emplace_back();
    }

    return sceneManagerInstance;
}

quartz::scene::Scene&
quartz::managers::SceneManager::loadScene(
    const quartz::rendering::Device& renderingDevice,
    quartz::managers::PhysicsManager& physicsManager,
    const uint32_t index
) {
    LOG_FUNCTION_CALL_TRACE(SCENEMAN, "index {}", index);

    this->unloadCurrentScene(physicsManager);

    m_currentlyLoadedSceneIndex = index;

    const quartz::scene::Scene::Parameters& sceneParameters = m_sceneParameters[index];

    m_scenes[index].load(
        renderingDevice,
        physicsManager,
        sceneParameters.ambientLight,
        sceneParameters.directionalLight,
        sceneParameters.pointLights,
        sceneParameters.spotLights,
        sceneParameters.screenClearColor,
        sceneParameters.skyBoxInformation,
        sceneParameters.doodadParameters,
        sceneParameters.o_fieldParameters
    );

    return m_scenes[index];
}

void
quartz::managers::SceneManager::unloadCurrentScene(
    quartz::managers::PhysicsManager& physicsManager
) {
    LOG_FUNCTION_CALL_TRACE(SCENEMAN, "index {}", m_currentlyLoadedSceneIndex);

    if (m_scenes.empty()) {
        return;
    }

    m_scenes[m_currentlyLoadedSceneIndex].unload(physicsManager);
}

void
quartz::managers::SceneManager::destroyAllScenes() {
    m_sceneParameters.clear();
    m_scenes.clear();
    m_currentlyLoadedSceneIndex = 0;
}
