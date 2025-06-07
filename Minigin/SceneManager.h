#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Singleton.h"

namespace dae
{
	class Scene;
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene(const std::string& name);

		void Update(float deltaTime);  
		void Render();

		//Additional methods
		void SetActiveScene(const std::string& name);
		void SetActiveScene(Scene* scene);
		Scene* GetActiveScene() const { return m_ActiveScene; }
		Scene* GetScene(const std::string& name) const;
		void RemoveScene(const std::string& name);


	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		std::vector<std::shared_ptr<Scene>> m_scenes;
		Scene* m_ActiveScene{ nullptr };
	};
}
