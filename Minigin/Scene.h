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

		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

	private:
		std::string m_name;
		std::vector<std::shared_ptr<GameObject>> m_objects; 
	};
}
