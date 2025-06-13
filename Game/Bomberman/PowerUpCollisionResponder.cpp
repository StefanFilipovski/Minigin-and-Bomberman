#include "PowerUpCollisionResponder.h"
#include "PowerUpComponent.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include <CollisionComponent.h>
#include <SoundIds.h>
#include <ServiceLocator.h>

namespace dae {
    PowerUpCollisionResponder::PowerUpCollisionResponder(PowerUpComponent* powerUp)
        : m_PowerUp(powerUp)
    {
    }

    void PowerUpCollisionResponder::OnCollide(GameObject* other)
    {
        // 1) If we've already handled this power‐up, bail out
        if (!m_Active || m_PowerUp == nullptr || m_PowerUp->IsCollected())
            return;

        // 2) Check if 'other' is the player
        if (auto* player = other->GetComponent<PlayerComponent>())
        {
            // Grant the power-up to the player (this should NOT call MarkForDeletion() internally)
            m_PowerUp->Collect(player);

            // 3) Disable this responder immediately so we don’t re-enter OnCollide()
            m_Active = false;
            if (auto* cc = m_PowerUp->GetOwner()->GetComponent<CollisionComponent>())
            {
                cc->SetResponder(nullptr);
            }

            ServiceLocator::GetSoundSystem().Play(dae::SoundId::SOUND_POWERUP_PICKUP, 0.9f);

        }
    }
}