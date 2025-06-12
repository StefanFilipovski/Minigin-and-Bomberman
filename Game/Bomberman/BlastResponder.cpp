#include "BlastResponder.h"
#include "GameObject.h"
#include "DestructibleWallResponder.h"
#include "BaseEnemyComponent.h"
#include "PlayerComponent.h"
#include "CollisionComponent.h"

namespace dae {

    BlastResponder::BlastResponder(Segment segment)
        : m_Segment(segment)
    {
    }

    void BlastResponder::OnCollide(GameObject* other)
    {
        if (!other)
            return;

        // Safety check
        if (other->IsMarkedForDeletion())
            return;

        // We don't handle destructible walls here anymore
        // They're handled during the explosion setup

        // Check for any enemy component derived from BaseEnemyComponent
        if (auto* enemy = other->GetComponent<BaseEnemyComponent>())
        {
            if (!enemy->IsDead()) {
                enemy->Die();
            }
        }

        // Player takes damage
        if (auto* player = other->GetComponent<PlayerComponent>())
        {
            player->TakeDamage(1);
        }

        // Note: We don't check for bombs here because BombCollisionResponder handles that
    }

} // namespace dae