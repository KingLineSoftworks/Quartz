#include "quartz/managers/scene_manager/SceneManager.hpp"
#include "util/logger/Logger.hpp"

quartz::managers::SceneManager::SceneManager() :
    m_sceneParameters(),
    m_scenes(),
    m_currentlyLoadedSceneIndex(0)
{
    LOG_FUNCTION_CALL_TRACEthis("");
}

quartz::managers::SceneManager::~SceneManager() {
    LOG_FUNCTION_CALL_TRACEthis("");
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

    /** @todo 2024/11/07 Unload the currently loaded scene before we load the next one */

    m_currentlyLoadedSceneIndex = index;

    const quartz::scene::Scene::Parameters& sceneParameters = m_sceneParameters[index];

    m_scenes[index].load(
        renderingDevice,
        physicsManager,
        sceneParameters.camera,
        sceneParameters.ambientLight,
        sceneParameters.directionalLight,
        sceneParameters.pointLights,
        sceneParameters.spotLights,
        sceneParameters.screenClearColor,
        sceneParameters.skyBoxInformation,
        sceneParameters.doodadInformations
    );

    return m_scenes[index];
}

