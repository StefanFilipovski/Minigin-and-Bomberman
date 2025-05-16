#include "BlastResponder.h"
#include "GameObject.h"
#include "DestructibleWallResponder.h"
#include "BalloonComponent.h"
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

        if (auto* balloon = other->GetComponent<BalloonComponent>())
        {
            balloon->Die();
            // (no return, if you want the blast to keep propagating you can omit)
        }
      
        // 3) Player takes damage (no invulnerability here)
        if (auto* player = other->GetComponent<PlayerComponent>())
        {
            player->TakeDamage(1);
        }

        // Static walls will simply block further explosion raycasts
    }

} // namespace dae

