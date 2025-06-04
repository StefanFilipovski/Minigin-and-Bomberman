#include "PowerUpComponent.h"
#include "PlayerComponent.h"
#include "GameObject.h"
#include "GameEvents.h"

namespace dae {
    PowerUpComponent::PowerUpComponent(GameObject* owner, PowerUpType type)
        : Component(owner), m_Type(type)
    {
    }

    void PowerUpComponent::OnPickup(PlayerComponent* player)
    {
        // Apply power-up to player
        switch (m_Type) {
        case PowerUpType::ExtraBomb:
            player->IncreaseBombCapacity();
            break;
        case PowerUpType::Detonator:
            player->EnableDetonator();
            break;
        case PowerUpType::FlameRange:
            player->IncreaseBombRange();
            break;
        }

        // Notify for UI/score
        Event powerUpEvent{ GameEvents::POWERUP_COLLECTED };
        Notify(powerUpEvent);

        // Remove the power-up
        GetOwner()->MarkForDeletion();
    }
}