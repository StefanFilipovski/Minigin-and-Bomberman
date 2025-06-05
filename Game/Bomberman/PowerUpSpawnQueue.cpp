#include "PowerUpSpawnQueue.h"
#include "PowerUpFactory.h"

namespace dae {
    void PowerUpSpawnQueue::QueueSpawn(const glm::vec3& position, Scene* scene, float dropChance)
    {
        m_SpawnQueue.push_back({ position, scene, dropChance });
    }

    void PowerUpSpawnQueue::ProcessQueue()
    {
        for (const auto& request : m_SpawnQueue) {
            PowerUpFactory::GetInstance().CreateRandomPowerUp(
                request.position,
                request.scene,
                request.dropChance
            );
        }
        m_SpawnQueue.clear();
    }
}