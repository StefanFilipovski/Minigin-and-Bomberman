#pragma once
#include <vector>
#include <memory>
#include <string>

namespace dae
{
	class GameObject;
	class Scene final
	{
	public:
		explicit Scene(const std::string& name);
		~Scene() = default;

		void Add(std::shared_ptr<GameObject> object);  
		void Remove(std::shared_ptr<GameObject> object);
		void RemoveAll();

		void Update(float deltaTime);
		void Render() const;
		const std::string& GetName() const { return m_name; }

		void SetActive(bool active) { m_IsActive = active; }
		bool IsActive() const { return m_IsActive; }

		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		bool m_IsActive{ true };
		std::string m_name;
		std::vector<std::shared_ptr<GameObject>> m_objects; 
		std::vector<std::shared_ptr<GameObject>> m_pendingObjects;
	};
}
