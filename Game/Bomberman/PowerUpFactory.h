#pragma once
#include "Singleton.h"
#include <memory>
#include "PowerUpType.h"
#include <vec3.hpp>

namespace dae {
    class Scene;
    class GameObject;

    class PowerUpFactory : public Singleton<PowerUpFactory> {
    public:
        void CreatePowerUp(PowerUpType type, const glm::vec3& position, Scene* scene);
        void CreateRandomPowerUp(const glm::vec3& position, Scene* scene, float dropChance = 0.3f);

    private:
        friend class Singleton<PowerUpFactory>;
        PowerUpFactory() = default;
    };
}