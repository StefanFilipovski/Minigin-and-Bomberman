#include "PowerUpFactory.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "CollisionComponent.h"
#include "PowerUpComponent.h"
#include "PowerUpCollisionResponder.h"
#include "Scene.h"
#include <random>

namespace dae {
    void PowerUpFactory::CreatePowerUp(PowerUpType type, const glm::vec3& position, Scene* scene)
    {
        if (!scene) return;

        auto powerUpGO = std::make_shared<GameObject>();

        // Transform
        powerUpGO->AddComponent<TransformComponent>()
            .SetLocalPosition(position.x, position.y, position.z);

        // Render
        auto& rc = powerUpGO->AddComponent<RenderComponent>();
        switch (type) {
        case PowerUpType::ExtraBomb:
            rc.SetTexture("PowerUpBomb.tga");
            break;
        case PowerUpType::Detonator:
            rc.SetTexture("PowerUpDetonator.tga");
            break;
        case PowerUpType::FlameRange:
            rc.SetTexture("PowerUpFlame.tga");
            break;
        }

        // Power-up component
        auto& puc = powerUpGO->AddComponent<PowerUpComponent>(type);

        // Collision
        auto& cc = powerUpGO->AddComponent<CollisionComponent>();
        cc.SetSize(16.f, 16.f);
        cc.SetResponder(std::make_unique<PowerUpCollisionResponder>(&puc));

        // Add to scene
        scene->Add(powerUpGO);
    }

    void PowerUpFactory::CreateRandomPowerUp(const glm::vec3& position, Scene* scene, float dropChance)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_real_distribution<> chanceDist(0.0, 1.0);
        static std::uniform_int_distribution<> typeDist(0, 2);

        if (chanceDist(gen) > dropChance) {
            return; // No power-up
        }

        PowerUpType type = static_cast<PowerUpType>(typeDist(gen));
        CreatePowerUp(type, position, scene);
    }
}