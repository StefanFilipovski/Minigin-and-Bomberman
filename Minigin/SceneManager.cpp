#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update(float deltaTime) 
{
	for (auto& scene : m_scenes)
	{
		scene->Update(deltaTime); 
	}
}

void dae::SceneManager::Render()
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

dae::Scene& dae::SceneManager::CreateScene(const std::string& name)
{
	const auto& scene = std::shared_ptr<Scene>(new Scene(name));
	m_scenes.push_back(scene);
	return *scene;
}
void dae::SceneManager::SetActiveScene(const std::string& name)
{
    auto scene = GetScene(name);
    if (scene) {
        m_ActiveScene = scene;
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