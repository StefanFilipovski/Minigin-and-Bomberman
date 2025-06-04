#include "BlastResponder.h"
#include "GameObject.h"
#include "DestructibleWallResponder.h"
#include "BaseEnemyComponent.h"
#include "PlayerComponent.h"

namespace dae {

    BlastResponder::BlastResponder(Segment segment)
        : m_Segment(segment)
    {
    }

    void BlastResponder::OnCollide(GameObject* other)
    {
        if (!other)
            return;

        // If the other object has a destructible‐wall responder, trigger it
        if (auto* dwr = other->GetComponent<DestructibleWallResponder>())
        {
            dwr->OnCollide(other);
        }

        // Check for any enemy component derived from BaseEnemyComponent
        if (auto* enemy = other->GetComponent<BaseEnemyComponent>())
        {
            enemy->Die();
        }

        // Player takes damage (no invulnerability here)
        if (auto* player = other->GetComponent<PlayerComponent>())
        {
            player->TakeDamage(1);
        }

        // Static walls will simply block further explosion raycasts
    }

} // namespace dae