#pragma once
#include <vector>
#include "GameObject.h"

namespace dae
{
	class DebugUIManager
	{
	public:
		static DebugUIManager& GetInstance();
		static void Shutdown();

		void AddGameObject(GameObject* obj);
		void RemoveGameObject(GameObject* obj);
		void RenderUI();

	private:
		DebugUIManager() = default;
		~DebugUIManager() = default;
		DebugUIManager(const DebugUIManager&) = delete;
		DebugUIManager& operator=(const DebugUIManager&) = delete;

		std::vector<GameObject*> m_GameObjects;
	};
}


