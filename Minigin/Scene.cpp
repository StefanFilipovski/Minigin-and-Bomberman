#include "Scene.h"
#include "GameObject.h"
#include <algorithm>

using namespace dae;

Scene::Scene(const std::string& name) : m_name(name) {}

void Scene::Add(std::shared_ptr<GameObject> object)
{
    if (!object) return;

    // If we're active, add to pending list
    // Otherwise, don't add new objects
    if (m_IsActive) {
        m_pendingObjects.emplace_back(std::move(object));
    }
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
    if (!m_IsActive) return;

    // Use index-based iteration to avoid iterator invalidation
    for (size_t i = 0; i < m_objects.size(); )
    {
        auto& object = m_objects[i];

        // Check if object should be removed
        if (!object || object->IsMarkedForDeletion())
        {
            // Remove and continue without incrementing i
            m_objects.erase(m_objects.begin() + i);
            continue;
        }

        // Update the object
        object->Update(deltaTime);

        // Only increment if we didn't erase
        ++i;
    }

    // Add pending objects only if we're still active
    if (m_IsActive && !m_pendingObjects.empty())
    {
        // Move pending objects to main list
        for (auto& pendingObject : m_pendingObjects)
        {
            if (pendingObject)
            {
                m_objects.emplace_back(std::move(pendingObject));
            }
        }
        m_pendingObjects.clear();
    }
}

void Scene::Render() const
{
	if (!m_IsActive) return;

	for (const auto& object : m_objects)
	{
		object->Render();
	}
}
