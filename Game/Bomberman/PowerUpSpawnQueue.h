#pragma once
#include "Singleton.h"
#include <vector>
#include <ext/vector_float3.hpp>

namespace dae {
    class Scene;

    struct PowerUpSpawnRequest {
        glm::vec3 position;
        Scene* scene;
        float dropChance;
    };

    class PowerUpSpawnQueue : public Singleton<PowerUpSpawnQueue> {
    public:
        void QueueSpawn(const glm::vec3& position, Scene* scene, float dropChance = 0.3f);
        void ProcessQueue();

    private:
        friend class Singleton<PowerUpSpawnQueue>;
        PowerUpSpawnQueue() = default;

        std::vector<PowerUpSpawnRequest> m_SpawnQueue;
    };
}