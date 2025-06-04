#pragma once
#include "CollisionResponder.h"
#include "PlayerComponent.h"
#include "BaseEnemyComponent.h"
#include "GameObject.h"

namespace dae {
    /// Handles collisions for AI enemies: player contact only
    class EnemyCollisionResponder final : public CollisionResponder {
    public:
        explicit EnemyCollisionResponder(BaseEnemyComponent* enemy)
            : m_Enemy(enemy)
        {
        }

        void OnCollide(GameObject* other) override
        {
            // Player collision → deal damage once
            if (auto* player = other->GetComponent<PlayerComponent>())
            {
                player->TakeDamage(1);
            }
        }

    private:
        BaseEnemyComponent* m_Enemy;
    };
}