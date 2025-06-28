#pragma once

#include <vector>

#include "quartz/managers/Loggers.hpp"
#include "quartz/managers/physics_manager/PhysicsManager.hpp"
#include "quartz/scene/scene/Scene.hpp"

namespace quartz {

class Application; // forward declare the Application class so we can declare it as a friend

namespace managers {
    class SceneManager;
}
}

class quartz::managers::SceneManager {
public: // classes
    class Client {
    public: // member functions
        Client() = delete;

    private: // static functions
        static quartz::managers::SceneManager& getInstance() { return quartz::managers::SceneManager::getInstance(); }
        static quartz::managers::SceneManager& getInstance(const quartz::scene::Scene::Parameters& sceneParameters) { return quartz::managers::SceneManager::getInstance(sceneParameters); }
        static quartz::managers::SceneManager& getInstance(const std::vector<quartz::scene::Scene::Parameters>& sceneParameters) { return quartz::managers::SceneManager::getInstance(sceneParameters); }

    private: // friend classes
        friend class quartz::Application;
    };

public: // member functions
    SceneManager(const SceneManager& other) = delete;
    SceneManager(SceneManager&& other) = delete;
    SceneManager& operator=(const SceneManager& other) = delete;
    SceneManager& operator=(SceneManager&& other) = delete;
    ~SceneManager();

    USE_LOGGER(SCENEMAN);

    quartz::scene::Scene& loadScene(
        const quartz::rendering::Device& renderingDevice,
        quartz::managers::PhysicsManager& physicsManager,
        const uint32_t index
    );
    void unloadCurrentScene(
        quartz::managers::PhysicsManager& physicsManager
    );

private: // member functions
    SceneManager();

private: // static functions
    static SceneManager& getInstance();
    static SceneManager& getInstance(const quartz::scene::Scene::Parameters& sceneParameters);
    static SceneManager& getInstance(const std::vector<quartz::scene::Scene::Parameters>& sceneParameters);

private: // member variables
    std::vector<quartz::scene::Scene::Parameters> m_sceneParameters;
    std::vector<quartz::scene::Scene> m_scenes;
    uint32_t m_currentlyLoadedSceneIndex;
};

