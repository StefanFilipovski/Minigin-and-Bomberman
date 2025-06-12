#include "SceneManager.h"
#include "Scene.h"
#include <iostream>

void dae::SceneManager::Update(float deltaTime)
{
    if (m_ActiveScene) {
        m_ActiveScene->Update(deltaTime);
    }
}

void dae::SceneManager::Render()
{
    if (m_ActiveScene) {
        m_ActiveScene->Render();
    }
}

dae::Scene& dae::SceneManager::CreateScene(const std::string& name)
{
    // Remove any existing scene with the same name first
    RemoveScene(name);

    const auto& scene = std::shared_ptr<Scene>(new Scene(name));
    m_scenes.push_back(scene);
    return *scene;
}

void dae::SceneManager::RemoveScene(const std::string& name)
{
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&name](const std::shared_ptr<Scene>& scene) {
            return scene->GetName() == name;
        });

    if (it != m_scenes.end()) {
        // If this is the active scene, clear the active scene pointer
        if (m_ActiveScene == it->get()) {
            m_ActiveScene = nullptr;
        }

        // Remove all game objects from the scene first
        (*it)->RemoveAll();

        // Then remove the scene
        m_scenes.erase(it);
    }
}

void dae::SceneManager::SetActiveScene(const std::string& name)
{
    // Allow setting to null
    if (name.empty()) {
        m_ActiveScene = nullptr;
        return;
    }

    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&name](const std::shared_ptr<Scene>& scene) {
            return scene && scene->GetName() == name;
        });

    if (it != m_scenes.end()) {
        m_ActiveScene = it->get();
    }
    else {
        std::cerr << "Scene not found: " << name << std::endl;
        m_ActiveScene = nullptr;
    }
}

void dae::SceneManager::SetActiveScene(Scene* scene)
{
    m_ActiveScene = scene;
}

dae::Scene* dae::SceneManager::GetScene(const std::string& name) const
{
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&name](const std::shared_ptr<Scene>& scene) {
            return scene->GetName() == name;
        });

    if (it != m_scenes.end()) {
        return it->get();
    }
    return nullptr;
}

void dae::SceneManager::RemoveAllScenes()
{
    // Clear active scene pointer first
    m_ActiveScene = nullptr;

    // Clear all scenes - this will trigger their destructors
    m_scenes.clear();
}

// Overload for nullptr
void dae::SceneManager::SetActiveScene(std::nullptr_t)
{
    m_ActiveScene = nullptr;
}