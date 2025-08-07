#pragma once
#include <vector>
#include "GameObject.h"
#include "Singleton.h"

namespace dae
{
	class DebugUIManager final : public Singleton<DebugUIManager>
	{
	public:
		
		void AddGameObject(GameObject* obj);
		void RemoveGameObject(GameObject* obj);
		void Clear() { m_GameObjects.clear(); }  
		void RenderUI();

	private:
		DebugUIManager() = default;
		~DebugUIManager() = default;
		friend class Singleton<DebugUIManager>;

		std::vector<GameObject*> m_GameObjects;
	};
}


