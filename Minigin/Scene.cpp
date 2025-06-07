#include "Scene.h"
#include "GameObject.h"
#include <algorithm>

using namespace dae;

Scene::Scene(const std::string& name) : m_name(name) {}

void Scene::Add(std::shared_ptr<GameObject> object)  
{
	m_objects.push_back(std::move(object));
}

void Scene::Remove(std::shared_ptr<GameObject> object)
{
	m_objects.erase(std::remove(m_objects.begin(), m_objects.end(), object), m_objects.end());
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update(float deltaTime)
{
	for (auto& object : m_objects)
	{
		object->Update(deltaTime);
	}

	// Remove objects marked for deletion
	m_objects.erase(
		std::remove_if(m_objects.begin(), m_objects.end(),
			[](const std::shared_ptr<GameObject>& object) {
				return object->IsMarkedForDeletion();
			}),
		m_objects.end()
	);

}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}
