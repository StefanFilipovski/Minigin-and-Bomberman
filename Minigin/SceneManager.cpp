#include "SceneManager.h"
#include "Scene.h"

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
    // If this is the active scene, clear the pointer first
    if (m_ActiveScene && m_ActiveScene->GetName() == name) {
        m_ActiveScene = nullptr;
    }

    // Remove the scene
    m_scenes.erase(
        std::remove_if(m_scenes.begin(), m_scenes.end(),
            [&name](const std::shared_ptr<Scene>& scene) {
                return scene->GetName() == name;
            }),
        m_scenes.end()
    );
}



void dae::SceneManager::SetActiveScene(const std::string& name)
{
    m_ActiveScene = nullptr;
    for (auto& scene : m_scenes)
    {
        if (scene->GetName() == name)
        {
            m_ActiveScene = scene.get();
            break;
        }
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